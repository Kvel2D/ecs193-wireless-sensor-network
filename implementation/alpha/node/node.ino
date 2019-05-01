#include <SPI.h>
#include <EEPROM.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Adafruit_SleepyDog.h>

#include "sensor.h"
#include "tree_data.h"

struct Packet {
    float reading[NUM_SENSORS];
    uint32_t age;
    uint16_t number;
    uint8_t origin_id;
    uint8_t current_id;
};

#include "queue.h"

// NOTE: uncomment whichever id method you use
// Hardcode id
// uint8_t my_id = 0;
// Read id from EEPROM
uint8_t my_id = EEPROM.read(EEPROM.length() - 1);
NodeData my_data;
NodeData parent_data;

#define PRINT_DEBUG     false
#define WAIT_FOR_SERIAL false
#define RF69_FREQ       433.0
#define RFM69_CS        8
#define RFM69_INT       7
#define RFM69_RST       4
#define LED_PIN         13
#define LED_PERIOD      (60ul * 1000ul)
#define FAKE_SLEEP_DURATION     (60ul * 1000ul)

#define PACKET_PERIOD           (1000ul * 60ul * 5ul)
#define HEALTH_PACKET_PERIOD    (1000ul * 60ul * 60ul)
#define RX_RATE                 (600.0f)
#define TX_RATE                 (200.0f)

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, my_id);
float current_frequency = 0.0f;

uint32_t last_reading_time = 0;
uint32_t last_healthPacket_time = 0;
Queue packet_queue;
static int16_t packet_number = 0;
bool do_first_health_packet = true;
bool do_first_reading_packet = true;

void setup() {  
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);

    if (WAIT_FOR_SERIAL) {
        while (!Serial) {
            delay(1);
        }
    }

    // Read node and parent data
    memcpy_P(&my_data, &tree_data[my_id], sizeof(NodeData));
    if (my_data.parent != NO_ID) {
        memcpy_P(&parent_data, &tree_data[my_data.parent], sizeof(NodeData));
    }

    // NOTE: has to happen here right after serial connection is made
    if (my_data.has_sensor) {
        setup_sensors();
    }

    pinMode(2, OUTPUT); // set up for the sensor voltage
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);

    // reset radio
    // NOTE: is this needed?
    digitalWrite(RFM69_RST, HIGH);
    delay(10);
    digitalWrite(RFM69_RST, LOW);
    delay(10);

    if (!rf69_manager.init()) {
        while (true) {
            delay(1);
            Serial.println("RFM69 radio init failed");
        }
    }

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    // No encryption
    if (!rf69.setFrequency(RF69_FREQ)) {
        while (true) {
            delay(1);
            Serial.println("setFrequency failed");
        }
    }

    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    // power range is 14-20, 2nd arg must be true for 69HCW
    rf69.setTxPower(20, true);

    // The encryption key has to be the same as the one in the server
    uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);

    rf69_manager.setRetries(0);
    rf69_manager.setTimeout(10);

    last_reading_time = correct_millis();
}

int free_ram() {
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

float expovariate(float rate) {
    float k = -((float) random(0, RAND_MAX) / (RAND_MAX + 1));
    return -log(1.0f - k) / (1 / rate);
}

uint32_t convert_to_sleepydog_time(uint32_t time) {
    uint32_t remainder = time;
    remainder = remainder % 250;
    remainder = remainder % 15;
    uint32_t sleepydog_time = time - remainder;
    if (sleepydog_time < 15) {
        sleepydog_time = 15;
    }
    return sleepydog_time;
}

uint32_t millis_error = 0;
uint32_t correct_millis() {
    return millis() + millis_error;
}

void blink_led_periodically() {
    static uint32_t prev_time = 0;

    uint32_t current_time = millis();
    
    if (current_time - prev_time > LED_PERIOD) {
        prev_time = current_time;
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
    }
}

void print_packet(struct Packet p) {
    // NOTE: old printing with multiple prints, remove when sure that it's not needed
    // Serial.print(p.age);
    // Serial.print(",");
    // Serial.print(p.current_id);
    // Serial.print(",");
    // Serial.print(p.number);
    // Serial.print(",");
    // Serial.print(p.origin_id);
    // for (int i = 0; i < NUM_SENSORS; i++) {
    //     Serial.print(",");
    //     Serial.print(p.reading[i]);
    // }
    // NOTE: to print a float do: ("%d%01d", (int)(f), (int)(f * 100) % 100)

    // 1 uint32 (10 chars)
    // 3 uint16 (5 chars)
    // 6 floats (formatted as -123.45, so 7 chars)
    // 9 commas
    // null-terminator
    // 1 more char for leeway
    static char print_packet_buffer[10 + 3 * 5 + 6 * 6 + 9 + 1 + 1];
    int print_size = snprintf(print_packet_buffer, sizeof(print_packet_buffer), "%lu,%u,%u,%u,%d.%01d,%d.%01d,%d.%01d,%d.%01d,%d.%01d,%d.%01d", 
        p.age, 
        p.current_id, 
        p.number, 
        p.origin_id, 
        (int)(p.reading[0]), (int)(p.reading[0] * 100) % 100 / 10, 
        (int)(p.reading[1]), (int)(p.reading[1] * 100) % 100 / 10, 
        (int)(p.reading[2]), (int)(p.reading[2] * 100) % 100 / 10, 
        (int)(p.reading[3]), (int)(p.reading[3] * 100) % 100 / 10, 
        (int)(p.reading[4]), (int)(p.reading[4] * 100) % 100 / 10,
        (int)(p.reading[5]), (int)(p.reading[5] * 100) % 100 / 10);

    Serial.println(print_packet_buffer);

    if ((size_t)print_size > sizeof(print_packet_buffer)) {
        Serial.println("print_packet_buffer too small");
    }
}

void sleep(uint32_t sleep_time) {
    static uint32_t time_slept = 0;

    if (time_slept < FAKE_SLEEP_DURATION) {
        time_slept += sleep_time;
    }

    // NOTE: Don't sleep for real at the start so that serial doesn't disconnect and board can be reflashed easily
    if (my_data.parent == NO_ID || time_slept < FAKE_SLEEP_DURATION) {
        // Gateway node doesn't sleep to prevent serial disconnect
        delay(sleep_time);
    } else {
        // Other nodes sleep for real
        int sleep_time_left = (int)sleep_time;
        uint32_t time_before = millis();

        while (sleep_time_left > 0) {
            sleep_time_left -= Watchdog.sleep(sleep_time_left);
        }

        // Record clock error
        uint32_t perceived_sleep_time = millis() - time_before;
        if (perceived_sleep_time < sleep_time) {
            millis_error += sleep_time - perceived_sleep_time;
        }
    }
}

void loop_tx() {
    bool tx_success = false;
    uint32_t sleep_time = expovariate(TX_RATE);
    sleep(sleep_time);

    do {
        tx_success = false;

        // Turn on radio
        rf69.setModeIdle();

        if (packet_queue.size > 0) {
            Packet packet = packet_queue.front();

            // Transmit
            tx_success = rf69_manager.sendtoWait((uint8_t *) &packet, sizeof(Packet), my_data.parent);
        }

        if (tx_success) {
            Packet popped = packet_queue.pop();

            if (PRINT_DEBUG) {
                Serial.print("|TX,");
                print_packet(popped);
            }
        }
    } while (tx_success && packet_queue.size > 0);
    // Chain tx's until a failed transmit or queue is empty
}

void loop_rx() {
    static uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];

    uint32_t sleep_time = expovariate(RX_RATE);
    sleep(sleep_time);

    bool rx_success;
    do {
        rx_success = false;
        uint32_t start_time = millis();
    
        // Turn on radio
        rf69.setModeRx();

        // Clear buffer
        if (rf69_manager.available()) {
            uint8_t len = sizeof(buffer);
            rf69.recv(buffer, &len);
        }
    
        Packet p;
        while (millis() - start_time < 10) {
            if (rf69_manager.available()) {
                // Wait for a message addressed to us from the client
                uint8_t len = sizeof(buffer);
                uint8_t from;
                if (rf69_manager.recvfromAck(buffer, &len, &from)) {
                    // Turn off radio
                    rf69.sleep();
                    
                    // zero out remaining string
                    buffer[len] = 0; 

                    memcpy(&p, buffer, sizeof(Packet));
                    rx_success = true;

                    p.current_id = my_id;

                    // NOTE: Last node doesn't put packets into queue, they are "transferred" to gateway when packet is printer
                    if (my_data.parent != NO_ID) {
                        packet_queue.push(p);
                    }

                    break;
                }
            }
        }

        // Turn off radio
        rf69.sleep();
        
        // Only last node prints to serial
        if (rx_success && (my_data.parent == NO_ID || PRINT_DEBUG)) {
            if (PRINT_DEBUG) {
                Serial.print("|RX,");
            }
            print_packet(p);
        }
    } while (rx_success && packet_queue.size < QUEUE_SIZE_MAX);
    // Chain rx's until a failed receive or queue is full
}

void health_packet_generate() {
    if (do_first_health_packet || correct_millis() - last_healthPacket_time >= HEALTH_PACKET_PERIOD) {
        do_first_health_packet = false;
        
        Packet new_packet = {
            .reading = {(float)packet_queue.size,0.0f,0.0f,0.0f,0.0f,0.0f},
            .age = 0,
            .number = packet_number,
            .origin_id = my_id,
            .current_id = my_id,
        };

        if (my_data.parent == NO_ID) {
            // this is gateway, so print something instead of push
            print_packet(new_packet);
        } else {
            // Clear space by deleting older packets
            if (packet_queue.size == QUEUE_SIZE_MAX) {
                packet_queue.pop();
            }

            packet_queue.push(new_packet);
        }

        if (PRINT_DEBUG) {
            Serial.print("Health_Packet created: ");
            Serial.println(packet_number);
            print_packet(new_packet);
        }

        packet_number++;
        last_healthPacket_time = correct_millis();
    }
}

void updatePacketAge(uint32_t time) {
    if (packet_queue.size > 0) {
        for (size_t i = 0; i < QUEUE_SIZE_MAX; i++) {
            packet_queue.data[i].age += time;
        }
    }
}

void loop() {
    uint32_t loop_start_time = millis();

    // Do readings periodically if node has sensor
    if (my_data.has_sensor && (do_first_reading_packet || correct_millis() - last_reading_time >= PACKET_PERIOD)) {
        do_first_reading_packet = false;

        Packet new_packet = {
            .reading = {},
            .age = 0,
            .number = packet_number,
            .origin_id = my_id,
            .current_id = my_id,
        };

        read_temperatures(new_packet.reading);

        // Clear space by deleting older packets
        if (packet_queue.size == QUEUE_SIZE_MAX) {
            packet_queue.pop();
        }

        packet_queue.push(new_packet);

        if (PRINT_DEBUG) {
            Serial.print("Packet created: ");
            Serial.println(packet_number);
            print_packet(new_packet);
        }

        packet_number++;

        last_reading_time = correct_millis();
    } else if (my_id > 128) {
        health_packet_generate();
    }

    // Set frequency
    float expected_frequency = 0.0f;
    if (packet_queue.size > 0 && my_data.parent != NO_ID) {
        expected_frequency = parent_data.rx_frequency;
    } else {
        expected_frequency = my_data.rx_frequency;
    }
    if (current_frequency != expected_frequency) {
        rf69.setFrequency(expected_frequency);
        current_frequency = expected_frequency;
    }

    if (packet_queue.size > 0 && my_data.parent != NO_ID) {
        loop_tx();
    } else {
        loop_rx();
    }

    blink_led_periodically();

    // Update packet ages
    updatePacketAge(millis() - loop_start_time);
}

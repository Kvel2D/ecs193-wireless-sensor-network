#include <SPI.h>
#include <EEPROM.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sensor.h"

struct Packet {
    float reading[NUM_SENSORS];
    uint32_t age;
    uint16_t number;
    uint8_t origin_id;
    uint8_t current_id;
};

#include "queue.h"

#define ID_MAX      10
#define NO_ID     255

bool has_sensor[ID_MAX];
uint8_t parent_of[ID_MAX];
float rx_frequency[ID_MAX];

// NOTE: uncomment whichever id method you use
// Hardcode id
// uint8_t my_id = 0;
// Read id from EEPROM
uint8_t my_id = EEPROM.read(EEPROM.length() - 1);

bool init_data_arrays() {
    // NOTE: make sure that even when only using one node, the parent frequency is defined

    has_sensor[5] = true;
    parent_of[5] = 4;
    rx_frequency[5] = 433.0f;

    has_sensor[4] = false;
    parent_of[4] = 3;
    rx_frequency[4] = 434.0f;

    has_sensor[3] = false;
    parent_of[3] = NO_ID;
    rx_frequency[3] = 435.0f;
}

#define PRINT_DEBUG     true
#define RF69_FREQ       433.0
#define RFM69_CS        8
#define RFM69_INT       7
#define RFM69_RST       4
#define LED_PIN         13
#define LED_PERIOD      (60ul * 1000ul)

#define PACKET_PERIOD   (1000ul * 60ul)
#define RX_RATE      (600.0f)
#define TX_RATE      (200.0f)

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, my_id);
float current_frequency = 0.0f;

uint32_t total_sleep_time = 0;

// TX
uint32_t sensor_wakeup_time = 0;
int16_t packet_number = 0;
Queue packet_queue;
uint32_t start_time = millis();
uint8_t tx_time = 0;

// RX
uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];

void setup() {  
    init_data_arrays();

    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
    // while (!Serial) {
        // delay(1);
    // }

    // NOTE: has to happen here right after serial connection is made
    if (has_sensor[my_id]) {
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

    sensor_wakeup_time = millis();
}

void check_memory() {
    size_t size = 2048;
    void* ptr;
    while ((ptr = malloc(size)) == NULL && size >= 2) {
        size /= 2;
    }
    free(ptr);
    Serial.println(size);
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

    // printing float: ("%d%02d", (int)(f), (int)(f * 100) % 100)
    char print_buffer[100];
    sprintf(print_buffer, "%lu,%u,%u,%u,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d", p.age, p.current_id, p.number, p.origin_id, (int)(p.reading[0]), (int)(p.reading[0] * 100) % 100, (int)(p.reading[1]), (int)(p.reading[1] * 100) % 100, (int)(p.reading[2]), (int)(p.reading[2] * 100) % 100, (int)(p.reading[3]), (int)(p.reading[3] * 100) % 100, (int)(p.reading[4]), (int)(p.reading[4] * 100) % 100,(int)(p.reading[5]), (int)(p.reading[5] * 100) % 100);
    Serial.println(print_buffer);
}

void loop_tx() {
    bool tx_success = false;
    uint32_t sleep_time = expovariate(TX_RATE);
    sleep_time = convert_to_sleepydog_time(sleep_time);

    rf69.sleep();
    delay(sleep_time);
    total_sleep_time += sleep_time;

    uint32_t send_tx = millis();
    tx_time = 0;
    // Turn on radio
    rf69.setModeIdle();

    if (packet_queue.size > 0) {
        for (size_t i = 0; i < QUEUE_SIZE_MAX; i++) {
            packet_queue.data[i].age += sleep_time;
        }
        start_time = millis();
        Packet packet = packet_queue.front();

        // Transmit
        tx_success = rf69_manager.sendtoWait((uint8_t *) &packet, sizeof(Packet), parent_of[my_id]);

        tx_time = millis() - start_time;
    }

    uint32_t total_time = millis();
    uint32_t total_time_ms = total_time % 1000;
    total_time = total_time / 1000;

    // Serial.print(sleep_time); 
    // Serial.print(","); 
    // Serial.print(tx_time); 
    // Serial.print(","); 
    // Serial.print(total_time); 
    // Serial.print(","); 
    // Serial.print(total_time_ms);

    if (tx_success) {
        Packet popped = packet_queue.pop();

        total_time = millis();
        
        if (PRINT_DEBUG) {
            Serial.print("|tx-packet-stuff,");
            print_packet(popped);
        }
    }
}

void loop_rx() {
    uint32_t sleep_time = expovariate(RX_RATE);
    sleep_time = convert_to_sleepydog_time(sleep_time);
    delay(sleep_time);
    total_sleep_time += sleep_time;

    uint32_t start_time = millis();
    uint8_t available_loop = 10;
    uint32_t start_recv = 0;
    uint8_t recv_time = 0;
    bool rx_success = false;
    
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
            available_loop = millis() - start_time;

            // Wait for a message addressed to us from the client
            uint8_t len = sizeof(buffer);
            uint8_t from;
            start_recv = millis();
            if (rf69_manager.recvfromAck(buffer, &len, &from)) {
                // Turn off radio
                rf69.sleep();
                
                // zero out remaining string
                buffer[len] = 0; 

                memcpy(&p, buffer, sizeof(Packet));
                recv_time = millis() - start_recv;
                rx_success = true;

                p.current_id = my_id;

                // NOTE: Last node doesn't put packets into queue, they are "transferred" to gateway when packet is printer
                if (parent_of[my_id] != NO_ID) {
                    packet_queue.push(p);
                }

                break;
            }
        }
        available_loop = millis() - start_time;
    }

    // Turn off radio
    rf69.sleep();
    
    uint32_t total_time = millis();
    uint32_t total_time_ms = total_time % 1000;
    total_time =  total_time / 1000;

    // Serial.print(sleep_time); 
    // Serial.print(","); 
    // Serial.print(available_loop); 
    // Serial.print(","); 
    // Serial.print(recv_time); 
    // Serial.print(","); 
    // Serial.print(total_time); 
    // Serial.print(","); 
    // Serial.print(total_time_ms); 

    // Only last node prints to serial
    if (rx_success && (parent_of[my_id] == NO_ID || PRINT_DEBUG)) {
        if (PRINT_DEBUG) {
            Serial.print("|rx-packet-stuff,");
        }
        print_packet(p);
    }
}

void loop() {
    // Do readings periodically if node has sensor
    if (has_sensor[my_id]) {
        if (millis() - sensor_wakeup_time >= PACKET_PERIOD) {
            if (packet_queue.size < QUEUE_SIZE_MAX) {
                Packet new_packet = {
                    .reading = {},
                    .age = 0,
                    .number = packet_number,
                    .origin_id = my_id,
                    .current_id = my_id,
                };

                read_temperatures(new_packet.reading);

                packet_queue.push(new_packet);

                if (PRINT_DEBUG) {
                    Serial.print("Packet created: ");
                    Serial.println(packet_number);
                    print_packet(new_packet);
                }

                packet_number++;
            } else {
                if (PRINT_DEBUG) {
                    Serial.println("Queue full");
                }
            }
            sensor_wakeup_time = millis();
        }
    }

    // Set frequency
    float expected_frequency = 0.0f;
    if (packet_queue.size > 0 && parent_of[my_id] != NO_ID) {
        expected_frequency = rx_frequency[parent_of[my_id]];
    } else {
        expected_frequency = rx_frequency[my_id];
    }
    if (current_frequency != expected_frequency) {
        rf69.setFrequency(expected_frequency);
        current_frequency = expected_frequency;
    }

    if (packet_queue.size > 0 && parent_of[my_id] != NO_ID) {
        loop_tx();
    } else {
        loop_rx();
    }

    blink_led_periodically();
}

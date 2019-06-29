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
    uint16_t reading[NUM_SENSORS];
    uint16_t age;
    uint8_t number;
    uint8_t origin_id;
    uint8_t current_id;
};

#include "queue.h"
#include "duplicate.h"

// NOTE: uncomment whichever id method you use
// Hardcode id
// uint8_t my_id = 0;
// Read id from EEPROM
uint8_t my_id = EEPROM.read(EEPROM.length() - 1);
NodeData my_data;
NodeData parent_data;

#define PRINT_DEBUG     false
#define PRINT_STATE_DEBUG false
#define WAIT_FOR_SERIAL false
#define RF69_FREQ       433.0
#define RFM69_CS        8
#define RFM69_INT       7
#define RFM69_RST       4
#define LED_PIN         13
#define LED_PERIOD      (60ul * 1000ul)
#define REPORT_PERIOD   (60ul * 1000ul)

#define PACKET_PERIOD           (15ul * 60ul * 1000ul)
#define HEALTH_PACKET_PERIOD    (60ul * 60ul * 1000ul)
#define RX_RATE                 (600.0f)
#define TX_RATE                 (200.0f)
#define FAKE_SLEEP_DURATION     (60ul * 1000ul)
#define GATEWAY_ALWAYS_ON       false


RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, my_id);
Queue packet_queue;
float current_frequency = 0.0f;
uint8_t packet_number = 0;
typedef enum { Start, Receive, Transmit, Reading, HealthPacket } State;
State current_state = Start;
uint32_t next_reading = 0;
uint32_t next_health = 0;
uint32_t next_transmit = 0;
uint32_t next_receive = 0;
uint32_t current_tx_sleep = 0;
uint32_t current_rx_sleep = 0;
// need this boolean to tell when to generate a new tx time
bool transmit_valid = false;

float expovariate(float rate);
uint32_t convert_to_sleepydog_time(uint32_t time);

void setup() {  
    pinMode(LED_PIN, OUTPUT);

    delay(60ul * 1000ul);

    Serial.begin(115200);

    // Turn on LED while waiting for serial for debugging
    digitalWrite(LED_PIN, HIGH);

    if (WAIT_FOR_SERIAL || my_id == 254) {
        while (!Serial) {
            delay(1);
        }
    }

    digitalWrite(LED_PIN, LOW);

    // Read node and parent data
    memcpy_P(&my_data, &tree_data[my_id], sizeof(NodeData));
    if (my_data.parent != NO_ID) {
        memcpy_P(&parent_data, &tree_data[my_data.parent], sizeof(NodeData));
    }

    // NOTE: has to happen here right after serial connection is made
    if (my_data.has_sensor) {
        setup_sensors();
    }

    initialize_children_array();

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

    // generate health or reading packet on startup
    if (my_data.parent == NO_ID) {
        current_state = Receive;
    } else {
        if (!my_data.has_sensor) {
            current_state = HealthPacket;
            next_receive = expovariate(RX_RATE);
        } else {
            current_state = Reading;
        }
        // Both relay and sensor nodes will need a tx sleep
        next_transmit = expovariate(TX_RATE);
        transmit_valid = true;
    }
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

uint16_t compress_float(float f) {
    // NOTE: 0.0f is reserved for N/A value of -127.0f when no sensor is connected or sensor is faulty
    if (f == -127.0f) {
        f = 0.0f;
    } else {
        // Limit to 0.1f-99.0f range
        if (f < 0.1f) {
            f = 0.1f;
        } else if (f > 99.0f) {
            f = 99.0f;
        }
    }

    return (uint16_t) round(f / 99.0f * UINT16_MAX);
}

float decompress_float(uint16_t f) {
    return f * 99.0f / UINT16_MAX;
}

void blink_led_periodically() {
    static uint32_t prev_time = 0;

    uint32_t current_time = correct_millis();
    
    if (current_time - prev_time > LED_PERIOD) {
        prev_time = current_time;
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
    }
}


void report_periodically() {
    static uint32_t last_report_time = 0;
    uint32_t current_time = correct_millis();
    if (current_time - last_report_time > REPORT_PERIOD || current_state == Reading || current_state == HealthPacket) {
        last_report_time = current_time;
        Serial.print("Report: T: ");
        Serial.print(millis());
        Serial.print(" TX: ");
        Serial.print(next_transmit);
        Serial.print(" RX: ");
        Serial.print(next_receive);        
        Serial.print(" Rd: ");
        Serial.print(next_reading);
        Serial.print(" Ht: ");
        Serial.print(next_health);
        Serial.print(" TX_valid: ");
        Serial.println(transmit_valid);
    }
}

#define print_float(x) (int) (round(x * 10) / 10), ((int) round(x * 10.0f)) % 10

void print_packet(struct Packet p) {
    // 1 uint16 (5 chars)
    // 3 uint8 (3 chars)
    // 6 floats (formatted as 12.3, so 4 chars)
    // 9 commas
    // null-terminator
    // 1 more char for leeway
    static char print_packet_buffer[5 + 3 * 3 + 6 * 4 + 9 + 1 + 1];
    int print_size = snprintf(print_packet_buffer, sizeof(print_packet_buffer), "%u,%u,%u,%u,%d.%d,%d.%d,%d.%d,%d.%d,%d.%d,%d.%d", 
        p.age, 
        p.current_id, 
        p.number, 
        p.origin_id, 
        print_float(decompress_float(p.reading[0])),
        print_float(decompress_float(p.reading[1])),
        print_float(decompress_float(p.reading[2])),
        print_float(decompress_float(p.reading[3])),
        print_float(decompress_float(p.reading[4])),
        print_float(decompress_float(p.reading[5])));

    if (Serial) {
        Serial.println(print_packet_buffer);
        
        if ((size_t)print_size > sizeof(print_packet_buffer)) {
            Serial.println("print_packet_buffer too small");
        }
    }
}

void sleep(uint32_t sleep_time) {
    static uint32_t time_slept_total = 0;

    uint32_t intended_sleep_time = sleep_time;

    if (time_slept_total < FAKE_SLEEP_DURATION) {
        time_slept_total += sleep_time;
    }

    // NOTE: Don't sleep for real at the start so that serial doesn't disconnect and board can be reflashed easily
    if (my_data.parent == NO_ID || time_slept_total < FAKE_SLEEP_DURATION) {
        // Gateway node doesn't sleep to prevent serial disconnect
        delay(sleep_time);
    } else {
        // Other nodes sleep for real
        uint32_t time_before = correct_millis();

        // input to sleep function is only suggested sleep time, HW may sleep less
        while (sleep_time > INT16_MAX) {
            sleep_time -= Watchdog.sleep(INT16_MAX);
        }
        int sleep_time_int = sleep_time;
        while (sleep_time_int > 0) {
            sleep_time_int -= Watchdog.sleep(sleep_time_int);
        }

        // Record clock error
        uint32_t perceived_sleep_time = correct_millis() - time_before;
        if (perceived_sleep_time < intended_sleep_time) {
            millis_error += intended_sleep_time - perceived_sleep_time;
        }
    }
}

void loop_tx() {
    // Set frequency
    float expected_frequency = parent_data.rx_frequency;
    if (current_frequency != expected_frequency) {
        rf69.setFrequency(expected_frequency);
        current_frequency = expected_frequency;
    }
    // Turn on radio
    rf69.setModeIdle();
    bool tx_success = false;
    do {
        tx_success = false;

        if (packet_queue.size > 0) {
            Packet packet = packet_queue.front();

            if (my_data.has_sensor == false && packet.origin_id == my_id) {
                packet.reading[1] = compress_float((float)packet_queue.size);
            }

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
    rf69.sleep();  // turn off radio
}

void loop_rx() {
    if (packet_queue.size == QUEUE_SIZE_MAX) {
        return;
    }

    // Set frequency
    float expected_frequency = my_data.rx_frequency;
    if (current_frequency != expected_frequency) {
        rf69.setFrequency(expected_frequency);
        current_frequency = expected_frequency;
    }
    // Turn on radio
    rf69.setModeRx();
    static uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];
    bool rx_success;
    do {
        rx_success = false;
        uint32_t start_time = correct_millis();

        // Clear buffer
        if (rf69_manager.available()) {
            uint8_t len = sizeof(buffer);
            rf69.recv(buffer, &len);
        }

        Packet p;

        bool duplicate = false;
        while (correct_millis() - start_time < 10) {
            if (rf69_manager.available()) {
                // Wait for a message addressed to us from the client
                uint8_t len = sizeof(buffer);
                uint8_t from;
                if (rf69_manager.recvfromAck(buffer, &len, &from)) {
                    // zero out remaining string
                    buffer[len] = 0; 

                    memcpy(&p, buffer, sizeof(Packet));
                    rx_success = true;

                    // NOTE: Last node doesn't put packets into queue, they are "transferred" to gateway when packet is printer
                    duplicate = is_duplicate(p);
                    if (my_data.parent != NO_ID && !duplicate) {
                        p.current_id = my_id;
                        packet_queue.push(p);
                    } 

                    if (duplicate && PRINT_DEBUG) {
                        Serial.print("duplicate: ");
                        Serial.println(p.number);
                    }

                    break;
                }
            }
        }
        
        // Only last node prints to serial
        if (rx_success && (my_data.parent == NO_ID || PRINT_DEBUG) && (!duplicate)) {
            if (PRINT_DEBUG) {
                Serial.print("|RX,");
            }
            print_packet(p);
        }
    } while (rx_success && packet_queue.size < QUEUE_SIZE_MAX);
    // Chain rx's until a failed receive or queue is full
    rf69.sleep();  // Turn off radio
}

void health_packet_generate() {
    Packet new_packet = {
        .reading = {compress_float((float)packet_queue.size), 0, 0, 0, 0, 0},
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
}

void data_packet_generate() {
    Packet new_packet = {
        .reading = {},
        .age = 0,
        .number = packet_number,
        .origin_id = my_id,
        .current_id = my_id,
    };
    float reading_f[NUM_SENSORS];
    read_temperatures(reading_f);
    for (int i = 0; i < NUM_SENSORS; i++) {
        new_packet.reading[i] = compress_float(reading_f[i]);
    }
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
}

void increment_packet_age(uint32_t time) {
    // NOTE: don't need to reset ms counter because inaccuracy of <1s doesn't matter
    static uint16_t packet_queue_age_ms[QUEUE_SIZE_MAX];

    if (packet_queue.size > 0) {
        for (size_t i = 0; i < QUEUE_SIZE_MAX; i++) {
            packet_queue_age_ms[i] += (uint16_t) time;

            if (packet_queue_age_ms[i] > 1000) {
                uint16_t d_seconds = packet_queue_age_ms[i] / 1000;
                packet_queue_age_ms[i] -= d_seconds * 1000;

                // Don't overflow age
                if (packet_queue.data[i].age < UINT16_MAX - d_seconds) {
                    packet_queue.data[i].age += d_seconds;
                }
            }
        }
    }
}

void loop() {
    static uint32_t loop_start;
    static uint32_t loop_end;
    loop_start = correct_millis();
    switch (current_state) {
        case Reading: {
            data_packet_generate();
            break;
        }
        case HealthPacket: {
            health_packet_generate();
            break;
        }
        case Transmit: {
            loop_tx();
            // need to generate new tx time
            transmit_valid = false;
            break;
        }
        case Receive: {
            loop_rx();
            break;
        }
    }
    // For debug purposes
    blink_led_periodically();

    State next_state;
    // gateway always receives
    if (my_data.parent == NO_ID && GATEWAY_ALWAYS_ON) {
        next_state = Receive;
    } else {
        // Unified sleep
        // Generate new transmit time if packet_queue is not empty and we just
        // transmitted
        if (packet_queue.size > 0 &&
            (current_state == Transmit || !transmit_valid)) {
            next_transmit = expovariate(TX_RATE);
        transmit_valid = true;
    }
    // Generate new receive time if there's no sensors (isRelay)
    if (!my_data.has_sensor && current_state == Receive) {
        next_receive = expovariate(RX_RATE);
    }
    // Generate new reading generation time if there are sensors and we just
    // generated a data packet
    if (my_data.has_sensor && current_state == Reading) {
        next_reading = PACKET_PERIOD;
    }
    // Generate new health packet generation time if is relay and we just
    // generated a health packet
    if (!my_data.has_sensor && current_state == HealthPacket) {
        next_health = HEALTH_PACKET_PERIOD;
    }
    uint32_t next_time;
    if (my_data.has_sensor) {
        if (transmit_valid) {
                // transmit, or generate data
                // must not be <= or else it will always be in transmit mode while sleeping 0
                if (next_transmit < next_reading) {
                    next_time = next_transmit;
                    next_state = Transmit;
                } else {
                    next_time = next_reading;
                    next_state = Reading;
                }
            } else {
                next_time = next_reading;
                next_state = Reading;
            }
        } else {
            // transmit, receive, or generate health
            if (transmit_valid) {
                // prioritizes transmit
                if (next_transmit <= next_receive) {
                    next_time = next_transmit;
                    next_state = Transmit;
                } else {
                    next_time = next_receive;
                    next_state = Receive;
                }

                // prioritize health
                if (next_health <= next_time) {
                    next_time = next_health;
                    next_state = HealthPacket;
                }
            } else {
                // transmit time is not valid, do not consider next_transmit
                if (next_receive < next_health) {
                    next_time = next_receive;
                    next_state = Receive;
                } else {
                    next_time = next_health;
                    next_state = HealthPacket;
                }
            }
        }
        if (PRINT_STATE_DEBUG) {
            report_periodically();
        }
        if (PRINT_STATE_DEBUG) {
            switch (next_state) {
                case Transmit: {
                    Serial.print("Transmit: ");
                    break;
                }
                case Receive: {
                    Serial.print("Receive: ");
                    break;
                }
                case HealthPacket: {
                    Serial.print("Health: ");
                    break;
                }
                case Reading: {
                    Serial.print("Reading: ");
                    break;
                }
            }
            Serial.println(next_time);
        }
        sleep(next_time);

        // Update packet ages
        increment_packet_age(correct_millis() - loop_start);
        loop_end = correct_millis();
        uint32_t offset = loop_end - loop_start;
        if (next_transmit > offset) {
            next_transmit -= offset;
        } else {
            next_transmit = 0;
        }
        if (next_receive > offset) {
            next_receive -= offset;
        } else {
            next_receive = 0;
        }
        if (next_reading > offset) {
            next_reading -= offset;
        } else {
            next_reading = 0;
        }
        if (next_health > offset) {
            next_health -= offset;
        } else {
            next_health = 0;
        }
    }
    // Update packet ages
    current_state = next_state;
}

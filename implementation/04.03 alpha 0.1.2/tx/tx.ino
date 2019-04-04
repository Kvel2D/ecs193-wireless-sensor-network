#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "shared.h"

#define DEST_ADDRESS   1
#define MY_ADDRESS     2
#define PACKET_PERIOD (1000ul * 60ul * 5ul)
#define QUEUE_SIZE_MAX 20

struct Queue {
    Packet data[QUEUE_SIZE_MAX];
    uint8_t head = 0;
    uint8_t size = 0;

    void push(Packet packet) {
        if (size < QUEUE_SIZE_MAX - 1) {
            data[(head + size) % QUEUE_SIZE_MAX] = packet;
            size++;
        }
    }

    Packet front() {
        return data[head];
    }

    Packet pop() {
        if (size > 0) {
            uint8_t old_head = head;
            head = (head + 1) % QUEUE_SIZE_MAX;
            size--;

            return data[old_head];
        }
    }
};

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

uint32_t sensor_wakeup_time = 0;
int16_t packet_number = 0;
uint32_t total_sleep_time = 0;
char reading[5];
Queue packet_queue;

uint32_t start_time = millis();
uint8_t tx_time = 0;

void setup() 
{   
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
//    while (!Serial) {
//        delay(1);
//    }
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

    Serial.print("RFM69 radio @");  
    Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
    sensor_wakeup_time = millis();

    Packet new_packet = {
        .reading = read_temp(),
        .age = 0,
        .number = packet_number,
    };
    packet_queue.push(new_packet);
    packet_number++;
}

float read_temp(void){
    float v_out;
    float temp;
    digitalWrite(A0, LOW);
    // NOTE: can pull up specific pin instead of using 3V pin, probably the way to go
    digitalWrite(2, HIGH);
    delay(2);
    v_out = analogRead(0);
    digitalWrite(2, LOW);
    // NOTE: this is the original value, assume it comes from (5.0v / 1023), which is ADC to volts conversion
    // v_out *= .0048;
    // 3.3v / 1023 = 0.0032
    v_out *= .0032;
    v_out *= 1000;
    temp = 0.0512 * v_out - 20.5128;
    return temp;
} 

void loop() {
    bool tx_result = false;
    uint32_t sleep_time = expovariate(400.0f);
    sleep_time = convert_to_sleepydog_time(sleep_time);
    
    if (millis() - sensor_wakeup_time >= PACKET_PERIOD) {
        if (packet_queue.size < QUEUE_SIZE_MAX) {
            Packet new_packet = {
                .reading = read_temp(),
                .age = 0,
                .number = packet_number,
            };
            packet_queue.push(new_packet);

            Serial.print("Packet created: ");
            Serial.println(packet_number);
            packet_number++;
        } else {
            Serial.println("Queue full");
        }
        sensor_wakeup_time = millis();
    }

    rf69.sleep();
    delay(sleep_time);
    total_sleep_time += sleep_time;

    uint32_t send_tx = millis();
    tx_time = 0;
    rf69.setModeIdle();

    if (packet_queue.size > 0) {
        for (size_t i = 0; i < QUEUE_SIZE_MAX; i++) {
            // TODO: need to add awake time as well
            packet_queue.data[i].age += sleep_time;
        }
        start_time = millis();
        Packet packet = packet_queue.front();
        tx_result = rf69_manager.sendtoWait((uint8_t *) &packet, sizeof(Packet), DEST_ADDRESS);
        tx_time = millis() - start_time;
    }
    
    uint32_t total_time = millis();
    uint32_t total_time_ms = total_time % 1000;
    total_time = total_time / 1000;

    if (tx_result) {
        Serial.println("Send success");
    }

    Serial.print(sleep_time); 
    Serial.print("  "); 
    Serial.print(tx_time); 
    Serial.print("  "); 
    Serial.print(total_time); 
    Serial.print("  "); 
    Serial.print(total_time_ms);

    if (tx_result) {
        tx_result = false;

        Packet popped = packet_queue.pop();

        total_time = millis();
        
        Serial.print("  ");
        Serial.print(popped.age);
        Serial.print("  ");
        Serial.print(popped.number);
        Serial.print("  ");
        Serial.print(popped.reading);
    }

    Serial.println("");

    blink_led_periodically(sleep_time);
}

#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <QueueArray.h>

struct Packet {
    int id;
    float temperature;
};

#define RF69_FREQ 430.0 //915.0

#define DEST_ADDRESS   1

#define MY_ADDRESS     2

#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13

#define TIME_BETWEEN_READINGS 1000 * 5

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

QueueArray<Packet> queue;
int packet_id_max = 0;
unsigned long reading_timer = 0;

float expovariate(float rate) {
    float k = -(((float) random(0, RAND_MAX)) / (RAND_MAX + 1));
    return -log(1.0f - k) / (1 / rate);
}

void print_packet(Packet packet) {
    Serial.print("    packet id = ");
    Serial.println(packet.id);
    Serial.print("    packet temperature = ");
    Serial.println(packet.temperature);
}

void setup() {
    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
    // while (!Serial) {
    //     delay(1);
    // }
    pinMode(2, OUTPUT); // set up for the sensor voltage
    pinMode(LED, OUTPUT);     
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
    rf69_manager.setTimeout(6);

    Serial.print("RFM69 radio @");  
    Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

    reading_timer = millis();
}

long convert_to_sleepydog_time(long time) {
    long remainder = time;
    remainder = remainder % 250;
    remainder = remainder % 15;
    long sleepydog_time = time - remainder;
    if (sleepydog_time < 15) {
        sleepydog_time = 15;
    }
    return sleepydog_time;
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
    long sleep_time = expovariate(50.0f);
    sleep_time = convert_to_sleepydog_time(sleep_time);
    rf69.sleep();
    delay(sleep_time);

    rf69.setModeIdle();

    // Read temperature periodically
    if (millis() - reading_timer >= TIME_BETWEEN_READINGS) { 
        reading_timer = millis();

        if (queue.count() < 20) {
            float temperature = read_temp();
            Packet new_packet;
            new_packet.temperature = read_temp();
            new_packet.id = packet_id_max;
            packet_id_max++;
            queue.push(new_packet);

            Serial.println("New temperature reading.");
            print_packet(new_packet);
        }
    }

    if (!queue.isEmpty()) {
        char radiopacket[20];
        Packet packet = queue.peek();
        memcpy(radiopacket, &packet, sizeof(Packet));

        bool tx_result = rf69_manager.sendtoWait((uint8_t *)radiopacket, sizeof(Packet), DEST_ADDRESS);

        if (tx_result) {
            Packet packet = queue.pop();
            Serial.println("Successfully sent packet.");
            print_packet(packet);
        }
    }
}

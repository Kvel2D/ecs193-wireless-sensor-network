
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>

#define RF69_FREQ 434.0 //915.0

#define DEST_ADDRESS   1

#define MY_ADDRESS     2

#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int setup_end_time = 0;

void setup() 
{
    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
    // while (!Serial) {
    //     delay(1);
    // }

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
    rf69_manager.setTimeout(5);

    Serial.print("RFM69 radio @");  
    Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
    setup_end_time = millis();
}


float expovariate(float rate) {
    float k = -(((float) random(0, RAND_MAX)) / (RAND_MAX + 1));
    return -log(1.0f - k) / (1 / rate);
}

int16_t packet_number = 0;
long total_sleep_time = 0;
int successful_packet_count = 0;

void loop() {
    long sleep_time = (long)expovariate(50.0f);
    rf69.sleep();
    delay(sleep_time);
    total_sleep_time += sleep_time;

    rf69.setModeIdle();

    char radiopacket[20] = "Hello World #";
    itoa(packet_number, radiopacket + 13, 10);
    packet_number++;

    // Send and try to receive ack
    int tx_result = rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS);
    unsigned long end_tx = millis();

    if (tx_result) {
        successful_packet_count += 1;

        Serial.print("Send success");

        unsigned long total_time = millis() - setup_end_time;

        Serial.print("number of successful packet: ");
        Serial.println(successful_packet_count);
        Serial.print("total sleep time: ");
        Serial.println(total_sleep_time);
        Serial.print("total awake time: ");
        Serial.println(total_time - total_sleep_time);
        Serial.print("total time: ");
        Serial.println(total_time);
    } else {
        // Serial.println("Send fail (no ack)");
    }
}

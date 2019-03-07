
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>

#define RF69_FREQ 434.0

#define MY_ADDRESS     1

#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

unsigned long setup_end_time = 0;

void setup() 
{
    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
    // while (!Serial) {
    //     delay(1);
    // }

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
        Serial.println("setFrequency failed");
    }

    // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
    // ishighpowermodule flag set like this:
    // power range is 14-20, 2nd arg must be true for 69HCW
    rf69.setTxPower(20, true);

    // The encryption key has to be the same as the one in the server
    uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);


    Serial.print("RFM69 radio @");
    Serial.print((int)RF69_FREQ);
    Serial.println(" MHz");

    setup_end_time = millis();
}

float expovariate(float rate) {
    float k = -((float) random(0, RAND_MAX) / (RAND_MAX + 1));
    return -log(1.0f - k) / (1 / rate);
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

unsigned long total_sleep_time = 0;

int16_t packetnum = 0;
uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];

void loop() {
    // long sleep_time = expovariate(50.0f);
    // rf69.sleep();
    // delay(sleep_time);
    // total_sleep_time += sleep_time;

    rf69.setModeRx();

    unsigned long start_time = millis();

    while (millis() - start_time < 10) {
        if (rf69_manager.available()) {
            // Wait for a message addressed to us from the client
            uint8_t len = sizeof(buffer);
            uint8_t from;

            if (rf69_manager.recvfromAck(buffer, &len, &from)) {
                // zero out remaining string
                buffer[len] = 0; 

                // Serial.print("Got packet from #"); Serial.println(from);
                // Serial.print(" [RSSI :");
                // Serial.print(rf69.lastRssi());
                // Serial.print("] : ");
                // Serial.println((char*)buffer);

                // long total_time = millis() - setup_end_time;
                // Serial.print("total sleep time ="); Serial.println(total_sleep_time);
                // Serial.print("total awake time ="); Serial.println(total_time - total_sleep_time);
                // Serial.print("total time ="); Serial.println(total_time);

                break;
            }
        }
    }
}

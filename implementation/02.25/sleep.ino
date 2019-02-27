
#include <Adafruit_SleepyDog.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#define RF69_FREQ 434.0 //915.0

#define MY_ADDRESS     2

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);

    // manual reset
    digitalWrite(RFM69_RST, HIGH);
    delay(10);
    digitalWrite(RFM69_RST, LOW);
    delay(10);

    // Serial.begin(115200);
    // while(!Serial);
    // Serial.println("Adafruit Watchdog Library Sleep Demo!");
    // Serial.println();

    rf69_manager.init();

    rf69.setFrequency(RF69_FREQ);
    rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

    uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);
    rf69_manager.setRetries(0);
    rf69_manager.setTimeout(50);


    // Serial.println("Waiting 5 seconds");
    delay(5000);
    digitalWrite(LED_BUILTIN, LOW);

    // Serial.println("Setup over");
    // Serial.println("clout farming");
}

void test_8000() {
    Watchdog.sleep(8000);
}

void test_8000_with_delay() {
    delay(10);
    Watchdog.sleep(8000);
}

void test_15() {
    Watchdog.sleep(15);
}

void test_all() {
    Watchdog.sleep(8000);
    Watchdog.sleep(4000);
    Watchdog.sleep(2000);
    Watchdog.sleep(1000);
    Watchdog.sleep(500);
    Watchdog.sleep(250);
    Watchdog.sleep(120);
    Watchdog.sleep(60);
    Watchdog.sleep(30);
    Watchdog.sleep(15);
    Watchdog.sleep(15);
    Watchdog.sleep(30);
    Watchdog.sleep(60);
    Watchdog.sleep(120);
    Watchdog.sleep(250);
    Watchdog.sleep(500);
    Watchdog.sleep(1000);
    Watchdog.sleep(2000);
    Watchdog.sleep(4000);
    Watchdog.sleep(8000);
}

void loop() {
    delay(5000);

    rf69.sleep();
    Watchdog.sleep(8000);
}

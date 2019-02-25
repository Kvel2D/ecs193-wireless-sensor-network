
#include <Adafruit_SleepyDog.h>

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);
    while(!Serial);
    Serial.println("Adafruit Watchdog Library Sleep Demo!");
    Serial.println();

    Serial.println("Waiting 5 seconds");
    delay(5000);
    Serial.println("Setup over");
    Serial.println("clout farming");
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
}

void loop() {
    test_8000();
    // test_8000_with_delay();
    // test_15();
    // test_all();
}

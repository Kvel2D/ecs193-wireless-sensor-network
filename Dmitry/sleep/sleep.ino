
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


void loop() {
    // delay(1000);

    // Turn off led when going to sleep
    // digitalWrite(LED_BUILTIN, LOW);
    int sleep_left = 1543;

    while (sleep_left > 0) {
        // Serial.println("Going to sleep");
        // delay(5000);

        int actual_sleep_time = Watchdog.sleep(sleep_left);

        // Reattach USB connection
        // #ifdef USBCON
        // USBDevice.attach();
        // #endif

        // NOTE: wait before printing so that the terminal has time to reconnect
        // delay(10000);

        // Serial.print("Requested sleep duration = ");
        // Serial.println(sleep_left);
        // Serial.print("Actual sleep duration = ");
        // Serial.println(actual_sleep_time);
        
        sleep_left -= actual_sleep_time;

        // Serial.print("sleep left = ");
        // Serial.println(sleep_left);
        
    }

    // Turn on led when awake
    // digitalWrite(LED_BUILTIN, HIGH);

    // Reattach USB connection
    // #ifdef USBCON
    // USBDevice.attach();
    // #endif

    // NOTE: wait before printing so that the terminal has time to reconnect
    // delay(5000);

    // Serial.print("Slept for ");
    // Serial.print(sleep_time_ms, DEC);
    // Serial.println(" milliseconds.");
}

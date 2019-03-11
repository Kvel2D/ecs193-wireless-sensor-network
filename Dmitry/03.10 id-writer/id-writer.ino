#include <EEPROM.h>

#define ID 21

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(1);
    }

    // Write ID
    Serial.print("writing ID = ");
    Serial.println(ID);
    EEPROM.write(EEPROM.length() - 1, ID);

    // Serial.begin(115200);

    // Check ID
    uint8_t current_id = EEPROM.read(EEPROM.length() - 1);
    Serial.print("current ID = ");
    Serial.println(current_id);
}

void loop() {

}

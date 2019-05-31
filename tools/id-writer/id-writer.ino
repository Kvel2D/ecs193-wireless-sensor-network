#include <EEPROM.h>

// How to use:
// Write the ID that you need
// Upload the code to a board
// Open the serial monitor
// Recommended to label the board's id with tape

// Write ID here:
#define ID 133

void setup() {
    // Wait for serial monitor to open
    Serial.begin(115200);
    while (!Serial) {
        delay(1);
    }

    // Write ID
    Serial.print("writing ID = ");
    Serial.println(ID);
    EEPROM.write(EEPROM.length() - 1, ID);

    // Check ID
    uint8_t current_id = EEPROM.read(EEPROM.length() - 1);
    Serial.print("current ID = ");
    Serial.println(current_id);
}

void loop() {

}

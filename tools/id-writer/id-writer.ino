#include <EEPROM.h>

// How to use:
// Write the ID that you need
// Upload the code to a board
// Wait for red LED on the board to turn on
// Done!

// Write ID here:
#define ID 69


#define LED_PIN 13

void setup() {
    // Write ID
    EEPROM.write(EEPROM.length() - 1, ID);

    // Turn on red LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
}

void loop() {

}

#include <OneWire.h>

// this only print the serial number of one Sparkfun wtrprf ds18b20 sensor 

OneWire  ds(2);  // on pin 2 (a 4.7K resistor is necessary)

void setup(void) {
  Serial.begin(9600);

  byte i;
  byte addr[8];

    if (1) {
        while (!Serial) {
            delay(1);
        }
    }

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
  }
  else {
    Serial.print("ROM =");
    for( i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
  }
}

void loop(void) {
}

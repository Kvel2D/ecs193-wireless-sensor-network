
void setup(void) {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
}

float read_temp(void){
    float v_out;
    float temp;
    digitalWrite(A0, LOW);
    digitalWrite(2, HIGH);
    delay(2);
    v_out = analogRead(0);
    digitalWrite(2, LOW);
    v_out *= .0032;
    v_out *= 1000;
    temp = 0.0512 * v_out - 20.5128;
    return temp;
} 

void loop(void) {
  float reading = read_temp();

  Serial.println(reading);

  delay(1000);
}

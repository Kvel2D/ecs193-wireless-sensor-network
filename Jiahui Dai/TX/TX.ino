#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
// cite from Efstathios Chatzikyriakidis
// from https://playground.arduino.cc/Code/QueueArray
#include <QueueArray.h> 
//#include <queue.h> No such file or directory

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
unsigned long sensor_wakeup_time = 0;

void setup() 
{
    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
    while (!Serial) {
        delay(1);
    }
    pinMode(2, OUTPUT); // set up for the sensor voltage
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
    sensor_wakeup_time = millis();
}

float read_temp(void){
    float v_out;
    float temp;
    digitalWrite(A0, LOW);
    // NOTE: can pull up specific pin instead of using 3V pin, probably the way to go
    digitalWrite(2, HIGH);
    delay(2);
    v_out = analogRead(0);
    digitalWrite(2, LOW);
    // NOTE: this is the original value, assume it comes from (5.0v / 1023), which is ADC to volts conversion
    // v_out *= .0048;
    // 3.3v / 1023 = 0.0032
    v_out *= .0032;
    v_out *= 1000;
    temp = 0.0512 * v_out - 20.5128;
    
    return temp;
} 
class Packet{
    public:
        float reading;
        int packet_number;
        int packet_age;
        Packet(){reading = 0; packet_number = 0; packet_age = 0;}
        setAllValue(float temp_reading, int temp_packet_number, int temp_packet_age){
            reading = temp_reading;
            packet_number = temp_packet_number;
            packet_age= temp_packet_age;
        }
        setPacketAge(int temp_packet_age){
            packet_age = temp_packet_age;
        }
        void creatPacket(char* radiopacket){
            dtostrf(reading,5, 2, radiopacket);
            radiopacket[5] = ',';
            itoa(packet_number, radiopacket + 6, 10);
            radiopacket[(int)log10(packet_number) + 7] = ',';
            itoa(packet_age, radiopacket + 8 + (int)log10(packet_number), 10);
            
        }
};






float expovariate(float rate) {
    float k = -(((float) random(0, RAND_MAX)) / (RAND_MAX + 1));
    return -log(1.0f - k) / (1 / rate);
}

int16_t packet_number = 0;
long total_sleep_time = 0;
int successful_packet_count = 0;
char reading[5];
QueueArray <Packet> packet_queue;
int tx_result = 0;

void loop() {
    long sleep_time = (long)expovariate(50.0f);
    Packet new_packet;
    // Is the sendsor need few ms to warm up?
    if(millis()- sensor_wakeup_time >= 1000){ // 1000 = 1 sec
        // the sensor should wake up
        new_packet.setAllValue(read_temp(),packet_number, 0);
        packet_queue.push(new_packet);
        packet_number++;
        sensor_wakeup_time = millis();
        
    }
    
    rf69.sleep();
    delay(sleep_time);
    total_sleep_time += sleep_time;

    unsigned long send_tx = millis();
    rf69.setModeIdle();

    if(!packet_queue.isEmpty()){
        char radiopacket[62];
        // for this queue library, we must take packet out from the queue, so we can change value in the packet
        for(int i = packet_queue.count(); i>0; i--){
            new_packet = packet_queue.peek();
            packet_queue.pop();
            new_packet.setPacketAge(new_packet.packet_age + sleep_time);
            packet_queue.push(new_packet);
        }
        packet_queue.peek().creatPacket(radiopacket);
        //Serial.println(packet_queue.count());
        Serial.println(radiopacket);
        // Send and try to receive ack
        tx_result = rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS);
    }
    
    unsigned long end_tx = millis();
    //Serial.println(end_tx-send_tx);
    
    
    if (tx_result) {
        successful_packet_count += 1;
        packet_queue.pop();
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

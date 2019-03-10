#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
// cite from Efstathios Chatzikyriakidis
// from https://playground.arduino.cc/Code/QueueList
//#include <QueueList.h> 
#include <QueueArray.h>
//#include <queue.h> //No such file or directory

#define RF69_FREQ 434.0 //915.0

#define DEST_ADDRESS   1

#define MY_ADDRESS     2

#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13

#define PACKET_PERIOD 1000 * 15

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int setup_end_time = 0;
unsigned long sensor_wakeup_time = 0;
int16_t packet_number = 0;
long total_sleep_time = 0;
int successful_packet_count = 0;
char reading[5];
QueueArray <packet_t> packet_queue;

uint16_t sleep_log = 0;
unsigned long start_time = millis();
uint8_t tx_time = 0;
char log_output[32];

typedef struct packet_t {
    float reading;
    int packet_number;
    uint32_t packet_age;
} packet_t;

void setAllValue(struct packet_t *p, float temp_reading, int temp_packet_number, int temp_packet_age){
    p->reading = temp_reading;
    p->packet_number = temp_packet_number;
    p->packet_age= temp_packet_age;
}

void setPacketAge(struct packet_t *p, int temp_packet_age){
    p->packet_age = temp_packet_age;
}

float expovariate(float rate) {
    float k = -(((float) random(0, RAND_MAX)) / (RAND_MAX + 1));
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


void setup() 
{
    Serial.begin(115200);

    // Uncomment this to start running only after serial monitor is open
//    while (!Serial) {
//        delay(1);
//    }
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
    rf69_manager.setTimeout(6);

    Serial.print("RFM69 radio @");  
    Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
    setup_end_time = millis();
    sensor_wakeup_time = millis();

//    packet_t new_packet;
//    setAllValue(&new_packet, read_temp(),packet_number, 0);
//    packet_queue.push(new_packet);
//    packet_number++;
    
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


void loop() {
    bool tx_result = false;
    long sleep_time = (long)expovariate(100.0f);
    sleep_time = convert_to_sleepydog_time(sleep_time);
    sleep_log = (uint16_t) sleep_time;
    
    //Packet new_packet;
    packet_t new_packet;
    // Is the sendsor need few ms to warm up?
    if(millis() - sensor_wakeup_time >= PACKET_PERIOD){ // 1000 = 1 sec
        // the sensor should wake up
        //new_packet.setAllValue(read_temp(),packet_number, 0);
        //setAllValue(new_packet, read_temp(), packet_number, 0);
        //float temp = read_temp();
        //Serial.print("temperature: ");
        //Serial.println(temp);
        setAllValue(&new_packet, read_temp(), packet_number, 0);
        packet_queue.push(new_packet);
        packet_number++;
        sensor_wakeup_time = millis();
    }
    
    rf69.sleep();
    delay(sleep_time);
    total_sleep_time += sleep_time;

    unsigned long send_tx = millis();
    tx_time = 0;
    rf69.setModeIdle();

    if(!packet_queue.isEmpty()){
        char radiopacket[20]; // = "Hello World";
        // for this queue library, we must take packet out from the queue, so we can change value in the packet. Ex: packet_queue.peek().packet_age = 0 doesn't work
        // change the packet_age for every paclet inside the queue
        for(int i = packet_queue.count(); i>0; i--){
            new_packet = packet_queue.peek();
            packet_queue.pop();
            //new_packet.setPacketAge(new_packet.packet_age + sleep_time);
            setPacketAge(&new_packet, new_packet.packet_age + sleep_time);
            packet_queue.push(new_packet);
        }
        //Packet p = packet_queue.peek();
        packet_t p = packet_queue.peek();
        memcpy(radiopacket, &p, sizeof(packet_t));
        //packet_queue.peek().creatPacket(radiopacket);
        //Serial.println(packet_queue.count());
        //Serial.println(radiopacket);
        // Send and try to receive ack
        start_time = millis();
        tx_result = rf69_manager.sendtoWait((uint8_t *)radiopacket, sizeof(packet_t), DEST_ADDRESS);
        tx_time = millis() - start_time;
    }
    
    //Packet popped;
    packet_t popped;
    unsigned long total_time = millis() - setup_end_time;
    long total_time_ms = total_time % 1000;
    total_time = total_time / 1000;
    if (tx_result) {
        successful_packet_count += 1;
        popped = packet_queue.pop();
        Serial.println("Send success");

        total_time = millis() - setup_end_time;

        sprintf(log_output, "%u  %d  %lu  %u", sleep_log, tx_time, total_time, total_time_ms);
        Serial.print(log_output);
        Serial.print("  "); Serial.print(popped.packet_age);
        Serial.print("  "); Serial.print(popped.packet_number);
        Serial.print("  "); Serial.println(popped.reading);
        tx_result = false;
    } else {
        // Serial.println("Send fail (no ack)");
        sprintf(log_output, "%u  %d  %lu  %u", sleep_log, tx_time, total_time, total_time_ms);
        Serial.println(log_output);  
    
    }

    //sprintf(log_output, "%u  %d  %u", sleep_log, tx_time, total_time);
    //Serial.println(log_output);  
}

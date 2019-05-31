// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

//#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <RHGenericDriver.h>
#include <math.h>
#include <stdlib.h>
/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 434.0 //915.0

// Where to send packets to!
#define DEST_ADDRESS   1
// change addresses for each client board, any number :)
#define MY_ADDRESS     2


#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined(ADAFRUIT_FEATHER_M0) // Feather M0 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
  #define LED           13
#endif

#if defined(ESP8266)    // ESP8266 feather w/wing
  #define RFM69_CS      2    // "E"
  #define RFM69_IRQ     15   // "B"
  #define RFM69_RST     16   // "D"
  #define LED           0
#endif

#if defined(ESP32)    // ESP32 feather w/wing
  #define RFM69_RST     13   // same as LED
  #define RFM69_CS      33   // "B"
  #define RFM69_INT     27   // "A"
  #define LED           13
#endif

/* Teensy 3.x w/wing
#define RFM69_RST     9   // "A"
#define RFM69_CS      10   // "B"
#define RFM69_IRQ     4    // "C"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/
 
/* WICED Feather w/wing 
#define RFM69_RST     PA4     // "A"
#define RFM69_CS      PB4     // "B"
#define RFM69_IRQ     PA15    // "C"
#define RFM69_IRQN    RFM69_IRQ
*/

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);


int16_t packetnum = 0;  // packet counter, we increment per xmission

float expovariate(float rate) {
  float k = -(((float) random(0, RAND_MAX)) / (RAND_MAX + 1));
  return -log(1.0f - k) / (1 / rate);
  
}

float setup_end_time = 0;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  //Serial.println("Feather Addressed RFM69 TX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69_manager.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  rf69_manager.setRetries(0);
  
  // NOTE: from exp with rx, which takes about 2-4 ms, minimum tx timeout has to be ~5 
  // may have to be bigger
  // when timeout = 5, fails to receive ack very infrequently, similar to timeout = 50
  rf69_manager.setTimeout(5);
  
  pinMode(LED, OUTPUT);

  //Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
  setup_end_time = millis();
  Serial.println("one circle time, sleep_time, awake time, set_sleepTime, set_buffer, set_sendtoWait, set_send, set_randomTime_in_sendtoWait, set_waitAvailableTimeout, waitAvailableTimeout_within_numTimes");

}

// Dont put this on the stack:
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t data[] = "  OK";

float total_tx_time = 0;
long total_sleep_time = 0;
float total_time = 0;
int successful_pkt = 0;
float set_sleepTime = 0;
float set_buffer = 0;
float set_send = 0;
float set_sendtoWait = 0;
float set_randomTime_in_sendtoWait = 0;
int waitAvailableTimeout_within_numTimes = 0;
float set_waitAvailableTimeout = 0;
float temp = 0;
float loop_start = 0;
float one_circle_time = 0;

// Constructors
uint8_t my_seenIds[256];
uint32_t _retransmissions = 0;
uint8_t _lastSequenceNumber = 0;
uint16_t _timeout = 5;
uint8_t _retries = 0;
//memset( my_seenIds, 0, sizeof(my_seenIds));

void loop() 
{
  //reset
  set_sleepTime = 0;
  set_buffer = 0;
  set_send = 0;
  set_sendtoWait = 0;
  set_randomTime_in_sendtoWait = 0;
  set_waitAvailableTimeout = 0;
  waitAvailableTimeout_within_numTimes = 0;

  
  temp = loop_start = millis();
  long sleep_time = (long)expovariate(50.0f);
  total_sleep_time += sleep_time;
  set_sleepTime = millis() - temp; // JD
  delay(sleep_time);  // Wait 1 second between transmits, could also 'sleep' here!

  temp = 0; temp = millis();
  char radiopacket[20] = "Hello World #";
  itoa(packetnum++, radiopacket+13, 10);
  set_buffer = millis() - temp; // JD

  ////////////////////////////////////////////////////////////// sendtoWait ///////////////////////////////////////////////////
  unsigned long start_tx = millis();
  //int tx_ret = rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS);
  bool tx_ret = false;
  uint8_t* buf = (uint8_t *)radiopacket;
  uint8_t len = strlen(radiopacket); 
  uint8_t address = DEST_ADDRESS;
  // Assemble the message
  uint8_t thisSequenceNumber = ++_lastSequenceNumber;
  uint8_t retries = 0;
  while (retries++ <= _retries)
  {
    rf69.setHeaderId(thisSequenceNumber);
    rf69.setHeaderFlags(RH_FLAGS_NONE, RH_FLAGS_ACK); // Clear the ACK flag
    //sendto(buf, len, address);
    rf69.setHeaderTo(address);
    rf69.send(buf, len);
    rf69.waitPacketSent();
    
    // Never wait for ACKS to broadcasts:
    if (address == RH_BROADCAST_ADDRESS)
       return true;
    
    if (retries > 1)
       _retransmissions++;

    set_send = millis() - start_tx;
    unsigned long thisSendTime = millis(); // Timeout does not include original transmit time
    
    // Compute a new timeout, random between _timeout and _timeout*2
    // This is to prevent collisions on every retransmit
    // if 2 nodes try to transmit at the same time
    #if (RH_PLATFORM == RH_PLATFORM_RASPI) // use standard library random(), bugs in random(min, max)
    uint16_t timeout = _timeout + (_timeout * (random() & 0xFF) / 256);
    #else
    uint16_t timeout = _timeout + (_timeout * random(0, 256) / 256);
    #endif
    int32_t timeLeft;
  
    set_randomTime_in_sendtoWait = millis()- thisSendTime; // JD
    bool waitAvailableTimeout_result = false;
    while ((timeLeft = timeout - (millis() - thisSendTime)) > 0)
    {
      ///////////////////////////////////////////////////////////////////////// waitAvailableTimeout //////////////////////////////////////////////////////////
      bool available_result = false;
      unsigned long starttime = millis();
      while ((millis() - starttime) < timeLeft)
      {
        float available_start = millis();
        int mode_isTX = 0;
        waitAvailableTimeout_within_numTimes++; // JD
        //if (rf69.available(available_start, mode_isTX))
        if (rf69.available())
        {
          waitAvailableTimeout_result = true;
        }
        YIELD;
      }
      set_waitAvailableTimeout = millis() - starttime; // JD
      ///////////////////////////////////////////////////////////////////////// end waitAvailableTimeout //////////////////////////////////////////////////////////
      //if (waitAvailableTimeout(timeLeft))
      if (waitAvailableTimeout_result)
      {
        tx_ret = true;
        uint8_t from, to, id, flags;
        bool recvfrom_result = false;
        if (rf69.recv(buf, len))
        {
          if (from)  from =  rf69.headerFrom();
          if (to)    to =    rf69.headerTo();
          if (id)    id =    rf69.headerId();
          if (flags) flags = rf69.headerFlags();
          recvfrom_result = true;
        }
        //if (recvfrom(0, 0, &from, &to, &id, &flags)) // Discards the message
        if (recvfrom_result)
        {
          // Now have a message: is it our ACK?
          //if (   from == address && to == _thisAddress && (flags & RH_FLAGS_ACK) && (id == thisSequenceNumber))
          if (   from == address && to == 0 && (flags & RH_FLAGS_ACK) && (id == thisSequenceNumber))
          {
            // Its the ACK we are waiting for
            tx_ret = true;
          }
          else if (   !(flags & RH_FLAGS_ACK)&& (id == my_seenIds[from]))
          {
            // This is a request we have already received. ACK it again
            //acknowledge(id, from);
            rf69.setHeaderId(id);
            rf69.setHeaderFlags(RH_FLAGS_ACK);
            uint8_t ack = '!';
            //rf69.sendto(&ack, sizeof(ack), from); 
            rf69.setHeaderTo(from);
            rf69.send(&ack, sizeof(ack));
            rf69.waitPacketSent();
          }
          // Else discard it
        }
      }
      // Not the one we are waiting for, maybe keep waiting until timeout exhausted
      YIELD;
    }
    // Timeout exhausted, maybe retry
    YIELD;
  }
  // Retries exhausted
  set_sendtoWait = millis() - start_tx;
  ////////////////////////////////////////////////////////////////////////// end sendtoWait //////////////////////////////////////////////////
  unsigned long end_tx = millis();
  unsigned long tx_time = end_tx - start_tx;
  one_circle_time = millis() - loop_start;
  //Serial.print(mode_isTX);Serial.print(", ");
  Serial.print(one_circle_time);Serial.print(", ");
  Serial.print(sleep_time);Serial.print(", ");
  Serial.print(one_circle_time - sleep_time);Serial.print(", ");
  Serial.print(set_sleepTime);Serial.print(", ");
  Serial.print(set_buffer);Serial.print(", ");
  Serial.print(set_sendtoWait);Serial.print(", ");
  Serial.print(set_send);Serial.print(", ");
  Serial.print(set_randomTime_in_sendtoWait);Serial.print(", ");
  Serial.print(set_waitAvailableTimeout);Serial.print(", ");
  Serial.println(waitAvailableTimeout_within_numTimes);  
  if (tx_ret) 
  {
    Serial.print("tx_time: "); Serial.println(tx_time);
    successful_pkt += 1;
  }

  /*
  else 
  {
    Serial.println("Sending failed (no ack)");
  }
  Serial.print("number of successful packet: "); Serial.println(successful_pkt);
  total_tx_time += tx_time;
  Serial.print("total sleep time: "); Serial.println(total_sleep_time);
  total_time = millis() - setup_end_time;
  Serial.print("total awake time: "); Serial.println(total_time - total_sleep_time);
  Serial.print("total time: "); Serial.println(total_time);
  */
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

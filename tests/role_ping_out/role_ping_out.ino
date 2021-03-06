/*
   Dec 2014 - TMRh20 - Updated
   Derived from examples by J. Coliz <maniacbug@ymail.com>
*/
/**
 * Example for efficient call-response using ack-payloads 
 * 
 * This example continues to make use of all the normal functionality of the radios including 
 * the auto-ack and auto-retry features, but allows ack-payloads to be written optionlly as well. 
 * This allows very fast call-response communication, with the responding radio never having to 
 * switch out of Primary Receiver mode to send back a payload, but having the option to switch to 
 * primary transmitter if wanting to initiate communication instead of respond to a commmunication. 
 */
 
#include <SPI.h>
#include "RF24.h"
#include "printf.h"


RF24 radio(9,10);



                                                                           // Topology
byte addresses[][6] = {"1Node","2Node"};              // Radio pipe addresses for the 2 nodes to communicate.


byte counter = 1;                                                          // A single byte to keep track of the data being sent back and forth


char bufferz[33] = "12345678901234567890123456789012";
char bufferx[33];

void setup(){
  Serial.begin(115200);
  Serial.println(F(">>>>>>>>  ping out role <<<<<<<"));
  Serial.println(" ");

  printf_begin();
  // Setup and configure radio

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  //radio.setDataRate(RF24_2MBPS);  
  radio.setRetries(15,15);
  radio.enableAckPayload();                     // Allow optional ack payloads
  radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads
  
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.printDetails();
  
}

void loop(void) {

  
/****************** Ping Out Role ***************************/


    int rd;
    byte gotByte;                                           // Initialize a variable for the incoming response
    
    radio.stopListening();                                  // First, stop listening so we can talk.      
    Serial.print(F("Now sending "));                         // Use a simple byte counter as payload
    Serial.println(counter);
    
    unsigned long time = micros();                          // Record the current microsecond count   
    bufferz[31] += 1;                                                        
    if ( radio.write(&bufferz,32) ){                         // Send the counter variable to the other radio 
        if(!radio.available()){                             // If nothing in the buffer, we got an ack but it is blank
            Serial.print(F("Blank. round-trip delay: "));
            Serial.print(micros()-time);
            Serial.println(F(" microseconds"));     
        }else{      
            while(radio.available() ){                      // If an ack with payload was received
                radio.read( &bufferx, 32 );                  // Read it, and display the response time
                unsigned long timer = micros();



                Serial.print(F("Got response "));
                Serial.print(gotByte);
                                for(int i=0; i<32; i++){
                  Serial.print(bufferx[i]);
                  
                }
                Serial.println(" ");
                //Serial.print(F(" round-trip delay: "));
                //Serial.print(timer-time);
                //Serial.println(F(" microseconds"));
                counter++;                                  // Increment the counter variable
            }
        }
    
    }else{        Serial.println(F("Sending failed."));           // If no ack response, sending failed
   }
 }



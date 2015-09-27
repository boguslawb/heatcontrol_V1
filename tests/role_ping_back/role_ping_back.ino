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
#include <DigitalIO.h>
#include "RF24.h"
#include "printf.h"




RF24 radio(5,6);


                                                                           // Topology
byte addresses[][6] = {"1Node","2Node"};              // Radio pipe addresses for the 2 nodes to communicate.


byte counter = 1;                                                          // A single byte to keep track of the data being sent back and forth


char bufferz[33] = "12345678901234567890123456789012";
char bufferx[33];

void setup(){

  Serial.begin(115200);
  Serial.println(F(">>>>>>>> pong back role <<<<<<<<<<"));
  printf_begin();
  // Setup and configure radio

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  //radio.setDataRate(RF24_2MBPS);  
  radio.setRetries(15,15);
  radio.enableAckPayload();                     // Allow optional ack payloads
  radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads
  
  
  radio.openWritingPipe(addresses[1]);        // Both radios listen on the same pipes by default, but opposite addresses
  radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
  
  radio.startListening();                       // Start listening  
  
  radio.writeAckPayload(1,&bufferz,32);          // Pre-load an ack-paylod into the FIFO buffer for pipe 1
  radio.printDetails();

  counter = 1;
  
  
}



void loop(void) {


/****************** Pong Back Role ***************************/

    byte pipeNo, gotByte;                          // Declare variables for the pipe and the byte received
    while( radio.available(&pipeNo)){              // Read all available payloads
      radio.read( &bufferx, 32 );                   
                                                   // Since this is a call-response. Respond directly with an ack payload.
      //gotByte += 1; 
      bufferz[31] += 1;                             // Ack payloads are much more efficient than switching to transmit mode to respond to a call
      radio.writeAckPayload(pipeNo,&bufferz, 32 );  // This can be commented out to send empty payloads.
      Serial.print(F("Loaded next response "));
                      for(int i=0; i<32; i++){
                  Serial.print(bufferz[i]);
                  
                }
                Serial.println(" ");
   }
 
       
}

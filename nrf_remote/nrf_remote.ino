 /*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example LED Remote
 *
 * This is an example of how to use the RF24 class to control a remote
 * bank of LED's using buttons on a remote control.
 *
 * On the 'remote', connect any number of buttons or switches from
 * an arduino pin to ground.  Update 'button_pins' to reflect the
 * pins used.
 *
 * On the 'led' board, connect the same number of LED's from an
 * arduino pin to a resistor to ground.  Update 'led_pins' to reflect
 * the pins used.  Also connect a separate pin to ground and change
 * the 'role_pin'.  This tells the sketch it's running on the LED board.
 *
 * Every time the buttons change on the remote, the entire state of
 * buttons is send to the led board, which displays the state.
 */
#include "analog.h"

    //usb down!
analog a0(0); //right 0 up
analog a1(1); //not working?
analog a2(2); //left 255 up
analog a3(3); //turn


#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CS)

RF24 radio(8, 9);
//RF24 radio(10, 9 );

// sets the role 1 rx, 2 tx
const int role_remote = 1;
const int role_led = 2;

const int role = role_remote;

// Pins on the remote for buttons
const uint8_t num_buttons = 4;

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "rx Board"};

//
// Payload
//


uint16_t button_states[num_buttons];
uint8_t led_states[num_buttons];
const uint8_t msgSize = sizeof(button_states);

//analog
int xval=0, yval =0;

//
// Setup
//

void setup(void)
{
  
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  
  printf("\n\rRF24/examples/led_remote/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);    //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setDataRate(RF24_250KBPS);
   //speed  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps

  // Open pipes to other nodes for communication
  //

  // This simple sketch opens a single pipes for these two nodes to communicate
  // back and forth.  One listens on it, the other talks to it.

  if ( role == role_remote )
  {
    radio.openWritingPipe(pipe);
  }
  else
  {
    radio.openReadingPipe(1,pipe);
  }

  //
  // Start listening
  //

  if ( role == role_led )
    radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();

  //
  // Set up buttons / LED's
  //

  // Set pull-up resistors for all buttons
  if ( role == role_remote )
  {
   
  }

  // Turn LED's ON until we start getting keys
  if ( role == role_led )
  {
    
  }

}

//
// Loop
//

void loop(void)
{
  //
  // Remote role.  If the state of any button has changed, send the whole state of
  // all buttons.
  //

  if ( role == role_remote )
  {
    
    button_states[0] = a0.getVal();
    button_states[1] = a1.getVal();
    button_states[2] = a2.getVal();
    button_states[3] = a3.getVal();
 
    
      printf("Now sending...%d %d %d %d ", button_states[0], button_states[1], button_states[2], button_states[3]);
      bool ok = radio.write( button_states, msgSize );
      if (ok)
        printf("ok\n\r");
      else
        printf("failed\n\r");


    // Try again in a short while
//    delay(10);
  }

}

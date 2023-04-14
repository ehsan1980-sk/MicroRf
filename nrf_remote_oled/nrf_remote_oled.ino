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
 


//#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CS)

//RF24 radio(10, 9); //rf nano
RF24 radio(8,9 );

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


// oled stuff

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

#include "analog.h"

    //usb down!
analog a0(0); //right 0 up
analog a1(1); //not working?
analog a2(2); //left 255 up
analog a3(3); //turn

//start/title screen screen
void mRec(){ 
    display.drawRect(1, 1, display.width()-2, display.height()-4, SSD1306_WHITE);
    
    display.setTextSize(3);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(6,4);             // Start at top-left corner
    display.println("PiPico");
    display.setCursor(15,20);    
    display.println("Clock");
    display.display();
}

 
bool showtime = false;  //show time or animation
bool pacd = false;  //anamation flag
int stepsize = 11;  //width of dots//jump width
int tt = 0, stage = 0;    //tik //level
int astart = 0, aend = 1, asteps = 15, apausetik =0;

void mTri(int x ) {
    int w = 23, h = 5+(x%2)*15, y = 30, xx = stepsize*x;
    display.fillTriangle( xx+3, y, xx+w, y+h, xx+w, y-h, SSD1306_BLACK);
   
}
void mdrawCircle(int x, int y, int d){ 
    display.fillCircle(x, y, d, SSD1306_WHITE);  
    display.display();
}

void drawPac(int x){
      int y = 30;
      int i = x-1;
      mdrawCircle((i)*stepsize, 30, 22);
      display.fillCircle(stepsize*i+7, y-11, 2, SSD1306_BLACK);  
      mTri(i); 
      display.display(); 
}
//left, right, steps, stage counter
void pac(){ 
    if(++tt < 10000) return;    //end here
     
      tt = 0;  //draw dots... 
          display.clearDisplay(); 
      if(stage ==0 and aend++ > asteps){
        stage = 1;
        for(int i=astart; i<aend; i++)
        mdrawCircle(11+i*stepsize, 32, 1);
      
      } else if(stage == 1 and astart++ <aend){
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1); 
           if(astart>asteps/2) stage = 2;
      }else if(stage == 2){    //pause keep chomping
          if(apausetik++ > asteps/2){ 
              stage = 3;
              apausetik = 0;
          }
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1);
           
      } else if(stage == 3 and astart++ <=asteps){    //pause keep chomping
          if(astart>= asteps) stage = 4;
          display.clearDisplay(); 
          drawPac(astart);
         for(int i=astart; i<aend; i++)
           mdrawCircle(11+i*stepsize, 32, 1);
           
      } else if(stage == 4){
         if(apausetik++ > asteps/2){ 
              stage = 5;
              apausetik = 0;
          }
          testdrawoled("ACME", "00");    
      } else if(stage == 5){
         if(apausetik++ > asteps/2){ 
              stage = 6;
              apausetik = 0;
          }
          testdrawoled("Remote", "01");    
      }else if(stage == 6){
         showtime = true;
         astart= 0;
         aend = 1;
         pacd = false;
      }
}
void playPac(){
  pacd = true;
  showtime = false;
  stage = 0;
}

//draw oled/ 2 lines
void testdrawoled( String currentTime, String line2) {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(currentTime); 
//  display.println(currentTime.substring(0,4) ); //only 4 chars wide on the main line...
    
  display.setTextSize(2); 
  display.setCursor(0,17);             // Start at top-left corner
  display.println(line2);
  //add day/date in the empty space
  display.display(); 
}

//end oled stuff

void setup(void)
{
  
  //
  // Print preamble
  //
//  Serial.begin(57600); 
//  printf_begin();
//  delay(222);
//  for(int i=0; i<111; i++) if(Serial) break;
////  while(!Serial);
//  Serial.println("Acme rf TEST");
//  printf("\n\rRF24/examples/led_remote/\n\r");
//  printf("ROLE: %s\n\r",role_friendly_name[role]);

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

//  radio.printDetails();

  //
  // Set up buttons / LED's
  //

  // Set pull-up resistors for all buttons
  if ( role == role_remote )
  {
   
  
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
//    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(100); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  testdrawoled("ACME", "MicroRF"); //CALLS DISPLAY()
   
  delay(1000);
  display.clearDisplay();  
  mRec();  //sbow title screen

   for(int i=0; i<11; i++){
     a0.getVal();
     a1.getVal();
     a2.getVal();
     a3.getVal();
   }
  }

  // Turn LED's ON until we start getting keys
  if ( role == role_led )
  {
    
  }

}

//
// Loop
//

int a, b, c, d;
long lst =0;
int hz = 0, hzz = 0;

void loop(void)
{
  //
  // Remote role.  If the state of any button has changed, send the whole state of
  // all buttons.
  //

    
  if ( role == role_remote )
  {

    if(millis()-lst > 10000){
      lst = millis();
      hz = tt;
//    if(++tt > 100) 
    tt = 0;
    }
    tt++;
    if(tt%4==0) a = a0.getVal();
    else if(tt%4==1) b = a1.getVal();
    else if(tt%4==2) c = a2.getVal();
    else if(tt%4==3) d = a3.getVal();
//    pac();
    button_states[0] = a;
    button_states[1] = b;
    button_states[2] = c;
    button_states[3] = d;
    
    
    
//      printf("Now sending...%d %d %d %d ",a, b, c, d);
      bool ok = radio.write( button_states, msgSize );
//      if (ok)
//        printf("ok\n\r");
//      else
//        printf("failed\n\r");

    
    if(tt%3 ==0) {
     //update the screen less...then the radio.
//      tt = 0;  //draw dots... 
    char l1[12], l2[12] ;
    sprintf(l1,"%03d %03d %02d", c, a, hz/10); 
    sprintf(l2,"%03d %03d %d", d, b, ok);
    testdrawoled(l1, l2);
    }
    
    // Try again in a short while
//    delay(10);
  }

}


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <Servo.h>

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CS)
// for nano tx remote
//RF24 radio( 10, 9);

RF24 radio( 8,9);
 
//RF24 radio( 8,10);
//for mega rx reciever
//RF24 radio(2, 3);

// sets the role 1 rx, 2 tx
const int role_remote = 1;
const int role_led = 2;

const int role = role_led;

// Pins on the remote for buttons
const uint8_t num_buttons = 4;

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote",  "rx Board"};

//
// Payload
//

uint16_t button_states[num_buttons];
const uint8_t msgSize = sizeof(button_states);
uint8_t ledPins[num_buttons];

Servo one, two, three, four;
//
// Setup
//
int timeOut = 300;
long last = 0;

void setup(void)
{
  

  Serial.begin(57600);
  printf_begin();
  for(int i=0; i<111; i++) if(Serial) break;
  printf("\n\rRF24/examples/led_remote/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);    //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setDataRate(RF24_250KBPS);
   //speed  RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps

  //
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


 
  //esc.ini();
 //pinMode(2, OUTPUT);
  one.attach(4, 1000, 2000);
  two.attach(5, 1000, 2000);
  three.attach(6, 1000, 2000);
  four.attach(10, 1000, 2000);
 
  
}

//
// Loop
//

void loop(void)
{
  //neutral both channels for no data for a second
  long now = millis() - last;
  if(now >= timeOut){
            printf("Timed out: %d \n\r", now);
     one.writeMicroseconds(1500);
     two.writeMicroseconds(1500);
     three.writeMicroseconds(1500);
     four.writeMicroseconds(1500);
     last = millis();
  }
 delay(20);
 

  if ( role == role_led )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
        radio.read( button_states, msgSize );

          int x = button_states[0];
           int y = button_states[3];
             x = map(x, 0, 255, 1000, 2000);
             y = map(y, 0, 255, 1000, 2000);
             
             
          one.writeMicroseconds(x);
          two.writeMicroseconds(y);
          
          int a = button_states[1];
           int b = button_states[2];
             a = map(a, 0, 255, 1000, 2000);
             b = map(b, 0, 255, 1000, 2000);
             
             
          three.writeMicroseconds(a);
          four.writeMicroseconds(b);
//          int a = button_states[2];
//            analogWrite( pPwm, a);
//            a = button_states[3];
//            analogWrite( pMosfet, a);
            
//          int s, d;
//          int m  = 200, p = 10;
//          s = map(x, 0, 1023, -m, m);
//          d = map(y, 0, 1023, -m, m); 
//
//          if(s > p || s < -p){
//              esc.motorDrive(0, s);
//          }
//           if(d > p || d < -p){
//              esc.motorDrive(1, d);
//          }
           
//         Spew it
        if(button_states)
               printf("Got buttons \n\r");
          else   printf("Got nunnn ..\n\r");


          
        for(uint8_t i=0; i<num_buttons; i++) {
             
               printf("%d == ", button_states[i]);
               
          
        }

      last = millis();
      
      
      
      }
    } 
  }
}

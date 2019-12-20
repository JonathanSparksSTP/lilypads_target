// LIGHT SABER #2 - VADER
#define MY_ID  1

//CAN STUFF
#include <ASTCanLib.h> 
#define MESSAGE_ID        0       // Message ID
#define MESSAGE_PROTOCOL  1       // CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
#define MESSAGE_LENGTH    8       // Data length: 8 bytes
#define MESSAGE_RTR       0       // rtr bit
void serialPrintData(st_cmd_t *msg);
st_cmd_t Msg;
uint8_t Buffer[8] = {};

// NEOPIX STUFF
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 6
#define NUM_LEDS 132
#define BRIGHTNESS 255
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//OTHER VARS
boolean triggerState = false;
boolean isIdle = true;
int dimVal = 20;
int pause = 1000; //millisecs between two swings
int glowSpeed = 2; // speed ofglow/"swing"



int incomingByte;
boolean initCan = false;
int thresh = 40;
void setup(){

  
//CAN STUFF
  canInit(500000);
  Msg.pt_data = &Buffer[0];    // reference message data to buffer
  // Initialise CAN packet.
  // All of these will be overwritten by a received packet
  Msg.ctrl.ide = MESSAGE_PROTOCOL;  // Set CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
  Msg.id.ext   = MESSAGE_ID;        // Set message ID
  Msg.dlc      = MESSAGE_LENGTH;    // Data length: 8 bytes
  Msg.ctrl.rtr = MESSAGE_RTR;       // Set rtr bit

//NEOPIX STUFF
  strip.begin();
  strip.fill(strip.Color(dimVal,0,0),0,strip.numPixels()); 
//  fadeOn(strip.Color(dimVal,0,0), 0); // dim Red
  strip.show();
  
  
  Serial.begin(9600);
  Serial.println("start");
  
  
}



void loop() {
 //clear buffer
  clearBuffer(&Buffer[0]);

  // Send command to the CAN port controller
  Msg.cmd = CMD_RX_DATA;
  // Wait for the command to be accepted by the controller
  while(can_cmd(&Msg) != CAN_CMD_ACCEPTED){ //commented out for testing animations
        Serial.println("wait1");
    }
//  testTrigger();
  // Wait for command to finish executing
  while(can_get_status(&Msg) == CAN_STATUS_NOT_COMPLETED){ //commented out for testing animations
        if(!triggerState){

            if(!isIdle){
              
              isIdle = true;
            }
          
        }
        if(triggerState){
          
          pulse(glowSpeed);

          triggerState = false;
            isIdle= false;
        }
        
    } // commented out for testing animations while()

  inData(&Msg);//commented out for testing animations
  


}

void inData(st_cmd_t *msg){

  int ID_IN = msg->id.ext;
  
  if(ID_IN == MY_ID){
    triggerState=0;
   int valIn = msg->pt_data[0];
    Serial.println(valIn);
    if(valIn>thresh){
      if(!initCan){
        triggerState=true;
        initCan = true;
       }
      
      }//if thresh
    else{  
     initCan=false;
     triggerState=false;
    }
  }      
}

void pulse(int _speed){
  
  for(int i = dimVal; i < 255; i = i+_speed){
    strip.fill(strip.Color(i,0,0),0,strip.numPixels());
   strip.show(); 
  }
  for(int i = 255; i > (dimVal*4); i = i-_speed){
   strip.fill(strip.Color(i,0,0),0,strip.numPixels());
    strip.show();  
  }

  
  strip.fill(strip.Color(dimVal,0,0),0,strip.numPixels());
}

void fadeOn(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void fadeOff(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor((strip.numPixels()-1)-i, c);
    strip.show();
    delay(wait);
  }
}




void testTrigger(){

  if (Serial.available() > 0) {
  incomingByte = Serial.read();
  
    if (incomingByte == 'x') {
      Serial.println("on");
      triggerState = true;
    } 
    if (incomingByte == 'z') {
      Serial.println("off");
      triggerState = false;   
    }
  } // if serial.avail
}

  // LIGHT SABER #3 - Rey
#define MY_ID  2

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
int waveSpeed = 3; // speed ofglow/"swing"
int waveLength = 30;
int waveFactor= 5;

boolean initCan = false;
int thresh = 40;

int incomingByte;


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
//  fadeOn(strip.Color(0,0,dimVal), 0); //
strip.fill(strip.Color(0,0,dimVal),0,strip.numPixels()); 
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
//          defaultRed();
            if(!isIdle){
              
              isIdle = true;
            }
          
        }
        if(triggerState){
          Serial.println("trig true");
          waveUp(waveSpeed);
          Serial.println("hello?");
          delay(pause);
          waveDown(waveSpeed );
          Serial.println("back");
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
//    Serial.println(valIn);

    if(valIn>thresh){
      Serial.println("high");
      if(!initCan){
        triggerState=true;
        initCan = true;
       }
      
      }//if thresh
    else{  
     triggerState=false;
     initCan = false;
//     Serial.println("low thr");
    }
  }      
}

void waveUp(int _speed){
  
  for(uint16_t i=0; i<(strip.numPixels()+waveLength); i= i+_speed) {
    strip.fill(strip.Color(0,0,dimVal),0,strip.numPixels()); 
      
    for(int j = 0; j<_speed; j++){
      for(int k = 0; k<waveLength; k++){
        if(((i+j)-k)<(strip.numPixels()-1)){
          strip.setPixelColor(((i+j)-k), (0,0,(255-(waveFactor*k))  ));
        }
      }  
    }
    strip.show();
  } 
} //waveUp()


void waveDown(int _speed){
  Serial.println("In waveDown");
 
    for(uint16_t h=0; h<(strip.numPixels()+waveLength); h= h+_speed) {
//  for(uint16_t h= ((strip.numPixels()-1)+waveLength); h>0; h= h-_speed) {
    int i = (strip.numPixels()-h);
    strip.fill(strip.Color(0,0,dimVal),0,strip.numPixels()); 
      
    for(int j = 0; j<_speed; j++){
      for(int k = 0; k<waveLength; k++){
        
        if(((i-j)+k)>0){
          
          strip.setPixelColor(   (   (i-j)  +k), (0,0,     (255-  (waveFactor*k)   )  ));
        }
      }  
    }
    
    strip.show();
//    Serial.print("after show :");
//    Serial.println(h);
    if(h<0){
      break;
    }
  } 
} //waveDown()



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

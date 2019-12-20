// LIGHT SABER #4 - Kylo
#define MY_ID  3

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
int pause = 250; //millisecs between two swings
int waveSpeed = 1; // speed of glow/"slice"
int waveSpeed2 = 5; // delay to control speed ofvglow/"slice"

int waveLength = 30;
int waveFactor= 5;
int animVersion = 1;

int incomingByte;

long cMillis = 0;
long pMillis = 0;
int milLimit = 1000;
boolean flickOn = true;
boolean initCan = false;
int thresh = 70;
int flickerCounter = 0;
int flickerCounterLimit = 10;

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
//  fadeOn(strip.Color(dimVal,0,0), 0); //
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
          
          strip.fill(strip.Color(dimVal,0,0),0,strip.numPixels());
          strip.show(); 
          
            if(!isIdle){
              
              isIdle = true;
            }
          
        }
        if(triggerState){
          
          
          if(isIdle){
            strip.fill(strip.Color(dimVal,0,0),0,strip.numPixels());
            strip.show();
            waveUp(waveSpeed, waveSpeed2);
            delay(pause);
            
            isIdle= false;
          }
          }
          
          flickering(random(50,500));
          strip.show(); 
          flickerCounter++;
          Serial.println(flickerCounter);
          if(flickerCounter> flickerCounterLimit){
            triggerState = false;
            flickerCounter = 0;
//          flickering(random(50,500));
//          strip.show(); 
          
//          triggerState = false;
            
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
      if(!initCan){
        triggerState=true;
        initCan = true;
       }
      
      }//if thresh
    else{  
      initCan = false;
     triggerState=false;
    }
  }      
}



void flickering(int _limit){
  
cMillis = millis();
//Serial.println(cMillis - pMillis);

if(cMillis - pMillis > milLimit){
  flickOn = !flickOn;
  pMillis = cMillis;
  strip.show(); 
  
}

if(flickOn){
  strip.fill(strip.Color(0,0,0),0,strip.numPixels()); 
  milLimit = random(80);
  
    
}else{

    strip.fill(strip.Color(random(dimVal,255),0,0),0,strip.numPixels()); 
    milLimit = random(90);

}
 
}// flickering()


void waveUp(int _speed, int _speed2){
  for(uint16_t i=strip.numPixels()-(strip.numPixels()/2); i<(strip.numPixels()+waveLength); i= i+_speed) {
    if(animVersion == 0){
      strip.fill(strip.Color(dimVal,0,0),0,strip.numPixels()); 
    
      
      for(int j = 0; j<_speed; j++){
        for(int k = 0; k<waveLength; k++){
          if(((i+j)-k)<(strip.numPixels()-1)){
            strip.setPixelColor(((i+j)-k), ((255-(waveFactor*k),0,0)  ));
          
          }
        }  
      }
     }else {
      strip.setPixelColor(i, 255,0,0);
     }
    strip.show();
    delay(_speed2);
  } 
} //waveUp()




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

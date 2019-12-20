#include <ASTCanLib.h>  
#include "HX711.h"

#define MESSAGE_ID        0         // ***Change to number of the Pad***
#define MESSAGE_PROTOCOL  1
#define MESSAGE_LENGTH    1
#define MESSAGE_RTR       0

const int red = 5;
const int green = 6;
const int blue = 7;
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
bool on = false;

uint8_t c[] = {200,200,200};
uint8_t c1[] = {200,200,200};
uint8_t c2[] = {0,0,255};
uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;

long calibration = 0;
long weightthreshold = 5000;
long currentread = 0;
long lastread = 0;

HX711 pad;

// CAN message object
st_cmd_t txMsg;

// Array of test data to send
uint8_t sendData = 0;
// Transmit buffer
uint8_t txBuffer[1] = {};

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  canInit(500000);               // Initialise CAN port. must be before Serial.begin
  Serial.begin(1000000);         // start serial port
  txMsg.pt_data = &txBuffer[0];  // reference message data to transmit buffer
  pad.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  //***PAD CALIBRATION***
  while(pad.is_ready()==0); //wait for sensor
  calibration = pad.read();
  //calibration -= weightthreshold;
  Serial.println(calibration);
  Serial.println("******************************************************************************");

  //LED TEST
//  while(true){
//    LED(255,255,255);
//    }
}

void loop() {
//  Serial.println("in loop");
  if (pad.is_ready()) {
    //Serial.println(pad.read());
    //lastread = currentread;
    //currentread = pad.read();
//    long distread = pad.read() - calibration;
//    Serial.println(abs(distread));
//    if(abs(distread) > weightthreshold){
    float reading = map(pad.read(),calibration,calibration-500000,0,255); //Map the sensor to 0-255 ***5500 or CHANGE BASED ON CALIBRATION***
    Serial.println(pad.read());
    
    if(reading>0){
      on = true;
      if(reading>255){
        reading=255; //Constrain data to 255
      }
      
//     r = (c1[0]*(1-(reading/255)))+(c2[0]*(reading/255));
//     g = (c1[1]*(1-(reading/255)))+(c2[1]*(reading/255));
//     b = (c1[2]*(1-(reading/255)))+(c2[2]*(reading/255));


      r = map(reading,0,255,c1[0],c2[0]);
      g = map(reading,0,255,c1[1],c2[1]);
      b = map(reading,0,255,c1[2],c2[2]);

//      r = boderRGB(r);
//      g = boderRGB(g);
//      b = boderRGB(b);
      
  
      //  load data into tx buffer
      txBuffer[0] = reading;
      // Setup CAN packet.
      txMsg.ctrl.ide = MESSAGE_PROTOCOL;  // Set CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
      txMsg.id.ext   = MESSAGE_ID;        // Set message ID
      txMsg.dlc      = MESSAGE_LENGTH;    // Data lengt: 8 bytes
      txMsg.ctrl.rtr = MESSAGE_RTR;       // Set rtr bit
      //Serial.println("test");
      // Send command to the CAN port controller
      txMsg.cmd = CMD_TX_DATA;       // send message
      // Wait for the command to be accepted by the controller
      while(can_cmd(&txMsg) != CAN_CMD_ACCEPTED);
      // Wait for command to finish executing
      while(can_get_status(&txMsg) == CAN_STATUS_NOT_COMPLETED);
//      Serial.println("data sent");  
    }
    else{
      r = c[0];
      g = c[1];
      b = c[2];
//      analogWrite(red,  c[0]);
//      analogWrite(green,c[1]);
//      analogWrite(blue, c[2]);
      if(on==true){
        Serial.write(0);
        on = false;
        txBuffer[0] = 0;
        // Setup CAN packet.
        txMsg.ctrl.ide = MESSAGE_PROTOCOL;  // Set CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
        txMsg.id.ext   = MESSAGE_ID;        // Set message ID
        txMsg.dlc      = MESSAGE_LENGTH;    // Data length: 8 bytes
        txMsg.ctrl.rtr = MESSAGE_RTR;       // Set rtr bit
        
        // Send command to the CAN port controller
        txMsg.cmd = CMD_TX_DATA;       // send message
        // Wait for the command to be accepted by the controller
        while(can_cmd(&txMsg) != CAN_CMD_ACCEPTED);
        // Wait for command to finish executing
        while(can_get_status(&txMsg) == CAN_STATUS_NOT_COMPLETED);
        Serial.println("data sent");
      }
    }
  Serial.print(r);
  Serial.print(" | ");
  Serial.print(g);
  Serial.print(" | ");
  Serial.println(b); 
  LED(r,g,b);
  }
 // delay(5);
}

//boderRGB(byte r0,byte g0;byte b0){
//  }

void LED(byte r0,byte g0,byte b0){
  analogWrite(red,r0);
  analogWrite(green,g0);
  analogWrite(blue,b0);
  }

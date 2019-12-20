#include <AltSoftSerial.h>
#include <wavTrigger.h>
#include <ASTCanLib.h> 
             
wavTrigger wTrig;             // Our WAV Trigger object
byte buttonState = 0;         // button State

#define MESSAGE_ID        0       // Message ID
#define MESSAGE_PROTOCOL  1       // CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
#define MESSAGE_LENGTH    8       // Data length: 8 bytes
#define MESSAGE_RTR       0       // rtr bit

// Function prototypes
void serialPrintData(st_cmd_t *msg);

// CAN message object
st_cmd_t Msg;

uint8_t padIn[8] = {};
uint8_t padVol[8] = {};
bool padState[8] = {};

// Buffer for CAN data
uint8_t Buffer[8] = {};

void setup() {
  canInit(500000);            // Initialise CAN port. must be before Serial.begin
  Serial.begin(1000000);       // start serial port
  Serial.println("test");
  Msg.pt_data = &Buffer[0];    // reference message data to buffer
  wTrig.start();
  delay(10);
  // Initialise CAN packet.
  // All of these will be overwritten by a received packet
  Msg.ctrl.ide = MESSAGE_PROTOCOL;  // Set CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
  Msg.id.ext   = MESSAGE_ID;        // Set message ID
  Msg.dlc      = MESSAGE_LENGTH;    // Data length: 8 bytes
  Msg.ctrl.rtr = MESSAGE_RTR;       // Set rtr bit


  delay(1000);
  wTrig.stopAllTracks();
  wTrig.trackLoop(9,1);
  wTrig.trackGain(9,-5);
  wTrig.trackPlayPoly(9);

  wTrig.samplerateOffset(0);
  wTrig.masterGain(-10);                  // set the master gain to -10dB
}

void loop() {
  
  // Clear the message buffer
  clearBuffer(&Buffer[0]);
 
  // Send command to the CAN port controller
  Msg.cmd = CMD_RX_DATA;
  // Wait for the command to be accepted by the controller
  while(can_cmd(&Msg) != CAN_CMD_ACCEPTED);
  // Wait for command to finish executing
  while(can_get_status(&Msg) == CAN_STATUS_NOT_COMPLETED);
  Serial.println("test");
  inData(&Msg);
  printArray();
}

void inData(st_cmd_t *msg){
  int x = msg->id.ext;
  padIn[x] = msg->pt_data[0];
  if(padIn[x] >50){
    if(padState[x] == 0){
      padState[x] = 1;
      Serial.print(x);
      Serial.println(" is on");
      wTrig.trackStop(x+1);
      //wTrig.trackGain(x+1, -20);             // Preset Track 2 gain to -40dB
      wTrig.trackPlayPoly(x+1);              // Start Track 2
      //wTrig.trackFade(x+1, 0, 100, 0);       // Fade Track 2 up to 0dB over 50 msecs
    }
    int g = map(padIn[x],0,255,-20,0);
    Serial.println(g);
    wTrig.trackGain(x+1, g);
  }
  else if(padState[x] == 1){
    padState[x] = 0;
    Serial.print(x);
    Serial.println(" is off");
    wTrig.trackFade(x+1, -70, 1000, true);
  }
}

void play(bool state){
  if (state == 1) {
    if(buttonState == 0){
      buttonState = 1;
      wTrig.trackStop(2);                  //Reset track
      wTrig.trackGain(2, -20);             // Preset Track 2 gain to -40dB
      wTrig.trackPlayPoly(2);              // Start Track 2
      wTrig.trackFade(2, 0, 100, 0);       // Fade Track 2 up to 0dB over 50 msecs
    }
  }
  else{
    buttonState = 0;
    wTrig.trackFade(2, -70, 1000, true);
  }
  delay(30);
}

void printArray(){
  for(int i=0;i<8;i++){
    Serial.print(" | ");
    Serial.print(padIn[i]);
  }
  Serial.print(" | ");
  if(padIn[1]>=5 && buttonState==0){
    play(1);
    Serial.print("***************PLAY******************");
  }
  else if(padIn[1]<5 && buttonState==1){
    play(0);
    Serial.print("***************STOP******************");
  }
  Serial.print("\r\n");
}

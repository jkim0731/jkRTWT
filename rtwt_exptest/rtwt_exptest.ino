#include <Math.h>

const float thetaThresholdGo = -17; // -17 for AH0527
const float thetaThresholdNoGo = -34; // -34 for AH0527
const int poletime = 500; // in millisecond. Should be 1000 times of poleRetractionTime on the behavior control dialogue

const int bittm = 10200; // in microsecond. Should be same as the bittm in make_and_upload_state_matrix
const int gaptm = 10200; // in microsecond. Should be same as the gaptm in make_and_upload_state_matrix
const int numbits = 3; // number of bits to express trial type. Should be 1 less than numtypes in make_and_upload_state_matrix

const int pinTrialType = 11;
int pulseTrialType;
const int pinPole = 12;
int inputPole;
int poleUp = 0; // if pole is up, 1.
const int laserAOM = 7;
const int delayinmilli = 4;
int lightdur = 2;
int flag = 0; // 1 for laser ON, 0 for laser OFF
int state = 0; // state of trial type bitcode arrival 
int bitcode[numbits]; // bitcode for the trial type
int trialType = 0; // 1 & 2 are normal, go & nogo, respectively. 3 & 4 are those for protraction, 5 & 6 for retraction

int protraction; // 1 for protraction, 0 for retraction
float thetaThreshold;
float Angle;
float temp;
char buf[5];
const int numArray = 5;
float thetaArray[numArray];
float thetaDiff[numArray-1];
int i;

unsigned long start_t = 0; 
unsigned long end_t = 0; 
unsigned long loop_start_t = 0; 
unsigned long loop_end_t = 0; 
unsigned long loop_elapsed_t = 0;
unsigned long elapsed_t = 0;

void setup() {
  // put your setup code here, to run once:
pinMode(pinTrialType, INPUT);
pinMode(pinPole, INPUT);
pinMode(laserAOM, OUTPUT);

Serial.begin(115200); // Opens serial port 0, sets data rate to 115,200 bps
Serial1.begin(250000); // Opens serial port 1 (19), sets data rate to 250,000 bps
}

void loop() {
  // put your main code here, to run repeatedly:

//  pulseTrialType = digitalRead(pinTrialType);
//  if (pulseTrialType == HIGH) { 
//    state = 1;
//    start_t = micros();
//  }
//  while (state > 0) {
//    pulseTrialType = digitalRead(pinTrialType);
//    if (pulseTrialType == LOW) { 
//      end_t = micros();
//      elapsed_t = (end_t - start_t);
//      Serial.println(elapsed_t);      
//      state = 0;
//      break;
//    }
//  }

  pulseTrialType = digitalRead(pinTrialType);
  if (pulseTrialType == HIGH) { 
    state = 1;
    delayMicroseconds(bittm);
  }
  
  if (state == 1) {
    for (i = 0; i < numbits; i++) {
      delayMicroseconds(gaptm);
      pulseTrialType = digitalRead(pinTrialType);
      if (pulseTrialType == HIGH) {
        bitcode[i] = 1;
//        Serial.println('1');
      } else {
        bitcode[i] = 0;
//        Serial.println('0');
      }
      delayMicroseconds(bittm);   
    }

    for (i = 0; i < numbits; i++) {
      trialType += bitcode[i] * (pow(2, i)+0.1);
    }
//    trialType = int(trialTypePre);
  Serial.println(trialType);  
  } // end of while (state == 1)
  
  state = 0;
  trialType = 0;
    
}

#include <Math.h>

const float thetaThresholdGo = -5; // -17 for AH0527
const float thetaThresholdNoGo = -34; // -34 for AH0527
const int poletime = 200; // in millisecond. Should be 1000 times of poleRetractionTime on the behavior control dialogue

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
//        Serial.println("1");
      } else {
        bitcode[i] = 0;
//        Serial.println("0");
      }
      delayMicroseconds(bittm);   
    }
  
    for (i = 0; i < numbits; i = i + 1) {
        trialType += bitcode[i] * (pow(2, i) + 0.1);              
    }
    Serial.println(trialType);
//    if ((trialType == 3) || (trialType == 4) || (trialType == 5) || (trialType == 6)) { // real time stimulation trials
      if (trialType <5) { // protraction trials
        protraction = 1;
      } else { // retraction trials
        protraction = 0;
      }
  
      if ((trialType % 2) == 1) { // go trials
        thetaThreshold = thetaThresholdGo;
      } else { // no-go trials
        thetaThreshold = thetaThresholdNoGo;
      }
  
      while(poleUp == 0) {
        inputPole = digitalRead(pinPole);
        if (inputPole == HIGH) {
          poleUp = 1;
          Serial.println("Pole Up");
          delay(poletime);
        }
      }

      while(poleUp == 1) {
        inputPole = digitalRead(pinPole);
        if (inputPole == LOW) {
          poleUp = 0;
          Serial.println("Pole Down");
          state = 0;
        }
        if (Serial1.available()>4) {            
       // If serial buffer has 5 or more bytes then read 5 bytes
          Serial1.readBytes(buf, 5);
          if(buf[0]>0) {
          // If whisker found, compute whisker angle
          // Convert receive data to fixed point signed number (16 bit word length and 8 bit integer length)
            Angle = (float)buf[1] + (float)((unsigned char)buf[2])/255;
            if (Angle > 90) {
              temp = Angle - 256;
              Angle = temp;
            }
            Serial.println(Angle);
            
          
            for ( i = 0; i < numArray-1; i++){
              thetaArray[i] = thetaArray[i+1];  
            }
            thetaArray[numArray-1] = Angle;
            for (i = 0; i < numArray-2; i++) {
              thetaDiff[i] = thetaArray[i+1] - thetaArray[i];
            }
            if (protraction == 1) {
              if ((thetaDiff[0] <= 0) && (thetaDiff[1] <= 0) && (thetaDiff[2] <= 0) && (thetaDiff[3] <= 0) && (thetaArray[numArray-2] > thetaThreshold) && (thetaArray[numArray-1] <= thetaThreshold)) {
                delay(delayinmilli);
                digitalWrite(laserAOM, HIGH);
                Serial.println("laser");
                delay(lightdur);
                digitalWrite(laserAOM, LOW);
              }  
            } else {
              if ((thetaDiff[0] >= 0) && (thetaDiff[1] >= 0) && (thetaDiff[2] >= 0) && (thetaDiff[3] >= 0) && (thetaArray[numArray-2] < thetaThreshold) && (thetaArray[numArray-1] >= thetaThreshold)) {
                delay(delayinmilli);
                digitalWrite(laserAOM, HIGH);
                Serial.println("laser");
                delay(lightdur);
                digitalWrite(laserAOM, LOW);
              }
            }
          }
        } // end of if (Serial1.available()>4)
      } // end of while(poleUp == 1)
//    } // end of if ((trialType == 3) || (trialType == 4) || (trialType == 5) || (trialType == 6))
  } // end of if (state == 1)
  state = 0;
  trialType = 0;
  poleUp = 0; 
  
}

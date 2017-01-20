#include <Math.h>

const float thetaThresholdGo = 0; // -17 for AH0527 // 0 for AH0580
const float thetaThresholdNoGo = -27; // -34 for AH0527 // -27 for AH0580
const int poletime = 200; // in millisecond. Should be 1000 times of poleRetractionTime on the behavior control dialogue

const int bittm = 10200; // in microsecond. Should be same as the bittm in make_and_upload_state_matrix
const int gaptm = 10200; // in microsecond. Should be same as the gaptm in make_and_upload_state_matrix
const int numbits = 3; // number of bits to express trial type. Should be 1 less than numtypes in make_and_upload_state_matrix

int state0 = 0; // state of trial type bitcode arrival 
unsigned long state1t_pre = 0;
int state1 = 0; // state of trial type bitcode arrival (1st bit)
unsigned long state1t_post = 0;
unsigned long state2t_pre = 0;
int state2 = 0; // state of trial type bitcode arrival (2nd bit)
unsigned long state2t_post = 0;
unsigned long state3t_pre = 0;
int state3 = 0; // state of trial type bitcode arrival (3rd bit)
unsigned long state3t_post = 0;
unsigned long state_bc_t_pre = 0;
int state_bitcode = 0; // 1 when all the bitcode for the trial type has arrived
unsigned long state_bc_t_post = 0;
int state_stimTrial = 0; // 1 when the trial is identified as "fake" trials

const int pinTrialType = 11;
int pulseTrialType;
const int pinPole = 12;
int inputPole;
int state_poleUp = 0; // if pole is up, 1.
unsigned long state_sp_t_pre = 0;
int state_stimPeriod = 0; // poletime after pole is up. Stimulation effective when 1. 
unsigned long state_sp_t_post = 0;

const int laserAOM = 7;
const int delayinmicros = 4000; // in microsec
int lightdur = 2000; // in microsec

unsigned long state_pl_t_pre = 0;
int state_prelaser = 0;
unsigned long state_pl_t_post = 0;

unsigned long state_laser_t_pre = 0;
int state_laser = 0;
unsigned long state_laser_t_post = 0;

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
unsigned long rtime; // relative time (micros)
unsigned long reftime; // reference time (micros)
unsigned long temptime;
char strbuffer[30];

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
      if (trialType > 2) {
//        temptime = micros();
//        sprintf(strbuffer, "%lu,%f", temptime, Angle);
//        Serial.println(strbuffer);
          Serial.println(Angle);
      }
      for ( i = 0; i < numArray-1; i++){
        thetaArray[i] = thetaArray[i+1];  
      }
      thetaArray[numArray-1] = Angle;
      for (i = 0; i < numArray-2; i++) {
        thetaDiff[i] = thetaArray[i+1] - thetaArray[i];
      }
    } else {
      if (trialType > 2) {
	Serial.println(“NaN”);
      }
    }	
  }  
  
  if (state0 == 0) {
    pulseTrialType = digitalRead(pinTrialType); 
    if (pulseTrialType == HIGH) { 
      state0 = 1;
      state1t_pre = micros();
    }
  } else if (state1 == 0) {
    state1t_post = micros();
    if ((state1t_post - state1t_pre) > (gaptm + bittm)) {
      pulseTrialType = digitalRead(pinTrialType); 
      state1 = 1;      
      if (pulseTrialType == HIGH) { 
        bitcode[0] = 1;
      } else {
        bitcode[0] = 0;
      }
      state2t_pre = micros();
    }
  } else if (state2 == 0) {
    state2t_post = micros();
    if ((state2t_post - state2t_pre) > (gaptm + bittm)) {
      pulseTrialType = digitalRead(pinTrialType); 
      state2 = 1;      
      if (pulseTrialType == HIGH) { 
        bitcode[1] = 1;
      } else {
        bitcode[1] = 0;
      }
      state3t_pre = micros();
    }
  } else if (state3 == 0) {
    state3t_post = micros();
    if ((state3t_post - state3t_pre) > (gaptm + bittm)) {
      pulseTrialType = digitalRead(pinTrialType); 
      state3 = 1;      
      if (pulseTrialType == HIGH) { 
        bitcode[2] = 1;
      } else {
        bitcode[2] = 0;
      }
      state_bc_t_pre = micros();
    }
  } else if (state_bitcode == 0) { // all bitcode has arrived, but the last bitcode time has not passed yet.
    state_bc_t_post = micros();
    if ((state_bc_t_post - state_bc_t_pre) > bittm) { 
      state_bitcode = 1;      
      for (i = 0; i < numbits; i = i + 1) {
          trialType += bitcode[i] * (pow(2, i) + 0.1);                      
      }
//      temptime = micros();
//      Serial.print(temptime);      
      Serial.print("trial_type=");
      Serial.println(trialType);    
//        sprintf(strbuffer, "%lu,trial_type=%d",temptime,trialType);
//        Serial.println(strbuffer);
      if ((trialType == 3) || (trialType == 4) || (trialType == 5) || (trialType == 6)) { // real time stimulation trials
        state_stimTrial = 1;
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
      }
    }
  }

  if(state_poleUp == 0) {
    inputPole = digitalRead(pinPole);
    if (inputPole == HIGH) {
      state_poleUp = 1;
//      temptime = micros();
//      Serial.print(temptime); 
      Serial.println("pole_up");
//        sprintf(strbuffer, "%lu,pole_up",temptime);
//        Serial.println(strbuffer);
      state_sp_t_pre = millis();
    } 
  } else if (state_stimPeriod == 0) {
      state_sp_t_post = millis();
      if ((state_sp_t_post - state_sp_t_pre) > poletime) {
        state_stimPeriod = 1;
//        temptime = micros();
//        Serial.print(temptime); 
        Serial.println("stim_ready");
//          sprintf(strbuffer, "%lu,stim_ready",temptime);
//          Serial.println(strbuffer);
      }
  } else {
    inputPole = digitalRead(pinPole);
    if (inputPole == LOW) { // end of trial. Need to reset every states.
      state_poleUp = 0; state_stimPeriod = 0; state0 = 0; state1 = 0; state2 = 0; state3 = 0; state_bitcode = 0;
      state_poleUp = 0; state_prelaser = 0; state_laser = 0; state_stimTrial = 0;
//      temptime = micros();
//      Serial.print(temptime); 
      Serial.println("pole_down");
//        sprintf(strbuffer, "%lu,pole_down",temptime);
//        Serial.println(strbuffer);
      trialType = 0;
    }
  }

if (state_stimTrial == 1) {
  if (state_stimPeriod == 1) {
    if (state_prelaser == 0) {
      if (protraction == 1) {
        if ((thetaDiff[0] <= 0) && (thetaDiff[1] <= 0) && (thetaDiff[2] <= 0) && (thetaDiff[3] <= 0) && (thetaArray[numArray-2] > thetaThreshold) && (thetaArray[numArray-1] <= thetaThreshold)) {
          state_prelaser = 1;
          state_pl_t_pre = micros();
        }
      } else { // retraction
        if ((thetaDiff[0] >= 0) && (thetaDiff[1] >= 0) && (thetaDiff[2] >= 0) && (thetaDiff[3] >= 0) && (thetaArray[numArray-2] < thetaThreshold) && (thetaArray[numArray-1] >= thetaThreshold)) {
          state_prelaser = 1;
          state_pl_t_pre = micros();
        }
      }
    } else if (state_laser == 0) { // if state_prelaser == 1
      state_pl_t_post = micros();
      if ((state_pl_t_post - state_pl_t_pre) > delayinmicros) {
        state_laser = 1;
        state_laser_t_pre = micros();
        digitalWrite(laserAOM, HIGH);
//        temptime = micros();
//        Serial.print(temptime); 
        Serial.println("laser_on");
//          sprintf(strbuffer, "%lu,laser_on",temptime);
//          Serial.println(strbuffer);
      }
    } else { // state_laser = 1; 
      state_laser_t_post = micros();
      if ((state_laser_t_post - state_laser_t_pre) > lightdur) { // end of stimulation
        digitalWrite(laserAOM, LOW);
//        temptime = micros();
//        Serial.print(temptime);
        Serial.println("laser_off");
//          sprintf(strbuffer, "%lu,laser_off",temptime);
//          Serial.println(strbuffer);
        state_laser = 0;
        state_prelaser = 0;
      }
    }
  } // end of if(state_stimPeriod == 1)  
} // end of if (state_stimTrial == 1) 
}



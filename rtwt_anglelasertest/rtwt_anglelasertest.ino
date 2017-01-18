int laserAOM = 7;
int delayinmilli = 6;
int lightdur = 2;
int flag = 0; // 1 for laser ON, 0 for laser OFF
float thetaThreshold = -10;
const int protraction = 1;
float Angle;
float temp;
char buf[5];
const int numArray = 5;
float thetaArray[numArray];
float thetaDiff[numArray-1];
int i;

void setup() {
  // put your setup code here, to run once:
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
          delay(lightdur);
          digitalWrite(laserAOM, LOW);
        }  
      } else {
        if ((thetaDiff[0] >= 0) && (thetaDiff[1] >= 0) && (thetaDiff[2] >= 0) && (thetaDiff[3] >= 0) && (thetaArray[numArray-2] < thetaThreshold) && (thetaArray[numArray-1] >= thetaThreshold)) {
          delay(delayinmilli);
          digitalWrite(laserAOM, HIGH);
          delay(lightdur);
          digitalWrite(laserAOM, LOW);
        }
      }
    }
  }
}

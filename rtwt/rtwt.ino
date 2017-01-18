int inputPin = 3;
int festoPin = 5;
int laserAOM = 7;
float Angle;
char buf[5];

int state = 0;
int i;
const int bitnum = 3;
int pulsetime = 7001; // in microseconds 
int pulse;
int bitcode[3]; // bitcode

int trialtype; // 1 & 2 normal right and left. 3 & 4 protraction right and left. 5 & 6 retraction right and left.
const int theta_go = -87; // angle for go position
const int theta_nogo = -125; // angle for no-go position
int theta_temp[3]; // temporary array of theta position, including 2 step history angles
int condition_flag = 0; // flag for condition met

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200); // Opens serial port 0, sets data rate to 115,200 bps
Serial1.begin(250000); // Opens serial port 1 (19), sets data rate to 250,000 bps
pinMode(inputPin, INPUT);
pinMode(festoPin, INPUT);
pinMode(laserAOM, OUTPUT);
}

void loop() 
{
pulse = digitalRead(inputPin);

if (pulse == HIGH)
{
  state = 1;
}
else
{
  state = 0;
}

if (state == 1)
{
  delayMicroseconds(pulsetime);
  for (i = 0; i < bitnum; i = i + 1)
  {
    pulse = digitalRead(inputPin);
    if (pulse == HIGH)
    {
      bitcode[i] = 1;
    }
    else
    {
      bitcode[i] = 0;
    }
    delayMicroseconds(pulsetime);
    //      Serial.print(bitcode[i], BIN);
  }
}  //    Serial.println('\n');


trialtype = 0;                                  // Transfer the 8 bit code to a integer
for (i = 0; i < bitnum; i = i + 1)
{
  trialtype = bitcode[i] * (pow(2, i) + 0.1) + trialtype;
}
//  Serial.println(Msteps);
    
if (Serial1.available()>4) 
 // If serial buffer has 5 or more bytes then read 5 bytes
 {      
 Serial1.readBytes(buf, 5);
 if(buf[0]>0)
  // If whisker found, compute whisker angle
  {
  // Convert receive data to fixed point signed number (16 bit word length and 8 bit integer length)
  Angle = (float)buf[1] + (float)((unsigned char)buf[2])/255;
  Serial.println(Angle);
  }
 }
// Put conditionals here for AOM, angle cutoff at whatever you want it to be
if (Angle > 180 && digitalRead(festoPin) == HIGH) { 
 digitalWrite(laserAOM, HIGH);
 }
}

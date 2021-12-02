// Base code.
#include <ECE3.h>

boolean encoder = true;

uint16_t currSensorValues[8]; // right -> left, 0 -> 7

uint16_t minSensorValues[8] = {971, 809, 855, 718, 740, 786, 809, 947};
uint16_t maxSensorValues[8] = {2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500};


int weighting[8] = { -8, -4, -2, -1, 1, 2, 4, 8};

const int left_nslp_pin=31;
const int left_dir_pin=29;
const int left_pwm_pin=40;

const int right_nslp_pin=11;
const int right_dir_pin=30;
const int right_pwm_pin=39;

boolean start = true;
boolean halfway = false;
boolean ending = false;
int lastError = 0;
int baseSpeed = 30;

float kp = .035;
float kd = .05;

//int baseSpeed = 0;

//float kp = 0;
//float kd = 0;


//float threshold = 1.75;

const int LED_RF = 41;

///////////////////////////////////
void setup() {
// put your setup code here, to run once:

  ECE3_Init();

  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);
  
  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);
  
  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);

  pinMode(LED_RF, OUTPUT);

  delay(2000); //Wait 2 seconds before starting 
  
}

void loop() {

digitalWrite(9, encoder);
encoder = !encoder;


  
if(!ending)
{
  int error=0;
  ECE3_read_IR(currSensorValues);

/*for(int i = 0; i<8; i++)
  {
    Serial.print(currSensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();
*/

//gets us our error value from weighting and normalizing our data to 1000

  for(int i = 0; i<8; i++)
  {
   currSensorValues[i] = abs(currSensorValues[i] - minSensorValues[i]);
   currSensorValues[i] = currSensorValues[i]*1000/(-minSensorValues[i]+maxSensorValues[i]);
   error = error + weighting[i]*currSensorValues[i];

    //Serial.print(currSensorValues[i]);
    //Serial.print('\t');
  }
  //Serial.println();

error = error/4;

//Serial.print("error: ");
//Serial.print(error);
//Serial.print('\t');


//if its the beginnign we set our last error to current error
if(start)
{
  lastError = error;
  start = false;
}

int counter =0;

//checks for the black bar
for(int i = 0; i<8; i++)
{
  if(currSensorValues[i] > 750)
    counter++;
}

//if we are at black bar
if(counter >= 6)
{

//checks whether we already hit the half point and ends program
  if(halfway)
  {
    analogWrite(left_pwm_pin, 0);
    analogWrite(right_pwm_pin, 0);
    digitalWrite(LED_RF, HIGH);
    ending = true;
  }

  //if we havent hit half point then we do a donut
  else
  {
      halfway = true;
      delay(50);
      digitalWrite(right_dir_pin, HIGH);
      analogWrite(left_pwm_pin, 70);
     analogWrite(right_pwm_pin, 70);
  
      resetEncoderCount_left();
      while (getEncoderCount_left() <= 355);
        resetEncoderCount_left();
      digitalWrite(right_dir_pin, LOW);
      delay(200);   
  }
}


//if we are not at black bar, continue forward using kp and kd equation
else
{
  //Serial.write("sup");
float correction = kp* error + kd*(error - lastError);

//Serial.print("correction: ");
//Serial.print(correction);
//Serial.println();



/*if(correction > threshold*baseSpeed)
 correction = threshold* baseSpeed;
  if(correction < -threshold*baseSpeed)
 correction = -threshold* baseSpeed;
*/

analogWrite(left_pwm_pin, baseSpeed - correction);
analogWrite(right_pwm_pin, baseSpeed + correction);

lastError = error;
}
}
    
  }

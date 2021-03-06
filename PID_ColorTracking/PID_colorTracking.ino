#include <PID_v1.h>

#define NUM_DATA 2
byte data[NUM_DATA];
byte cur_data_index;

byte state;


// Clockwise and counter-clockwise definitions.
// Depending on how you wired your motors, you may need to swap.
#define CW  0
#define CCW 1

// Motor definitions to make life easier:
#define MOTOR_A 0
#define MOTOR_B 1

#define STOP 's'
#define FOWARD 'f'
#define BACKWARD 'b'
#define LEFT 'l'
#define RIGHT 'r'
#define TURN_LEFT 'e'
#define TURN_RIGHT 'd'

const byte PWMA = 3;  // PWM control (speed) for motor A
const byte PWMB = 11; // PWM control (speed) for motor B
const byte DIRA = 12; // Direction control for motor A
const byte DIRB = 13; // Direction control for motor B

char incoming = 'n';


// Init Position
int posX = 1500;
int posY = 1300;

//Define Variables we'll be connecting to
//double Setpoint, Input, Output;

double SetpointX, InputX, OutputX;
double SetpointY, InputY, OutputY;


//Specify the links and initial tuning parameters
// face tracking: 0.8, 0.6, 0
// color tracking: 0.4, 0.4, 0
PID myPIDX(&InputX, &OutputX, &SetpointX, 0.4, 0.4, 0, DIRECT);
PID myPIDY(&InputY, &OutputY, &SetpointY, 0.4, 0.4, 0, DIRECT);


//double Kp=2, Ki=5, Kd=1;
//PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
 

 Serial.onReceive(receiveData);
 Serial.onRequest(sendData);

 //Setup PID 
 SetpointX = 100;
 SetpointY = 70;
 myPIDX.SetOutputLimits(-255, 255);
 myPIDY.SetOutputLimits(-255, 255);

 //turn PIDs on
 myPIDX.SetMode(AUTOMATIC);
 myPIDY.SetMode(AUTOMATIC);

 state = 1;
 cur_data_index = 0;

 Serial.begin(9600);
 Serial.println(“Ready!”);
 setupArdumoto();	

}

void loop()
{


 if (Serial.available()){
    
    incoming = Serial.read();
    switch(incoming){
      case 'f':
        driveArdumoto(MOTOR_A, CCW, 100);
        driveArdumoto(MOTOR_B, CCW, 100);
      break;
      case 'b':
        driveArdumoto(MOTOR_A, CW, 100);
        driveArdumoto(MOTOR_B, CW, 100);
      break;
      case 'l':
        analogWrite(PWMA, 50);
        analogWrite(PWMB, 200);
      break;
      case 'r':
        analogWrite(PWMA, 200);
        analogWrite(PWMB, 50);
      break;
      case 'e':
         stopArdumoto(MOTOR_A);
         stopArdumoto(MOTOR_B);
         driveArdumoto(MOTOR_A, CW, 200);
      break;
      case 'd':
         stopArdumoto(MOTOR_A);
         stopArdumoto(MOTOR_B);
         driveArdumoto(MOTOR_B, CW, 200);
      break;
      default:
        stopArdumoto(MOTOR_A);
        stopArdumoto(MOTOR_B);
    }
    	delay(1000);
    if(incoming != 'q'){
      Serial.write(incoming);
    }
    delay(1000);
    
}
  //Input = analogRead(PIN_INPUT);
  //myPID.Compute();
  //analogWrite(PIN_OUTPUT, Output);
}


// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }  
}

// stopArdumoto makes a motor stop
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}

// setupArdumoto initialize all pins
void setupArdumoto()
{
  // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}



// callback for received data
void receiveData(int byteCount){

 state = 0;

 while(Serial.available()) {

  data[cur_data_index++] = Serial.read();

  // When we have received both X and Y coordinates
  if(cur_data_index >= NUM_DATA){
  cur_data_index = 0;

  // Calculate PID outputs with Inputs
  InputX = data[0];
  InputY = data[1];
  myPIDX.Compute();
  myPIDY.Compute();

  // Update Servo Position
  posX = posX + OutputX;
  posY = posY – OutputY;


  state = 1;

  }

}
}

// callback for sending data
void sendData(){
  Serial.write(state);
}




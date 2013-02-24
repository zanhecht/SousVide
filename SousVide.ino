/*
TM1638.h - Library for TM1638.
GND  VCC
DIO	CLK
SB1	SB0
SB3	SB2
SB5	SB4
*/

#include <TM1638.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <stdio.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// initial set point
#define INITIAL_SET_POINT 144.5

// pin to trigger relay
#define TRIGGER_PIN 10

// milliseconds we want as a minimum time on or off
#define MINIMUM_ON_OFF_TIME 500

// Temp sensor resolution in bits
#define SENSOR_RESOLUTION 12

// Temp sensor polling time in milliseconds (allow at least 750 ms for 12-bit temp conversion)
#define TEMP_TIME 1000

//PWM Period in milliseconds
#define WINDOW_SIZE 8000

//Program Parameters
unsigned long tempTime = TEMP_TIME; //how often to check the thermometer

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(8, 9, 7); 

//Define Variables we'll be connecting to
double Setpoint, Input, Output, cSetpoint, cInput;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2000,0.25,0,DIRECT);

unsigned long serialTime; //this will help us know when to talk with processing
unsigned long windowStartTime; //PWM Window start time
char sOutput[9];
int Threshold = ((1024*MINIMUM_ON_OFF_TIME)/WINDOW_SIZE);
word ledBar = 0;

void setup() {
  // start serial port
  Serial.begin(9600); 
  
  //Initialize PWM Timer
  windowStartTime = millis();
  
  // Initialize Variables
  Input = 10;
  Setpoint = INITIAL_SET_POINT;
  Output = 0;
    
  //Start Dallas Library
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, SENSOR_RESOLUTION);
  sensors.setWaitForConversion(false);  // async mode
  sensors.requestTemperatures();  // Send the command to get temperatures
  
  //Initialize PID
  myPID.SetOutputLimits(0, WINDOW_SIZE); //tell the PID to range between 0 and the full window size
  myPID.SetSampleTime(1000); //set sample time in milliseconds 
  myPID.SetMode(AUTOMATIC); //Turn PID on
  
  //turn on display to brightness 1 (0-7)
  module.setupDisplay(true, 0);
  
  //activate relay pin as an output
  digitalWrite(TRIGGER_PIN, LOW);    // sets the relay off
  pinMode(TRIGGER_PIN, OUTPUT);

  Serial.print("Controller Ready\n");
}

void loop() {

  // Poll temp sensor once a second, and send request for next poll  
  if(millis()>tempTime)
  {
    cInput = sensors.getTempCByIndex(0);
    Input = sensors.toFahrenheit(cInput);
    sensors.requestTemperatures(); // Send the command to get temperatures
    tempTime+=TEMP_TIME;
    //Serial.print("Temperature: ");
    //Serial.println(cInput);
    //Serial.print("Output: ");
    //Serial.println(Output);
  }
  
// Turn on PID when we are getting close
/*  if((Setpoint - Input) > 9.0) {
    myPID.SetMode(MANUAL); //Turn PID off
    Output=8000;
  }
  else myPID.SetMode(AUTOMATIC); //Turn PID on
*/
// Call PID Function. Most of the time it will just return without doing anything. At a frequency specified by SetSampleTime it will calculate a new Output. 
  myPID.Compute();
  
  /*
  // display Fahrenheit formatted temperatures
  if (Input<100 && Setpoint<100) sprintf(sOutput," %d %d\n",(int)(Setpoint*10),(int)(Input*10));
    else if (Input<100) sprintf(sOutput,"%d %d\n",(int)(Setpoint*10),(int)(Input*10));
    else if (Setpoint<100) sprintf(sOutput," %d%d\n",(int)(Setpoint*10),(int)(Input*10));
    else sprintf(sOutput,"%d%d\n",(int)(Setpoint*10),(int)(Input*10));
  module.setDisplayToString(sOutput,0b00100010);
  */  
  
  // display Celsius formatted temperatures
  cSetpoint=sensors.toCelsius(Setpoint);
  sprintf(sOutput,"%d*%d*\n",(int)((cSetpoint*10)+0.5),(int)((cInput*10)+0.5));
  module.setDisplayToString(sOutput,0b01000100);
  
  // Calculate output mapping to LEDs
  ledBar = (1 << (char)((Output/1000) +0.5)) - 1;


  //delay to keep wait between button checks
  delay(125);

  //Register button presses
  byte keys = module.getButtons();
  if ((keys == 0b00000010) && (Setpoint <= 210.2)) Setpoint=Setpoint+0.9; //Setpoint Up
  else if ((keys == 0b00000001) && (Setpoint >= 50.0)) Setpoint=Setpoint-0.9; //Setpoint Down
  
  //turn the output pin on/off based on pid output
  unsigned long now = millis();
  if(now - windowStartTime>WINDOW_SIZE) windowStartTime += WINDOW_SIZE; //time to shift the Relay Window
  if(Output > now - windowStartTime)
  {
    digitalWrite(TRIGGER_PIN,HIGH);
    module.setLEDs(ledBar);
    
  }
  else
  {
    digitalWrite(TRIGGER_PIN,LOW);
    module.setLEDs(ledBar << 8);
  }

  //send-receive with processing if it's time
  if(millis()>serialTime)
  {
    SerialReceive();
    SerialSend();
    serialTime+=4000;
  }
  
}

/********************************************
 * Serial Communication functions / helpers
 ********************************************/

union {                // This Data structure lets
  byte asBytes[24];    // us take the byte array
  float asFloat[6];    // sent from processing and
}                      // easily convert it to a
foo;                   // float array



// getting float values from processing into the arduino
// was no small task.  the way this program does it is
// as follows:
//  * a float takes up 4 bytes.  in processing, convert
//    the array of floats we want to send, into an array
//    of bytes.
//  * send the bytes to the arduino
//  * use a data structure known as a union to convert
//    the array of bytes back into an array of floats

//  the bytes coming from the arduino follow the following
//  format:
//  0: 0=Manual, 1=Auto, else = ? error ?
//  1: 0=Direct, 1=Reverse, else = ? error ?
//  2-5: float setpoint
//  6-9: float input
//  10-13: float output  
//  14-17: float P_Param
//  18-21: float I_Param
//  22-245: float D_Param
void SerialReceive()
{

  // read the bytes sent from Processing
  int index=0;
  byte Auto_Man = -1;
  byte Direct_Reverse = -1;
  while(Serial.available()&&index<26)
  {
    if(index==0) Auto_Man = Serial.read();
    else if(index==1) Direct_Reverse = Serial.read();
    else foo.asBytes[index-2] = Serial.read();
    index++;
  } 
  
  // if the information we got was in the correct format, 
  // read it into the system
  if(index==26  && (Auto_Man==0 || Auto_Man==1)&& (Direct_Reverse==0 || Direct_Reverse==1))
  {
    Setpoint=double(foo.asFloat[0]);
    //Input=double(foo.asFloat[1]);       // * the user has the ability to send the 
                                          //   value of "Input"  in most cases (as 
                                          //   in this one) this is not needed.
    if(Auto_Man==0)                       // * only change the output if we are in 
    {                                     //   manual mode.  otherwise we'll get an
      Output=double(foo.asFloat[2]);      //   output blip, then the controller will 
    }                                     //   overwrite.
    
    double p, i, d;                       // * read in and set the controller tunings
    p = double(foo.asFloat[3]);           //
    i = double(foo.asFloat[4]);           //
    d = double(foo.asFloat[5]);           //
    myPID.SetTunings(p, i, d);            //
    
    if(Auto_Man==0) myPID.SetMode(MANUAL);// * set the controller mode
    else myPID.SetMode(AUTOMATIC);             //
    
    if(Direct_Reverse==0) myPID.SetControllerDirection(DIRECT);// * set the controller Direction
    else myPID.SetControllerDirection(REVERSE);          //
  }
  Serial.flush();                         // * clear any random data from the serial buffer
}

// unlike our tiny microprocessor, the processing ap
// has no problem converting strings into floats, so
// we can just send strings.  much easier than getting
// floats from processing to here no?
void SerialSend()
{
  Serial.print("PID ");
  Serial.print(Setpoint);   
  Serial.print(" ");
  Serial.print(Input);   
  Serial.print(" ");
  Serial.print(Output);   
  Serial.print(" ");
  Serial.print(myPID.GetKp());   
  Serial.print(" ");
  Serial.print(myPID.GetKi());   
  Serial.print(" ");
  Serial.print(myPID.GetKd());   
  Serial.print(" ");
  if(myPID.GetMode()==AUTOMATIC) Serial.print("Automatic");
  else Serial.print("Manual");  
  Serial.print(" ");
  if(myPID.GetDirection()==DIRECT) Serial.println("Direct");
  else Serial.println("Reverse");
}

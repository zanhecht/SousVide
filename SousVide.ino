/*
ARDUINO SOUS VIDE AND CROCKPOT CONTROLLER v1.0.0
================================================
Zan Hecht - 11 Dec 2016
http://zansstuff.com/sous-vide */

#define VER_1 1
#define VER_2 0
#define VER_3 0

/*The following Arduino libraries are required to compile:
* TM1638.h: http://code.google.com/p/tm1638-library/
* OneWire.h: http://www.pjrc.com/teensy/td_libs_OneWire.html
* DallasTemperature.h: http://milesburton.com/Dallas_Temperature_Control_Library
* PID_v1.h: http://www.arduino.cc/playground/Code/PIDLibrary

Concept and original code inspired by the article "Turn your crock pot into a
PID controlled sous vide cooker for $25" by andy@chiefmarley.com at
http://chiefmarley.com/Arduino/?p=3

PID settings for coffee urn and "aggressive mode" taken from the Ember source
code from Lower East Kitchen http://lowereastkitchen.com/wp/software/

Temperature input via Dallas DS18B20 One-Wire temperature sensor. Datasheet at
http://www.maxim-ic.com/datasheet/index.mvp/id/2812

UI is via a TM1638-based I/O module with an 8-character 7-seg display, 8 red/
green LEDs, and 8 buttons. This is available from DealExtreme as "8X Digital
Tube + 8X Key + 8X Double Color LED Module" at
http://dealextreme.com/p/module-81873

Written for Arduino 1.0 http://arduino.cc/hu/Main/Software

This program contains code by Brett Beauregard to communicate with the
Processing PID front end using the code available at
http://code.google.com/p/arduino-pid-library/ and
the Processing software available at http://processing.org/download

INSTRUCTIONS
------------

Key assignments on the TM1638 are as follows (LSB to MSB):

TEMP DOWN | TEMP UP | HRS DOWN | HOURS UP | MINS DOWN | MINS UP | MODE | SET

LEDs show the power being applied to the crock-pot via PWM. 8 LEDs = 100%, no
LEDs = 0%. If the LEDs are red, the relay is on. If the LEDs are green, the
relay is off.

Hit "MODE" to switch between modes. Modes are described below:

### ELAPSED TIME ("tinEr   ")

> Timer counts hours, minutes, and seconds. By default, the crock pot is off.
> * SET: Toggle between timer display and "PErcEnt" setting mode described below
> * MINS DOWN: Reset Time
> * HRS DOWN: Reset Time
> * TEMP UP: Turn on crock-pot at power percentage specified in "PErcEnt"
> > setting. Display will read ("On##.##.##").
> * TEMP DOWN: Turn off crock-pot
> 
> #### Settings:
> > ##### COOKING POWER PERCENTAGE ("PErcEnt")  
> > Scale the crockpot cooking power by the specified percentage. Use TEMP UP
> > and TEMP DOWN to adjust the percentage up or down by 5%.

### SOUS VIDE ("SOUSUIdE")
> Sous Vide cooking mode. This mode can only be accessed when a temperature
> sensor is detected at startup.
>   
> Crock-pot or urn should be set on HIGH, filled with water, and have some
> sort of forced circulation (pump or bubbler). Temperature sensor shold be
> in the water away from the sides or bottom.
>    
> Temperatures are diplayed in Celcius. Temperature on the left is the
> setpoint, temperature on the right is the current temperature.
>  
> * SET: Cycle through settings described below
> * TEMP UP: Raise the setpoint by 0.5°
> * TEMP DOWN: Lower the setpoint by 0.5°
> 
> #### Settings:
> > ##### SOUS VIDE TIMER ("S.U. tinEr")  
> > Functions much like the TIMER mode above. Reset by using HRS DOWN or MINS
> > DOWN.
> > 
> > ##### PROPORTIONAL ("Proport. ")  
> > Set the "P" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 100
> > 
> > ##### INTEGRAL ("IntEgrAL")     
> > Set the "I" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 0.05
> > 
> > ##### DERIVATIVE ("dErIvAt. ")      
> > Set the "D" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 1
> > 
> > ##### CALIBRATE TEMPERATURE ("CALibrAt.")
> > Offset the reading from the temperature probe by the amount shown. Use 
> > TEMP UP and TEMP DOWN to increase and decrease the offset by 0.1 degrees
> > celsius. Calibration offset is saved to flash memory when you press SET.

### COOK AND HOLD ("CrockPot")
> Acts like the timer function on more expensive crock-pots. Cooks at power
> power specified in the Percent setting until the time runs out and then
> decreases the power to the equivalent of the crock-pot WARM setting. As
> measured on my crock-pot, WARM is approximately 30% of the crock-pot's HIGH
> setting and 40% of the crock-pot's LOW setting. There is no need to start or
> stop the timer -- it is always running. The first two letters displayed
> indicate whether the Crockpot's knob is set to High ("CL") or Low ("CL").
>   
> **IMPORTANT: YOU MUST SPECIFY WHETHER THE CROCKPOT'S KNOB IS SET TO HIGH**
> **("CH") OR LOW ("CL") OR THE KEEP WARM MODE MAY NOT KEEP YOUR FOOD HOT**
> **ENOUGH TO BE SAFE! THE "CH" AND "CL" SETTINGS DO NOT CHANGE THE**
> **TEMPERATURE OF THE CROCKPOT -- YOU MUST DO THAT YOURSELF WITH THE**
> **CROCKPOT'S KNOB.**
>   
> * SET: Toggle between countdown display and "PErcEnt" setting mode
> * HRS UP, HRS DOWN, MINS UP, MINS DOWN: Set the countdown timer time
> * TEMP UP: Set to "Crockpot on HIGH" ("CH")
> * TEMP DOWN: Set to "Crockpot on LOW" ("CL")
>
> #### Settings:
> > ##### COOKING POWER PERCENTAGE ("PErcEnt")  
> > Scale the crockpot cooking power by the specified percentage. Use TEMP UP
> > and TEMP DOWN to adjust the percentage up or down by 5%.

### DELAYED START ("DELAY St.")
> Turns the crockpot on to power specified in Percent setting after the time
> runs out. There is no need to start or stop the timer -- it is always running.
> Once the time elapses, the Crockpot will stay on until more time is added, the
> controller is unplugged, or the controller is switched to another mode such as
> SOUS VIDE or ELAPSED TIME.
> 
> **WARNING: DO NOT USE THIS MODE WITH PERISHIBLE FOOD IN A THE CROCKPOT!**
> **FOOD IN THE CROCKPOT CAN TAKE TWO OR THREE HOURS TO COME TO A SAFE**
> **TEMPERATURE ONCE THE HEAT IS TURNED ON, MAKING IT EASY FOR THIS MODE TO**
> **CAUSE FOOD TO SIT IN THE "DANGER ZONE" FOR TOO LONG.**
>
> * SET: Toggle between countdown display and "PErcEnt" setting mode
> * HRS UP, HRS DOWN: Adjust the hours until the crockpot starts
> * MINS UP, MINS DOWN: Adjust the minutes until the crockpot starts
>
> #### Settings:
> > ##### COOKING POWER PERCENTAGE ("PErcEnt")  
> > Scale the crockpot cooking power by the specified percentage. Use TEMP UP
> > and TEMP DOWN to adjust the percentage up or down by 5%.

### COOK TO TEMPERATURE ("DELAY St.")
> Turns the crockpot on to power specified in Percent setting until target
> temperature is reached. Once target temperature is reached, turn the crockpot
> to warm mode. 
>
> The first character of the display shows the keep warm setting -- "H"
> indicates that it is calibrated for the CrockPot's knob being set to high, "L"
> indicates that it is calibrated for the knob being on low. This is adjustable
> through the SETTINGS button. The display then shows the target temperature
> in whole degrees followed by the current temperature. The target temperature
> display will change to "--" when the target temperature is reached, and will
> reset back to the default temperature if any temperature adjust button is
> pressed.
>   
> **IMPORTANT: YOU MUST SPECIFY WHETHER THE CROCKPOT'S KNOB IS SET TO HIGH**
> **("CH") OR LOW ("CL") OR THE KEEP WARM MODE MAY NOT KEEP YOUR FOOD HOT**
> **ENOUGH TO BE SAFE! THE "CH" AND "CL" SETTINGS DO NOT CHANGE THE**
> **TEMPERATURE OF THE CROCKPOT -- YOU MUST DO THAT YOURSELF WITH THE**
> **CROCKPOT'S KNOB.**
>
> * SET: Cycle through settings described below
> * TEMP UP: Raise the target temperature by 1°
> * TEMP DOWN: Lower the target temperature by 1°
>
> #### Settings:
> > ##### COOKING POWER PERCENTAGE ("PErcEnt")  
> > Scale the crockpot cooking power by the specified percentage. Use TEMP UP
> > and TEMP DOWN to adjust the percentage up or down by 5%.
> > 
> > ##### CROCKPOT KNOB SETTING ("CP Hi Lo")  
> > Calibrate the keep-warm temperature for the Crockpot's knob being on High
> > or Low. Press TEMP UP for High ("Hi") and TEMP DOWN for Low ("Lo").
*/

/*
TM1638 Pinout
GND VCC
DIO   CLK
SB1   SB0
SB3   SB2
SB5   SB4

By default, DI0-->8, CLK-->9, SB0-->7
*/

#include <TM1638.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <stdio.h>
#include <EEPROM.h>

// Enable serial output to Processing UI (0 to disable, 1 to enable)
#define SERIAL_ENABLE 1
// One-wire data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// define TM1638 module connected to data pin 8, clock pin 9, and strobe n 7
#define T1638_D 8
#define T1638_C 9
#define T1638_S 7
// initial set point
#define INITIAL_SET_POINT 62.5
// pin to trigger relay
#define TRIGGER_PIN 10
// Temp sensor resolution in bits
#define SENSOR_RESOLUTION 11
// Temp sensor polling time in milliseconds
// (allow 750ms for 12-bit temp conversion, 375ms for 11-bit, etc.)
#define TEMP_TIME 400
// Number of temperature readings to average
#define TEMP_NUM_READINGS 5
// PWM Period in seconds
#define WINDOW_SIZE 8
// PID Parameters
// For Rival 6-qt crock pot set on "High": KP=2000, KI=0.25, KD=0
// For coffee urn: KP=1500, KI=1, KD=0
#define INITIAL_KP 1500
#define INITIAL_KI 1
#define INITIAL_KD 0
// PID parameters for "aggressive mode", which is activated whenever the
// temperature is > AGR_INT from the setpoint
#define AGR_KP 3000
#define AGR_KI 0
#define AGR_KD 0
#define AGR_INT 3
// CrockPot Cooking Settings
#define CP_HIGH_PCT 100 // Percent Power for crockpot on 'High'. Should be 100.
#define CP_LOW_PCT 70 // Percent Power for crockpot on 'Low'
#define CP_WARM_PCT 30 // Percent Power for crockpot on 'Warm'
#define CP_ADJ_PCT 100 // Crocopot offset for all levels
// Time to display mode labels, in milliseconds
#define LABEL_TIME 1250
// Debounce time for buttons, in milliseconds
#define DEBOUNCE_TIME 200
//Keep warm percentages
const int cpLowOutput = (float(CP_WARM_PCT)/float(CP_LOW_PCT)) * WINDOW_SIZE * 10;
const int cpHighOutput= (float(CP_WARM_PCT)/float(CP_HIGH_PCT)) * WINDOW_SIZE * 10;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(T1638_D, T1638_C, T1638_S); 

//Define and initialize variables we'll be connecting to
double Setpoint = INITIAL_SET_POINT, Input = -127, Output = 0;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,INITIAL_KP,INITIAL_KI,INITIAL_KD,DIRECT);

unsigned long serialTime; //this will help us know when to talk with processing
unsigned long windowStartTime; //PWM Window start time
unsigned long tempTime; //time to check the thermometer
unsigned long keyTime = 0; //When a button was last pressed
unsigned long labelTime = millis(); //When a label was last displayed
unsigned long tReset = 0; //Variable for resetting timer
double tempReadings[TEMP_NUM_READINGS] = { 0 }; //Array of historical temps
char tempIndex = 0; //Where to store the current temperature in the array
double tempTotal = 0; //Running total of temperature array
long countDn = 0; //Variable for Countdown timer
char dispStr[9]; //String to display on LED
char highLow = 'L'; //For countdown timer, is crockpot on High or Low?
byte ledDots = 0, mode=0, settings=0, onOff=0; 
bool isSensor=false; //Flag for determining if a sensor is connected
int cpPercent=CP_ADJ_PCT; //Crockpot temperature scaling factor for non-Sous Vide
byte tOff, oldTOff; //Temperature Calibration Offset

/**********************************************
*                    Setup                    *
**********************************************/

void setup() {
	// start serial port
	Serial.begin(9600); 
	Serial.println(F("ARDUINO SOUS VIDE AND CROCKPOT CONTROLLER"));
	Serial.print(F("              VERSION "));
	Serial.print(VER_1); Serial.print(F("."));
	Serial.print(VER_2); Serial.print(F("."));
	Serial.print(VER_3); Serial.println(F("              \n"));

	//Read from and initialize (if necessary) EEPROM
	tOff = EEPROM.read(0);
	oldTOff = tOff;
	//Check for signature of current version.
	//Byte 0 is the calibration offset (27 to 227),
	//Byte 1 is the first part of the version number,
	//Byte 2 is the section part of the version number,
	//Byte 42 should be set to 42.
	if((tOff != 255) && (EEPROM.read(1) == VER_1)
			&& (EEPROM.read(2) == VER_2) && (EEPROM.read(42) == 42)) {
		Serial.print(F("EEPROM Signature Check OK: "));
		Serial.print(tOff);
		SerialSpace();
		Serial.print(EEPROM.read(1));
		SerialSpace();
		Serial.print(EEPROM.read(2));
		SerialSpace();
		Serial.println(EEPROM.read(42));
	} else {
		Serial.print(F("EEPROM Signature Check FAILED: "));
		Serial.print(tOff);
		SerialSpace();
		Serial.print(EEPROM.read(1));
		SerialSpace();
		Serial.print(EEPROM.read(2));
		SerialSpace();
		Serial.println(EEPROM.read(42));
		Serial.print(F("Writing to EEPROM... "));
		tOff=127;
		oldTOff=tOff;
		EEPROM.write(0,tOff);
		EEPROM.write(1,VER_1);
		EEPROM.write(2,VER_2);
		EEPROM.write(42,42);
		Serial.println(F("DONE"));
	}



	//Initialize PWM Timer
	windowStartTime = millis();

	Serial.println(F("Initializing Temperature Sensor..."));
	//Start Dallas Library and initialize temp sensor
	sensors.begin();
	isSensor = sensors.getAddress(tempDeviceAddress, 0);
	sensors.setWaitForConversion(false);  // async mode 
	if(isSensor) {
		Serial.print(F("Temperature Sensor Found: ")); Serial.println(tempDeviceAddress[0]);
		sensors.setResolution(tempDeviceAddress, SENSOR_RESOLUTION);
		sensors.requestTemperatures();  // Send the command to get temperatures
	} else {
		Serial.println(F("No Temperature Sensor Found"));  
	}
	tempTime = millis()+TEMP_TIME; // Set the timer to retrieve temps

	//Initialize PID
	myPID.SetOutputLimits(0, WINDOW_SIZE*1000); //set PID output to correct range
	myPID.SetSampleTime(TEMP_TIME); //set sample time in milliseconds

	//turn on display to brightness 0 (0-7)
	module.setupDisplay(true, 0);

	//activate relay pin as an output
	digitalWrite(TRIGGER_PIN, LOW);   // sets the relay off
	pinMode(TRIGGER_PIN, OUTPUT);

	Serial.println(F("Controller Ready\n"));
	delay(5);
}

/**********************************************
*                    Loop                     *
**********************************************/

void loop() {
	// Record current time 
	unsigned long now = millis();

	//Register button presses
	byte keys = 0;
	if(now - keyTime > DEBOUNCE_TIME) {
		keys = module.getButtons();
		if(keys){
			keyTime = now;
			switch (keys) { //Switch settings and modes
			case 0b10000000:
				if ((mode == 1 && settings < 5) 
						|| (mode == 4 && settings < 2) 
						|| (mode != 1 && mode != 4 && settings < 1)) {
					settings++;
				} else {
					settings = 0;
				}
				labelTime = now;
				break;
			case 0b01000000:
				if (mode < 4) {
					mode++;
				} else {
					mode = 0;
				}
				settings = 0;
				labelTime = now;
				break;
			}
		}
	}

	// Calculate output mapping to LEDs
	word outToLed = (1 << (char)((Output/1000) +0.5)) - 1;

	// Poll temp sensor every TEMP_TIME milliseconds and apply calibration offset
	if(now>tempTime && isSensor) Input = getTemps() + (double)(tOff-127)/10;

	//Call PID Function.
	//Most of the time it will just return without doing anything.
	//At a frequency specified by SetSampleTime it will calculate a new Output. 
	if ((abs(Setpoint-Input)>=AGR_INT)) { //Check for "aggressive mode"
		double p, i, d; //Read current tuning
		p = myPID.GetKp();
		i = myPID.GetKi();
		d = myPID.GetKd();
		myPID.SetTunings(AGR_KP, AGR_KI, AGR_KD); //Set aggressive mode tunings
		myPID.Compute(); //Call PID Function.
		myPID.SetTunings(p, i, d); //Restore old tunings
	} else {
		myPID.Compute(); //Call PID Function.
	}

	//turn the output pin on/off based on pid output
	//If one PWM time window has ended, it's time to shift the window
	if((now - windowStartTime)>(WINDOW_SIZE * 1000)) windowStartTime += (WINDOW_SIZE * 1000); 
	if(Output > now - windowStartTime) { //Turn on relay during "on" period
		digitalWrite(TRIGGER_PIN,HIGH); 
	} else {//Turn off relay
		digitalWrite(TRIGGER_PIN,LOW);
		outToLed = outToLed << 8; //Shift LED bar to make LEDs green
	}

	//Determine which mode we are in and perform the appropriate actions
	switch (mode) {
	case 0:
		doTimer(keys); break; //Simple time-elapsed Mode with crockpot off.
	case 1:
		doSousVide(keys); break; // Sous Vide Mode
	case 2:
		doCountdown(keys); break; // Countdown Timer
	case 3:
		doDelayStart(keys); break; // Delayed Start
	case 4:
		doDoneTemp(keys); break; // Cook to Temperature
	}

	//Display label if mode recently switched
	if ((now - labelTime) < LABEL_TIME) {
		doLabel();
		if(SERIAL_ENABLE && (labelTime == now)) {
			Serial.print(F("Mode: ")); Serial.print(mode);
			Serial.print(F("; Settings: ")); Serial.println(settings);
		}
	}

	//write to display
	//Serial.println(dispStr);
	module.setDisplayToString(dispStr,ledDots);
	module.setLEDs(outToLed);

	//send-receive with processing if it's time
	if(millis()>serialTime && SERIAL_ENABLE && mode==1) {
		SerialReceive();
		SerialSend();
		serialTime+=(TEMP_TIME*10);
	}

}

/**********************************************
*           Input/Output Functions            *
**********************************************/

// Query temp sensor, perform a running average, and send request for next poll 
double getTemps() {
	tempTotal -= tempReadings[tempIndex]; //subtract oldest temperature from running total
	tempReadings[tempIndex] = sensors.getTempC(tempDeviceAddress); //read sensor at tempDeviceAddress
	tempTotal += tempReadings[tempIndex]; //add reading to running total
	tempIndex++; //Advance to the next slot in the array for averaging
	if (tempIndex>=TEMP_NUM_READINGS) tempIndex=0; //Wrap around if at end of array

	sensors.setResolution(tempDeviceAddress, SENSOR_RESOLUTION); //Set resolution again for safety
	sensors.requestTemperatures(); //Send the command to request temperatures
	tempTime = millis()+TEMP_TIME; //Increment timer to retrieve temps

	return tempTotal / TEMP_NUM_READINGS; //Average the temperature array to get the running average
}

//Display mode label
void doLabel() {
	switch (mode) {
	case 0:
		switch (settings) {
		case 0:
			sprintf(dispStr,"tinEr   "); ledDots=0b00000000; break; //Elapsed Time
		case 1:
			sprintf(dispStr,"PErcEnt "); ledDots=0b00000000; break; // Set Cooking Percent
		}
		break;
	case 1:
		switch (settings) {
		case 0:
			sprintf(dispStr,"SOUSUIdE"); ledDots=0b00000000; break; //Sous Vide Mode
		case 1:
			sprintf(dispStr,"SU tinEr"); ledDots=0b11000000; break; //Elapsed Sous Vide Time
		case 2:
			sprintf(dispStr,"Proport "); ledDots=0b00000010; break; //Set Proportional Coefficient
		case 3:
			sprintf(dispStr,"IntEgrAL"); ledDots=0b00000000; break; //Set Integral Coefficient
		case 4:
			sprintf(dispStr,"dEriuAt "); ledDots=0b00000010; break; //Set Derivative Coefficient
		case 5:
			sprintf(dispStr,"CALibrAt"); ledDots=0b00000001; break; //Set temperature offset
		}
		break;
	case 2:
		switch (settings) {
		case 0:
			sprintf(dispStr,"CrockPot"); ledDots=0b00000000; break; // Cooking Time
		case 1:
			sprintf(dispStr,"PErcEnt "); ledDots=0b00000000; break; // Set Cooking Percent
		}
		break;
	case 3:
		switch (settings) {
		case 0:
			sprintf(dispStr,"DELAY St"); ledDots=0b00000001; break; // Delayed Start
		case 1:
			sprintf(dispStr,"PErcEnt "); ledDots=0b00000000; break; // Set Cooking Percent
		}
		break;
	case 4:
		switch (settings) {
		case 0:
			sprintf(dispStr,"DonEtEnP"); ledDots=0b00000000; break; // Cook to Temperature
		case 1:
			sprintf(dispStr,"PErcEnt "); ledDots=0b00000000; break; // Set Cooking Percent
		case 2:
			sprintf(dispStr,"CP Hi Lo"); ledDots=0b00000000; break; // Set Crockpot Knob Setting
		}
		break;
	}
}


//Simple time-elapsed Mode with crockpot off.
void doTimer(byte keys) {
	myPID.SetMode(MANUAL); //Turn PID off
	if(settings == 0) {
		unsigned long now = millis();
		unsigned long tTime = (now - tReset)/1000; //Get elapsed time in seconds
		if(keys & 0b10010100) tReset = now; //reset if any time down or the Set button is pushed
		if(keys == 0b00000010) onOff = 1; //Turn on crockpot if Temperature Up button is pushed
		else if(keys == 0b00000001) onOff = 0; //Turn off crockpot if Temperature Down button is pushed
		// Calculate hours, minutes, and seconds
		int tSec = tTime % 60; 
		int tMin = (tTime / 60) % 60;
		int tHr = tTime / 3600;
		if (onOff) { //Toggle output and display based on whether crockpot is on or off
			sprintf(dispStr,"On%02hi%02hi%02hi\n",tHr,tMin,tSec); //Create output string
			Output = WINDOW_SIZE * 10 * cpPercent;
		} else {
			sprintf(dispStr,"  %02hi%02hi%02hi\n",tHr,tMin,tSec); //Create output string
			Output = 0;
		}
		ledDots = 0b00010100; //Set dots to separate hours, minutes, and seconds.
	} else if (settings == 1) {
		setPct(keys);
	}
}

//Sous Vide Mode
void doSousVide(byte keys) {
	//Read in current pid tunings
	double p, i, d;
	p = myPID.GetKp();
	i = myPID.GetKi();
	d = myPID.GetKd();

	switch (settings) {
	case 0: //Temperature Display
		sprintf(dispStr,"---*---*\n");
		ledDots = 0b00000000;
		
		//Write new tOff to EEPROM if necessary
		if (tOff != oldTOff) {
			EEPROM.write(0,tOff);
			oldTOff = tOff;
			Serial.print(F("Updating EEPROM byte 0: ")); Serial.println(tOff);
		}
		break;
	case 1:
		setTimer(keys); break; //Timer
	case 2:
		p=setPro(keys, p); break; //Set Proportional Coefficient
	case 3:
		i=setInt(keys, i); break; //Set Integral Coefficient
	case 4:
		d=setDer(keys, d); break; //Set Derivative Coefficient
	case 5:
		tOff=setCal(keys, tOff); break; //Set temperature offset
	}

	if(settings>=2 && settings<=4) myPID.SetTunings(p, i, d); //Set tunings if in p, i, or d mode

	if(isSensor && Input > 0) { // Is there a sensor and valid temperature data?
		myPID.SetMode(AUTOMATIC); //Turn PID on once valid temperature data exists
		if(settings == 0) setSousVide(keys); //Temperature Display
	}
}


// Countdown Timer
void doCountdown(byte keys) {
	myPID.SetMode(MANUAL); //Turn PID off
	long tempTime = (long)millis();
	long cMs = countDn - tempTime;
	if (cMs < 0) cMs = 0;

	long cHr = cMs / 3600000;
	cMs -= (cHr * 3600000);
	long cMin = cMs / 60000;
	cMs -= (cMin * 60000);
	long cSec = cMs / 1000;
	cMs -= (cSec * 1000);

	if (settings == 0) {
		switch (keys) {
		case 0b00001000:
			cHr = (cHr+1)%24; break; //Hour Up
		case 0b00000100:
			cHr = (cHr-1)%24; break; //Hour Down
		case 0b00100000:
			cMin = (cMin+1); break; //Min Up
		case 0b00010000:
			cMin = (cMin-1); break; //Min Down 
		case 0b1000000:
			highLow = (highLow == 'L') ? 'H' : 'L'; break; // Toggle High/Low settings
		case 0b00000010:
			highLow = 'H'; break;
		case 0b00000001:
			highLow = 'L'; break;
		}
		
		sprintf(dispStr,"C%c%02hi%02hi%02hi\n",highLow,(int)cHr,(int)cMin,(int)cSec);
		ledDots = 0b00010100;
	} else if (settings == 1) {
		setPct(keys);
	}

	cMs = (cHr*3600000)+(cMin*60000)+(cSec*1000)+cMs;
	countDn = cMs + tempTime;

	if (cMs > 500) {
		Output = WINDOW_SIZE * 10 * cpPercent;
	} else {
		if (highLow == 'L') Output = cpLowOutput * cpPercent;
		else Output = cpHighOutput * cpPercent;
	}
}

// Delayed Start
void doDelayStart(byte keys) {
	myPID.SetMode(MANUAL); //Turn PID off
	long tempTime = (long)millis();
	long cMs = countDn - tempTime;
	if (cMs < 0) cMs = 0;

	long cHr = cMs / 3600000;
	cMs -= (cHr * 3600000);
	long cMin = cMs / 60000;
	cMs -= (cMin * 60000);
	long cSec = cMs / 1000;
	cMs -= (cSec * 1000);

	if (settings == 0) {
		switch (keys) {
		case 0b00001000:
			cHr = (cHr+1)%24; break; //Hour Up
		case 0b00000100:
			cHr = (cHr-1)%24; break; //Hour Down
		case 0b00100000:
			cMin = (cMin+1); break; //Min Up
		case 0b00010000:
			cMin = (cMin-1); break; //Min Down 
		}
		
		sprintf(dispStr,"dL%02hi%02hi%02hi\n",(int)cHr,(int)cMin,(int)cSec);
		ledDots = 0b00010100;
	} else if (settings == 1) {
		setPct(keys);
	}

	cMs = (cHr*3600000)+(cMin*60000)+(cSec*1000)+cMs;
	countDn = cMs + tempTime;

	if (cMs > 500) Output = 0;
	else Output = WINDOW_SIZE * 10 * cpPercent;
}


void doDoneTemp(byte keys) {
	myPID.SetMode(MANUAL); //Turn PID off

	if(settings == 0) { //Temperature Display
		if (isSensor && Input > 0) { // Is there a sensor and valid temperature data?
			if (Setpoint) {
				if (keys == 0b00000010 && Setpoint <= 99) Setpoint=((int)Setpoint)+1;
				else if (keys == 0b00000001 && Setpoint >= 10) Setpoint=((int)Setpoint)-1;
			} else if (keys & 0b00000011) { //Setpoint is 0 and Up or Down pressed
				Setpoint = (int)(INITIAL_SET_POINT+0.5);
			}
			
			if (Setpoint) {
				sprintf(dispStr,"%c%02d*%03d*\n",highLow,(int)((Setpoint)+0.5),(int)((Input*10)+0.5));
				ledDots = 0b00000100; //Set decimal points in temperatures
			} else {
				sprintf(dispStr,"%c--*%03d*\n",highLow,(int)((Input*10)+0.5));
				ledDots = 0b00000100; //Set decimal points in temperatures
			}
		} else {
			sprintf(dispStr,"%c--*---*\n",highLow);
			ledDots = 0b00000000;
		}
	} else if (settings == 1) {
		setPct(keys);
	} else if (settings == 2) {
		setHighLow(keys);
	}

	if (Input >= Setpoint) Setpoint = 0;

	if (Setpoint) {
		Output = WINDOW_SIZE * 10 * cpPercent;
	} else {
		if (highLow == 'L') Output = cpLowOutput * cpPercent;
		else Output = cpHighOutput * cpPercent;
	}
}
/********************************************
*               Settings Modes              *
********************************************/

void setSousVide(byte keys) { //Temperature Display
	/*
// display Fahrenheit formatted temperatures
sprintf(dispStr,"%3hi%3hi\n",(int)(sensors.toFahrenheit(Setpoint)*10),(int)(sensors.toFahrenheit(Input)*10));
*/  
	// display Celsius formatted temperatures
	sprintf(dispStr,"%03d*%03d*\n",(int)((Setpoint*10)+0.5),(int)((Input*10)+0.5));
	ledDots = 0b01000100; //Set decimal points in temperatures

	//Process Buttons
	// Setpoint up or down
	if ((keys == 0b00000010) && (Setpoint <= 99)) Setpoint=Setpoint+0.5;
	else if ((keys == 0b00000001) && (Setpoint >= 10.0)) Setpoint=Setpoint-0.5;
}

void setTimer(byte keys) { //Timer
	unsigned long tTime = (millis() - tReset)/1000;
	if(keys & 0b00010100) tReset = millis(); //reset if time dn buttons pushed
	int tSec = tTime % 60;
	int tMin = (tTime / 60) % 60;
	int tHr = tTime / 3600;
	sprintf(dispStr,"t %02hi%02hi%02hi\n",tHr,tMin,tSec);
	ledDots = 0b10010100; //Set dots to separate hours, minutes, and seconds.
}

double setPro(byte keys, double p) { //Set Proportional Coefficient
	sprintf(dispStr,"Pro %4hi\n",(int)p);
	ledDots = 0b00100000;
	//Process Buttons
	if ((keys == 0b00000010) && (p  < 9900)) p+=100; //Increase coeff.
	else if ((keys == 0b00000001) && (p >= 100)) p-=100; //Decrease coeff.
	return p;
}

double setInt(byte keys, double i) { //Set Integral Coefficient
	sprintf(dispStr,"Int %4hi\n",(int)((i*100)+0.5)); 
	ledDots = 0b00100100;
	//Process Buttons
	if ((keys == 0b00000010) && (i  < 99.95)) i+=0.05; //Increase coeff.
	else if ((keys == 0b00000001) && (i >= 0.05)) i-=0.05; //Decrease coeff.
	return i;
}

double setDer(byte keys, double d) { //Set Derivative Coefficient
	sprintf(dispStr,"dEr %4hi\n",(int)d);
	ledDots = 0b00100000;
	//Process Buttons
	if ((keys == 0b00000010) && (d  < 9999)) d++; //Increase coefficient
	else if ((keys == 0b00000001) && (d >= 1)) d--; //Decrease coefficient
	return d;
}

double setCal(byte keys, byte cal) { //Set temp sensor calibration
	sprintf(dispStr,"CAL  %+03hi\n", (int)cal-127);
	ledDots = 0b00100010;
	//Process Buttons
	if ((keys == 0b00000010) && (cal < 226)) cal+=1; //Increase temp offset
	else if ((keys == 0b00000001) && (cal > 28)) cal-=1; //Decrease temp offset
	return cal;
}

void setPct(byte keys) { //Set Cooking Percent
	sprintf(dispStr,"Pct  %3hi\n",cpPercent);
	ledDots = 0b00100000;
	//Process Buttons
	if ((keys == 0b00000010)&&(cpPercent <= 95)) cpPercent+=5; //Increase
	else if ((keys == 0b00000001)&&(cpPercent >= 10)) cpPercent-=5; //Decrease
}

void setHighLow (byte keys) { //Set hold temp for Crockpot Knob on high or low
	ledDots = 0b00000000;
	if (highLow == 'H') strcpy(dispStr,"CP on Hi");
	else if (highLow == 'L') strcpy(dispStr,"CP on Lo");
	
	if (keys == 0b00000010) highLow = 'H';
	else if (keys == 0b00000001) highLow = 'L';
}
/**********************************************
*  Serial Communication functions / helpers   *
**********************************************/

union {               // This Data structure lets
	byte asBytes[24]; // us take the byte array
	float asFloat[6]; // sent from processing and
}                     // easily convert it to a
foo;                  // float array

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
		if(index == 0) Auto_Man = Serial.read();
		else if(index == 1) Direct_Reverse = Serial.read();
		else foo.asBytes[index-2] = Serial.read();
		index++;
	} 

	// if the information we got was in the correct format, 
	// read it into the system
	if(index == 26  && (Auto_Man == 0 || Auto_Man == 1)
	&& (Direct_Reverse == 0 || Direct_Reverse == 1))
	{
		Setpoint=double(foo.asFloat[0]);
		//Input=double(foo.asFloat[1]);  // * the user has the ability to send the 
		                                 // value of "Input". In most cases (as
		                                 // in this one) this is not needed.
		
		if(Auto_Man == 0)                  // * only change the output if we
		{                                  // are in manual mode. Otherwise
			Output=double(foo.asFloat[2]); // we'll get an output blip, then
		}                                  // the controller will overwrite.
		
		double p, i, d;             // * read in and set the controller tunings
		p = double(foo.asFloat[3]); //
		i = double(foo.asFloat[4]); //
		d = double(foo.asFloat[5]); //
		myPID.SetTunings(p, i, d);  //
		
		if(Auto_Man == 0) myPID.SetMode(MANUAL);   // * set the controller mode
		else myPID.SetMode(AUTOMATIC);             //
		
		if(Direct_Reverse == 0) {                  // * set the controller
			myPID.SetControllerDirection(DIRECT);  // direction
		} else {                                   //
			myPID.SetControllerDirection(REVERSE); //
		}
	}
	Serial.flush();           // * clear any random data from the serial buffer
}

void SerialSend()
{
	Serial.print(F("PID "));
	Serial.print(Setpoint);  
	SerialSpace();
	Serial.print(Input);   
	SerialSpace();
	Serial.print(Output);  
	SerialSpace();
	Serial.print(myPID.GetKp());   
	SerialSpace();
	Serial.print(myPID.GetKi());   
	SerialSpace();
	Serial.print(myPID.GetKd());   
	SerialSpace();
	if(myPID.GetMode() == AUTOMATIC) Serial.print(F("Automatic "));
	else Serial.print(F("Manual ")); 
	if(myPID.GetDirection() == DIRECT) Serial.println(F("Direct"));
	else Serial.println(F("Reverse"));
}

inline void SerialSpace()
{
	Serial.print(" ");
}

/*
Copyright
---------

### COPYRIGHT 2012-2016 ZAN HECHT

ARDUINO CROCKPOT SOUS VIDE AND TIMER is licensed under a Creative-
Commons Attribution Share-Alike License.
http://creativecommons.org/licenses/by-sa/3.0/

You are free:
to Share — to copy, distribute and transmit the work
to Remix — to adapt the work
to make commercial use of the work

Under the following conditions:
* Attribution — You must attribute the work in the manner specified
by the author or licensor (but not in any way that suggests that
they endorse you or your use of the work).
* Share Alike — If you alter, transform, or build upon this work,
you may distribute the resulting work only under the same or similar
license to this one.

With the understanding that:
* Waiver — Any of the above conditions can be waived if you get
permission from the copyright holder.
* Public Domain — Where the work or any of its elements is in the
public domain under applicable law, that status is in no way
affected by the license.
* Other Rights — In no way are any of the following rights affected
by the license:
* Your fair dealing or fair use rights, or other applicable
copyright exceptions and limitations;
* The author's moral rights;
* Rights other persons may have either in the work itself or in
how the work is used, such as publicity or privacy rights.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the full
text of the Create-Commons license linked to above for more details.

CHANGELOG
---------

* 0.7 First Public Release
  * 0.7.1 Fixed typo in instructions
  * 0.7.2 Simplified code to output to LED bar
* 0.8 Change to use Celsius internally, fixed temp sensor polling, added moving average for temperature, added "aggressive mode"  
  * 0.8.1 Split code into functions. Skip sous vide mode if temperature sensor is missing. Tweaked aggressive mode.
  * 0.8.2 Formatted instructions with Markdown
  * 0.8.3 Bug Fixes
  * 0.8.4 Changed aggressive mode I parameter to 0 to reduce overshoot.
* 0.9 Add mode labels, delayed start, and calibration
* 1.0 Optimize code. Add cooking percentage and cook until done temp mode.
*/

ARDUINO SOUS VIDE AND CROCKPOT CONTROLLER v1.0.0
================================================
Zan Hecht - 11 Dec 2016
http://zansstuff.com/sous-vide

The following Arduino libraries are required to compile:
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
** 0.7.1 Fixed typo in instructions
** 0.7.2 Simplified code to output to LED bar
* 0.8 Change to use Celsius internally, fixed temp sensor polling, added moving average for temperature, added "aggressive mode"  
** 0.8.1 Split code into functions. Skip sous vide mode if temperature sensor is missing. Tweaked aggressive mode.
** 0.8.2 Formatted instructions with Markdown
** 0.8.3 Bug Fixes
** 0.8.4 Changed aggressive mode I parameter to 0 to reduce overshoot.
* 0.9 Add mode labels, delayed start, and calibration
* 1.0 Optimize code. Add cooking percentage and cook until done temp mode.

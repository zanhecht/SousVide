ARDUINO CROCKPOT SOUS VIDE AND TIMER v0.8.2
===========================================
Zan Hecht - 23 Feb 2013  
http://zansstuff.com/sous-vide

The following Arduino libraries are required to compile:
* TM1638.h: http://code.google.com/p/tm1638-library/
* OneWire.h: http://www.pjrc.com/teensy/td_libs_OneWire.html
* DallasTemperature.h: http://milesburton.com/Dallas_Temperature_Control_Library
* PID_v1.h: http://www.arduino.cc/playground/Code/PIDLibrary

Concept and original code inspired by the article "Turn your crock pot into a
PID controlled sous vide cooker for $25" by andy@chiefmarley.com at
http://chiefmarley.com/Arduino/?p=3

PID settings for coffe urn and "aggressive mode" taken from the Ember source
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

### TIMER ("  ##.##.##")
  
> Timer counts hours, minutes, and seconds. By default, the crock pot is off.
> * SET: Reset Time
> * MINS DOWN: Reset Time
> * HRS DOWN: Reset Time
> * TEMP UP: Turn on crock-pot at 100%. Display will read (On##.##.##)
> * TEMP DOWN: Turn off crock-pot
  
### SOUS VIDE ("62.5°##.#°)
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
> > ##### TIMER ("t ##.##.##")  
> > Functions much like the TIMER mode above. Reset by using HRS DOWN or MINS
> > DOWN.
> > 
> > ##### PROPORTIONAL ("Pro ####")  
> > Set the "P" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 100
> > 
> > ##### INTEGRAL ("Int ####")      
> > Set the "I" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 0.05
> > 
> > ##### DERIVATIVE ("dEr ####")      
> > Set the "D" coefficient in the PID loop. Use TEMP UP and TEMP DOWN to
> > increase and decrease the coefficient by 1
  
### COUNTDOWN TIMER ("CL##.##.##" or "CH##.##.##")
> Acts like the timer function on more expensive crock-pots. Cooks at full
> power until the time runs out and then decreases the power to the equivalent
> of the crock-pot WARM setting. As measured on my crock-pot, WARM is
> approximately 30% of the crock-pot's HIGH setting and 40% of the crock-pot's
> LOW setting. There is no need to start or stop the timer -- it is always
> running.
>    
> **IMPORTANT: YOU MUST SPECIFY WHETHER THE CROCKPOT'S KNOB IS SET TO HIGH ("CH")**
> **OR LOW ("CL") OR THE KEEP WARM MODE MAY NOT KEEP YOUR FOOD HOT ENOUGH TO BE**
> **SAFE! THE "CH" AND "CL" SETTINGS DO NOT CHANGE THE TEMPERATURE OF THE CROCK**
> **POT -- YOU MUST DO THAT YOURSELF WITH THE CROCK-POT'S KNOB.**
>    
> * SET: Toggle between "Crockpot on HIGH" ("CH") and "Crockpot on LOW" ("CL")
> * HRS UP, HRS DOWN, MINS UP, MINS DOWN: Set the countdown timer time.
> * TEMP UP: Set to "Crockpot on HIGH" ("CH")
> * TEMP DOWN: Set to "Crockpot on LOW" ("CL")

Copyright
---------

### COPYRIGHT 2012 ZAN HECHT

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
* 0.8 Change to use Celcius internally, fixed temp sensor polling, added moving average for temperature, added "aggressive mode"  
  * 0.8.1 Split code into functions. Skip sous vide mode if temperature sensor is missing. Tweaked aggressive mode.
  * 0.8.2 Formatted instructions with Markdown

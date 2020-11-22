#include "DHT.h"

/* Program to steer water pump, activate for certain number of seconds
 * MODE = 1 [AUTO] -   water sensor value activates pump
 * MODE = 2 [MANUAL] - button pressed activates pump
 * MODE = 3 [RPi] -    Raspberry PI activates pump
 */

/* Configuration parameters */
#define TIMER_PUMP                20   // Pump activation time [10 - 1 second]
#define PUMP_INHIBIT_TIME         100  // Pump inhibit time
#define WATER_SENSOR_ACTIVATION   400  // Water sensor activation lvl [-]

/* PINs for sensors and actuators */
#define LED_ARDUINO           13  // help
#define WATER_SENSOR_DIGITAL   6  // schematic 5
#define LED1_GREEN             9  // schematic 7
#define LED1_BLUE              7  // schematic 8
#define LED1_RED              11  // schematic 9
#define HUMID_TEMP             4  // schematic 6
#define PUMP_RELAY             2  // schematic 12
#define BUTTON_PUMP           12  // schematic 11
#define MODE_CHOOSE            0  // schematic 2
#define LIGHT_ANALOG           6  // schematic 0
#define WATER_SENSOR_ANALOG    3  // schematic 1

/* Globals */
int buttonState = 0;
int waterSensorDigital = 0;
int waterSensorAnalog = 0;
float humi = 0;
float tempC = 0;
int lightIntensity = 0;
int modeValue = 0;
int timerPump = 0;
int timerPumpInhibit = 0;
int modeChoose = 0;
DHT dht(HUMID_TEMP, DHT11);

void setup() 
{
  pinMode(LED_ARDUINO, OUTPUT);
  pinMode(WATER_SENSOR_DIGITAL, INPUT);
  pinMode(BUTTON_PUMP, INPUT);
  pinMode(PUMP_RELAY, OUTPUT);
  pinMode(LED1_GREEN, OUTPUT);
  pinMode(LED1_BLUE, OUTPUT);
  pinMode(LED1_RED, OUTPUT);
  pinMode(LIGHT_ANALOG, INPUT);
  dht.begin();
  Serial.begin(9600);
}

void loop() 
{
  buttonState = digitalRead(BUTTON_PUMP);
  waterSensorAnalog = analogRead(WATER_SENSOR_ANALOG);
  waterSensorDigital = digitalRead(WATER_SENSOR_DIGITAL);
  humi  = dht.readHumidity();
  tempC = dht.readTemperature();
  lightIntensity = analogRead(LIGHT_ANALOG);
  modeValue = analogRead(MODE_CHOOSE);
  
  // MODE 1: AUTO
  if (modeValue <= 333)
  {  
    modeChoose = 1;
    digitalWrite(LED1_RED, LOW);
    digitalWrite(LED1_BLUE, LOW);
    digitalWrite(LED1_GREEN, HIGH);
  }
  // MODE 3: RPI
  else if (modeValue >= 766)
  {
    modeChoose = 3;
    digitalWrite(LED1_RED, LOW);
    digitalWrite(LED1_BLUE, HIGH);
    digitalWrite(LED1_GREEN, LOW);
  } 
  else
  // MODE 2: MANUAL
  {
    modeChoose = 2;
    digitalWrite(LED1_RED, HIGH);
    digitalWrite(LED1_BLUE, LOW);
    digitalWrite(LED1_GREEN, LOW);   
  }
        
  // Check if pump not run recently
  if (timerPumpInhibit > 0)
  {
    digitalWrite(PUMP_RELAY, LOW);
    timerPumpInhibit--;
  }
  // If pump is not inhibited, run the program
  else
  {
    // Pump activated
    if (timerPump > 0)
    { 
      digitalWrite(PUMP_RELAY, HIGH);
      timerPump--;
      timerPumpInhibit = (timerPump == 0) ? PUMP_INHIBIT_TIME: 0; 
    }
    else
    { 
      // Pump deactivated
      digitalWrite(PUMP_RELAY, LOW);

      // Behave according to the mode
      switch(modeChoose)
      {
        case 1:
          timerPump = waterSensorAnalog < WATER_SENSOR_ACTIVATION ? TIMER_PUMP : 0;
          break;
        case 2:
          timerPump = buttonState ? TIMER_PUMP : 0; 
          break;
        case 3:
          timerPump = 0; // nothing for now
          break;
        default:
          timerPump = 0; // nothing for now
      }    
    }
  }
  
  // Printing to serial
  Serial.print("WsD: ");
  Serial.print(waterSensorDigital);
  Serial.print("[0-1], WsA: ");
  Serial.print(waterSensorAnalog);
  Serial.print("[-], LI: ");
  Serial.print(lightIntensity);
  Serial.print("[-], Hu: ");
  Serial.print(humi);
  Serial.print("[%], Temp: ");
  Serial.print(tempC);
  Serial.print("[°C], Timer: ");
  Serial.print(timerPump);
  Serial.print("[s], Mode: ");
  Serial.print(modeChoose);
  Serial.print("[-], Inhibt: ");
  Serial.print(timerPumpInhibit);
  Serial.print("[-], ButtonSts: ");
  Serial.print(buttonState);
  Serial.println("[-]");
  
}

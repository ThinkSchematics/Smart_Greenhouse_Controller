/***************************************************************************************************
 * Johnny McCown of Think Schemcatics
 * Start Date: 20 Jan 18
 * 
 * At my last house I had a Greenhouse and it was solar powered, and controlled by a Raspberry Pi.  This time I want to 
 * build a system that I could monitor the tempratures remotely and control stuff as needed.  This greenhouse is using Aquaponics
 * rather than Hydoponics, so I have the help of Fish to worry about.  I want to be able to auomate their feeding if we are out of the local area.
 ****************************************************************************************************/


// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
//I want to save power as much as posible so I am going to run one small heater then add another as needed
#define HeaterOne 8 //This will be used to activate a relay
#define HeaterTwo 9 //This will turn on a second heather if the temp drops tpo low

float ambientTemp = 0; //this will be used to store the ambient temp so this value can be used elsewhere
float tempF = 0;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
  uint8_t insideThermometer[8] = { 0x28, 0xFF, 0x65, 0x63, 0xA3, 0x16, 0x03, 0x25 };
  uint8_t outsideThermometer[8] = { 0x28, 0xFF, 0xF7, 0x21, 0xA3, 0x16, 0x03, 0xF6 };


void setup(void)
{
  // start serial port
  Serial.begin(9600);
//  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();

  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  pinMode(HeaterOne, OUTPUT);  //this will be the first heater to turn on 
  pinMode(HeaterTwo, OUTPUT);  // this will be a second heater to turn on if HeaterOne is not doing the job
}


// function to print a device address when going out on serial port
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
 
  ambientTemp = sensors.getTempC(insideThermometer);  // attempt to get this working so I can send the temp to the Blynk App
  tempF = ((ambientTemp)*1.8+32);
}


// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}


/*
 * Main function, calls the temperatures in a loop.
 */
void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  //print the device information
  printData(insideThermometer);
  printData(outsideThermometer);

  if (tempF >= 43){
    digitalWrite(HeaterOne, HIGH); //Anything above the set temp, will cause a high on this relay will deactivate it, causing the heater to turn off
  } else{
    digitalWrite(HeaterOne, LOW);  //Anything below the set temp, will cause a low will activate the relay turning the heater to turn on
  }

  if (tempF >= 38){
    digitalWrite(HeaterTwo, HIGH); //this will cause my relay to stay off if the current heat is above 38 deg
  } else{
    digitalWrite(HeaterTwo, LOW);  // this will turn on the heater if the temp drops below 38 deg, this is back up heat if the first heater doesn't take care of the heat
  }

  Serial.println("Ambient Temp is: ");
  Serial.print(ambientTemp);
  Serial.println();

  Serial.println("tempF is: ");
  Serial.print(tempF);
  Serial.println();
  delay(10000);
}


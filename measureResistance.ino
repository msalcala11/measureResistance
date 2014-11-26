// Author: Marty Alcala
// Description: This is an Arduino program to measure the value of a resistor in ohms.

// Include bluetooth libraries
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <services.h> 

// Initialize global variables
int vinSupplier= A0; // The 5V input voltage pin
int vinReader = A2; // The pin that reads the exact input voltage
int voutReader = A1; // The pin that reads the voltage accross the resistor being measured
int triggerPin = A3; // The pin that listens to whether the button used to initiate the measurement is pressed
int ledIndicator = A4; // Pin used to drive LED to alert the user a measurement is in progress
int voutRaw= 0; // A variable to hold the ADC measurement of the voltage accross measured resistor
int vinRaw = 0; // A variable to hold the ADC measurement of the input voltage
float Vin= 0; // vinRaw mapped to an actual voltage value
float Vout= 0; // voutRaw mapped to an actual voltage value
float Rknown= 14840;//14840;//991; // the value of the known resistor
float Resistance= 0; // the calculation of resistance
char charRes[10]; // The character array representation of the resistance for ble transmission
int triggerPinVoltage = 0; // the ADC measurement of the switch voltage

boolean inprogress = false; // a boolean to let us know whether a test is currently in progress
int avgLength = 1000; // The number of measurements to be averaged
float average(float a[]); // An averaging function

void setup()
{
    // Set the vinSupplier pin to be an output
    pinMode(vinSupplier, OUTPUT); 
    pinMode(ledIndicator, OUTPUT);
   
    // Begin serial communication 
    Serial.begin(9600);
    
    // Init. and start BLE library.
    ble_begin();
}

void loop()
{
    // Check to see if the push button is pressed to initiate a measurement
    triggerPinVoltage = analogRead(triggerPin);
    
    // Only measure if we have received a ble command to do s0
    if( ble_available() ){
      analogWrite(ledIndicator, 255);
      Serial.println("Ble Available");
      
      // Exhasut the ble data buffer so we have nothing left in there to cause a second test to run without user prompt
      while ( ble_available() )
        ble_read();
      
      // Let's take a whole bunch of measurements and average 
      // them to increase precision
      //float resistanceArray[avgLength];
      float sum = 0;
      int i;
      for(i=0; i<avgLength; i++){
      
        // Set inprogress to true to indicate a test has begun due to
        // a button press
        inprogress = true;
        
        // Drive the input to the ohm meter with a 5V input
        analogWrite(vinSupplier, 255);
        
        // Read the voltage across the measured resistor
        voutRaw= analogRead(voutReader);
        
        // Read the voltage being supplied by vinSupplier
        vinRaw = analogRead(vinReader);
        
         // Turn off the input voltage
        analogWrite(vinSupplier, 0);
        
        // Map the ADC measured voltages to actual voltage values
        Vin = (vinRaw/1024.0)*5.0;
        Vout = (voutRaw/1024.0)*5.0;
        
        // Compute the current flowing through the known resistor
        float current = (Vin - Vout)/Rknown;
        
        // Determine the resistance of the resistor we are measuring
        Resistance = Vout/current;
        sum = sum + Resistance;
        Serial.println(Resistance);
        //Serial.println(',');
        
         // Turn off the input voltage
        //analogWrite(vinSupplier, 0);
        
        //Add the resistance value to our averaging array
        //resistanceArray[i] = Resistance;
        //delay(1000);
        
       
        
      }// End for loop
      
      //float finalResistance = average(resistanceArray);
      float finalResistance2 = sum/avgLength;
      Serial.println("Final Resistance");
     // Serial.println(finalResistance);
      Serial.println(finalResistance2);
      
      // Convert float resistance to char array so that we 
      // can transmit over ble
      dtostrf(finalResistance2, 4, 3, charRes);
      
      for(int i=0;i<sizeof(charRes);i++)
      {
        // Write to ble and to console
        ble_write(charRes[i]);
        Serial.print(charRes[i]);
      }
      
      ble_write('\n'); //the new line character let's the phone know the message is over
      analogWrite(ledIndicator, 0);
      
    }
    else if(triggerPinVoltage > 10 && inprogress){
      // Do not do anything if the button has remained depressed since
      // the latest resistance measurement
      //Serial.println("Idling");
      
    }
    else{
      // This means the button has be unpressed so we can finally
      // say this resistance measurement is over and can start a new one
      // once the button is pressed again.
      inprogress = false;
    }
    
    // Communicate changes
    ble_do_events();
    //delay(1000);
    
    
}

float average(float a[]){ 
     int i;
     float avg, sum=0.0;
     for(i=0;i<avgLength;++i){
       sum+=a[i];
     }
     avg =(sum/avgLength);
     return avg;
}

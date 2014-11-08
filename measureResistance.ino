// Author: Marty Alcala
// Description: This is an Arduino program to measure the value of a resistor in ohms.

int vinSupplier= A0; // The 5V input voltage pin
int vinReader = A2; // The pin that reads the exact input voltage
int voutReader = A1; // The pin that reads the voltage accross the resistor being measured
int triggerPin = A3; // The pin that listens to whether the button used to initiate the measurement is pressed
int voutRaw= 0; // A variable to hold the ADC measurement of the voltage accross measured resistor
int vinRaw = 0; // A variable to hold the ADC measurement of the input voltage
float Vin= 0; // vinRaw mapped to an actual voltage value
float Vout= 0; // voutRaw mapped to an actual voltage value
float Rknown= 991; // the value of the known resistor
float Resistance= 0; // the calculation of resistance
int triggerPinVoltage = 0; // the ADC measurement of the switch voltage

boolean inprogress = false; // a boolean to let us know whether a test is currently in progress
int avgLength = 400; // The number of measurements to be averaged
float average(float a[]); // An averaging function

void setup()
{
    // Set the vinSupplier pin to be an output
    pinMode(vinSupplier, OUTPUT); 
   
    // Begin serial communication 
    Serial.begin(9600);
}

void loop()
{
    // Check to see if the push button is pressed to initiate a measurement
    triggerPinVoltage = analogRead(triggerPin);
    
    // Only measure if the button has been pressed (and has been unpressed at least once since last measurement)
    if(triggerPinVoltage > 10 && !inprogress){
      
      // Let's take a whole bunch of measurements and average 
      // them to increase precision
      float resistanceArray[avgLength];
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
        
        // Map the ADC measured voltages to actual voltage values
        Vin = (vinRaw/1024.0)*5.0;
        Vout = (voutRaw/1024.0)*5.0;
        
        // Compute the current flowing through the known resistor
        float current = (Vin - Vout)/Rknown;
        
        // Determine the resistance of the resistor we are measuring
        Resistance = Vout/current;
        
        //Add the resistance value to our averaging array
        resistanceArray[i] = Resistance;
        
        // Turn off the input voltage
        analogWrite(vinSupplier, 0);
        
      }// End for loop
      
      float finalResistance = average(resistanceArray);
      Serial.println("Final Resistance");
      Serial.println(finalResistance);
      
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
    delay(1000);
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

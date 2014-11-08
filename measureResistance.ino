int vinSupplier= A0;
int vinReader = A2;
int voutReader = A1;
int triggerPin = A3;
int raw= 0;
float Vin= 3.3;
float Vout= 0;
float Rknown= 991;
float Resistance= 0;
float buffer= 0;
int triggerPinVoltage = 0;
int vinRaw = 0;
boolean inprogress = false;
int avgLength = 400;
float average(float a[]);


void setup()
{
    pinMode(vinSupplier, OUTPUT);  
    Serial.begin(9600);
}

void loop()
{
    
    triggerPinVoltage = analogRead(triggerPin);
    
    // Only measure if the button has been pressed
    if(triggerPinVoltage > 10 && !inprogress){
      
      // Let's take a whole bunch of measurements and average 
      // them to increase precision
      float resistanceArray[avgLength];
      int i;
      for(i=0; i<avgLength; i++){
      
      // Set inprogress to true to indicate a test has begun due to
      // a button press
      inprogress = true;
      
      // Drive the input to the ohm meter with a 3.3V input
      analogWrite(vinSupplier, 255);
      
      // Read the voltage across the measured resistor
      raw= analogRead(voutReader);
      
      // Read the voltage being supplied by vinSupplier
      vinRaw = analogRead(vinReader);
      //Serial.println(raw);
      //Serial.println(vinRaw);
      
      Vin = (vinRaw/1024.0)*5.0;
      Vout = (raw/1024.0)*5.0;
      //Serial.println(Vin);
      //Serial.println(Vout);
      
      float current = (Vin - Vout)/Rknown;
      //Serial.println(current, 6);
      Resistance = Vout/current;
      //Resistance = (Vout*Rknown)/(Vin - Vout);
      //Serial.println(Resistance);
      resistanceArray[i] = Resistance;
      
      
      // Turn off the input voltage
      analogWrite(vinSupplier, 0);
      //Serial.println("Test complete");
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
      //Serial.println("off");
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

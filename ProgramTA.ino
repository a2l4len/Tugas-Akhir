#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Filters.h>
#include <SPI.h>
#include <SD.h> 

LiquidCrystal_I2C screen(0x27,16,2);

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; 
}

unsigned long waktu;
float voltage1;
float voltage2;
float voltage3;
float voltage4;
float voltage5;
float voltage6;
float voltage7;
float voltage8;
float voltage9;
float voltage10;
float voltage11;
float voltage12;
float voltage13;
float signal1;
float signal2;
bool displayed = false;
int distance = 0;


//batas untuk quality control
float voltage1minimum = 0;
float voltage1maximum = 0;
float voltage2minimum = 0;
float voltage2maximum = 0;
float voltage3minimum = 0;
float voltage3maximum = 0;
float voltage4minimum = 0;
float voltage4maximum = 0;
float voltage5minimum = 0;
float voltage5maximum = 0;
float voltage6minimum = 0;
float voltage6maximum = 0;
float voltage7minimum = 0;
float voltage7maximum = 0;
float voltage8minimum;
float voltage8maximum;
float voltage9minimum;
float voltage9maximum;
float voltage10mimimum;
float voltage10maximum;
float voltage11minimum;
float voltage11maximum;
float voltage12minimum;
float voltage12maximum;
float voltage13minimum;
float voltage13maximum;
//batas untuk quality control


float ACS_Value;                              
float windowLength = 0.1;
RunningStatistics currentStats; 
RunningStatistics sinusWaveStats;

float intercept = 0; 
float slope = 0.0752;
float   Amps_TRMS; 

unsigned long sampleRate = 2000; 
unsigned long sampleInterval = 1000000 / sampleRate; 
unsigned long lastSampleTime = 0;

unsigned long printPeriod = 10; 
unsigned long previousMillis = 0; 

String errormessages = "";


void setup() {
  Serial.begin(115200);
  pinMode (2, INPUT);
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
  pinMode (A4, INPUT);
  pinMode (A5, INPUT);
  pinMode (A6, INPUT);
  pinMode (A7, INPUT);
  pinMode (A8, INPUT);
  pinMode (A9, INPUT);
  pinMode (A10, INPUT);
  pinMode (A11, INPUT);
  pinMode (A12, INPUT);
  pinMode (A13, INPUT);
  pinMode (A14, INPUT);
  pinMode (A15, INPUT);

  currentStats.setWindowSecs(windowLength); 
  sinusWaveStats.setWindowSecs(windowLength);
  
  screen.init();
  screen.backlight();
  screen.clear();
  screen.setCursor(0,0);
  screen.print("Hello, Operator");
  delay(1000);
  screen.clear();

 if (!SD.begin(53)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");

  // Open the file for reading
  File myFile = SD.open("PARAME~1.txt");
  if (myFile) {
    Serial.println("PARAME~1.txt");

    // Read from the file until there's nothing else in it
    while (myFile.available()) {
      String data = myFile.readStringUntil('\n');
//      Serial.println(data);
      
      // Assuming the variable is stored as 'variableName=value'
      int separatorIndex = data.indexOf('=');
      if (separatorIndex != -1) {
        String variableName = data.substring(0, separatorIndex);
        String valueString = data.substring(separatorIndex + 1);
        float value = valueString.toFloat(); // Convert string to float
        
        // Store the value in the corresponding variable
        if (variableName == "voltage1minimum") {
          voltage1minimum = value;
        } else if (variableName == "voltage1maximum") {
          voltage1maximum = value;
        } else if (variableName == "voltage2minimum") {
          voltage2minimum = value;
        } else if (variableName == "voltage2maximum") {
          voltage2maximum = value;
        } else if (variableName == "voltage3minimum") {
          voltage3minimum = value;
        } else if (variableName == "voltage3maximum") {
          voltage3maximum = value;
        } else if (variableName == "voltage4minimum") {
          voltage4minimum = value;
        } else if (variableName == "voltage4maximum") {
          voltage4maximum = value;
        } else if (variableName == "voltage5minimum") {
          voltage5minimum = value;
        } else if (variableName == "voltage5maximum") {
          voltage5maximum = value;
        } else if (variableName == "voltage6minimum") {
          voltage6minimum = value;
        } else if (variableName == "voltage6maximum") {
          voltage6maximum = value;
        } else if (variableName == "voltage7minimum") {
          voltage7minimum = value;
        } else if (variableName == "voltage7maximum") {
          voltage7maximum = value;
        }
      }
    }
    // Close the file
    myFile.close();
    
    // Print the stored variables
//    Serial.print("voltage1minimum: ");
//    Serial.println(voltage1minimum);
//    Serial.print("voltage1maximum: ");
//    Serial.println(voltage1maximum);
//    Serial.print("voltage2minimum: ");
//    Serial.println(voltage2minimum);
//    Serial.print("voltage2maximum: ");
//    Serial.println(voltage2maximum);
//    Serial.print("voltage3minimum: ");
//    Serial.println(voltage3minimum);
//    Serial.print("voltage3maximum: ");
//    Serial.println(voltage3maximum);
//    Serial.print("voltage4minimum: ");
//    Serial.println(voltage4minimum);
//    Serial.print("voltage4maximum: ");
//    Serial.println(voltage4maximum);
//    Serial.print("voltage5minimum: ");
//    Serial.println(voltage5minimum);
//    Serial.print("voltage5maximum: ");
//    Serial.println(voltage5maximum);
//    Serial.print("voltage6minimum: ");
//    Serial.println(voltage6minimum);
//    Serial.print("voltage6maximum: ");
//    Serial.println(voltage6maximum);
//    Serial.print("voltage7minimum: ");
//    Serial.println(voltage7minimum);
//    Serial.print("voltage7maximum: ");
//    Serial.println(voltage7maximum);
  } else {
    // If the file didn't open, print an error
    Serial.println("Error opening PARAME~1.txt");
  }
}



void loop() {
  distance = digitalRead(2);
  waktu = micros();
  Serial.print (waktu);
  Serial.print (" ");
  printvalue();

  signal_reading();

  if (waktu - lastSampleTime >= sampleInterval) {
      lastSampleTime += sampleInterval; 
  
      float currentRawValue = analogRead(A15);
  
      currentStats.input(currentRawValue); 
      sinusWaveStats.input(signal1); 
  
//      Serial.print(currentRawValue);
//      Serial.print(" "); 
//      Serial.print(signal1); 
//      Serial.print(" "); 
  
    if ((unsigned long)(millis() - previousMillis) >= printPeriod) { 
      previousMillis = millis(); 
  
      Amps_TRMS = intercept + slope * currentStats.sigma(); 
  
    }
//      Serial.print(currentStats.sigma());
//      Serial.print(" ");
//      Serial.print(Amps_TRMS);
//      Serial.print(" ");
    }
  
  if (distance == 1){

    MeasureVoltage();

    if (!displayed){
      screen.clear();
      screen.setCursor(0,0);
      screen.print("PROCESSING...");
//      Serial.println ("Processing");
      delay (2000);
      displayed = true;
      screen.clear();
      delay (100);
    }
    
    else {
      if (voltage1 > voltage1minimum && voltage1 < voltage1maximum){
        if (voltage2 > voltage2minimum && voltage2 < voltage2maximum){
          if (voltage3 > voltage3minimum && voltage3 < voltage3maximum){
            if (voltage4 > voltage4minimum && voltage4 < voltage4maximum){
              if (voltage5 > voltage5minimum && voltage5 < voltage5maximum){
                if (voltage6 > voltage6minimum && voltage6 < voltage6maximum){
                  if (voltage7 > voltage7minimum && voltage7 < voltage7maximum){
//                    screen.clear();
//                    delay (100);
                    screen.setCursor(0,0);
                    screen.print("GOOD");
                  }
                }
              } 
            }
          }
        }
      }
      
      if (voltage1 < voltage1minimum || voltage1 > voltage1maximum) {
        errormessages += "TITIK 15 V ";
      }

      if (voltage2 < voltage2minimum || voltage2 > voltage2maximum) {
        errormessages += "TITIK -15 V ";
      }

      if (voltage3 < voltage3minimum || voltage3 > voltage3maximum) {
        errormessages += "TITIK 5 V ";
      }

      if (voltage4 < voltage4minimum || voltage4 > voltage4maximum) {
        errormessages += "TITIK 30 V ";
      }

      if (voltage5 < voltage5minimum || voltage5 > voltage5maximum) {
        errormessages += "TITIK -30 V ";
      }

      if (voltage6 < voltage6minimum || voltage6 > voltage6maximum) {
        errormessages += "TITIK 0.6 V ";
      }

      if (voltage7 < voltage7minimum || voltage7 > voltage7maximum) {
        errormessages += "TITIK 1.2 V ";
     }
      if (errormessages != "") {
        screen.clear();
        screen.setCursor(0,0);
        screen.print("NO GOOD");
        screen.setCursor(0, 1);
        scrollMessage(1, errormessages, 200, 16);
    }
  }
}
  else{
    screen.setCursor(0,0);
    screen.print("STAND BY");
    displayed = false;
//    signal1 = 0;
//    signal2 = 0;
//    Amps_TRMS = 0;
    voltage1 = 0;
    voltage2 = 0;
    voltage3 = 0;
    voltage4 = 0;
    voltage5 = 0;
    voltage6 = 0;
    voltage7 = 0;
//    delay (200);
//    screen.clear();
//    delay (2000);
//    voltage8 = 0;
//    voltage9 = 0;
//    voltage10 = 0;
//    voltage11 = 0;
//    voltage12 = 0;
//    voltage13 = 0;
    errormessages = "";
  }
    
}



void printvalue(){
  Serial.print (signal1);
  Serial.print (" ");
  Serial.print (signal2);
  Serial.print (" ");
  Serial.print (Amps_TRMS);
  Serial.print (" ");
  Serial.print (voltage1);
  Serial.print (" ");
  Serial.print (voltage2);
  Serial.print (" ");
  Serial.print (voltage3);
  Serial.print (" ");
  Serial.print (voltage4);
  Serial.print (" ");
  Serial.print (voltage5);
  Serial.print (" ");
  Serial.print (voltage6);
  Serial.print (" ");  
  Serial.println (voltage7);
//  Serial.print (" ");  
//  Serial.print (voltage8);
//  Serial.print (" ");
//  Serial.print (voltage9);
//  Serial.print (" ");  
//  Serial.print (voltage10);
//  Serial.print (" ");
//  Serial.print (voltage11);
//  Serial.print (" ");
//  Serial.print (voltage12);
//  Serial.print (" ");
//  Serial.println (voltage13);
}



void signal_reading(){
  float data = analogRead(A0);
  float adc_value = data * 4.75 / 1024;

  signal1 = mapFloat(adc_value, 0.0, 4.75, -20, 20);
//  Serial.print (signal1);
//  Serial.print (" ");

  float datasignal = analogRead(A1);
  float adc_signal = datasignal * 4.75 / 1024;

  signal2 = mapFloat(adc_signal, 0.0, 4.75, -20, 20);
//  Serial.print (signal2);
//  Serial.print (" ");
}



void MeasureVoltage(){

  float data1 = analogRead(A2);
  float adc_value1 = data1 * 4.75 / 1024;

  voltage1 = mapFloat(adc_value1, 0.0, 4.75, -20, 25);
//  Serial.print (voltage1);
//  Serial.print (" ");
  //Titik 1
  
  float data2 = analogRead(A3);
  float adc_value2 = data2 * 4.75 / 1024;

  voltage2 = mapFloat(adc_value2, 0.0, 4.75, -20, 25);
//  Serial.print (voltage2);
//  Serial.print (" ");
  //Titik 2
  
  float data3 = analogRead(A4);
  float adc_value3 = data3 * 4.75 / 1024;

  voltage3 = mapFloat(adc_value3, 0.0, 4.75, -20, 25);
  //Serial.println ("V5 = ");
//  Serial.print (voltage3);
//  Serial.print (" ");
  //Titik 3

  float data4 = analogRead(A5);
  float adc_value4 = data4 * 4.75 / 1024;

  voltage4 = mapFloat(adc_value4, 0.0, 4.75, -40, 45.5);
//  Serial.print (voltage4);
//  Serial.print (" ");

  //Titik 4  
  
  float data5 = analogRead(A6);
  float adc_value5 = data5 * 4.75 / 1024;
  voltage5 = mapFloat(adc_value5, 0.0, 4.75, -40, 45.5);
//  Serial.print (voltage5);
//  Serial.print (" ");

  //Titik 5

  float data6 = analogRead(A7);
  float adc_value6 = data6 * 4.75 / 1024;

  voltage6 = mapFloat(adc_value6, 0.0, 4.75, -20, 25);
//  Serial.print (voltage6);
//  Serial.print (" ");

  //Titik 6

  float data7 = analogRead(A8);
  float adc_value7 = data7 * 4.75 / 1024;

  voltage7 = mapFloat(adc_value7, 0.0, 4.75, -20, 25);
//  Serial.println (voltage7);
//  Serial.print (" ");
//  delayMicroseconds(500);
  //Titik 7

//  float data8 = analogRead(A9);
//  float adc_value8 = data8 * 4.75 / 1024;
//
//  voltage8 = mapFloat(adc_value8, 0.0, 4.75, -20, 25);
//  Serial.print (voltage8);
//  Serial.print (" ");
////  delayMicroseconds(500);
//  //Titik 8
//
//  float data9 = analogRead(A10);
//  float adc_value9 = data9 * 4.75 / 1024;
//
//  voltage9 = mapFloat(adc_value9, 0.0, 4.75, -20, 25);
//  Serial.print (voltage9);
//  Serial.print (" ");
////  delayMicroseconds(500);
//  //Titik 9
//
//  float data10 = analogRead(A11);
//  float adc_value10 = data10 * 4.75 / 1024;
//
//  voltage10 = mapFloat(adc_value10, 0.0, 4.75, -20, 25);
//  Serial.print (voltage10);
//  Serial.print (" ");
////  delayMicroseconds(500);
//  //Titik 10
//
//  float data11 = analogRead(A12);
//  float adc_value11 = data11 * 4.75 / 1024;
//
//  voltage11 = mapFloat(adc_value11, 0.0, 4.75, -20, 25);
//  Serial.print (voltage11);
//  Serial.print (" ");
////  delayMicroseconds(500);
//  //Titik 11
//  
//  float data12 = analogRead(A13);
//  float adc_value12 = data12 * 4.75 / 1024;
//
//  voltage12 = mapFloat(adc_value12, 0.0, 4.75, -20, 25);
//  Serial.print (voltage12);
//  Serial.print (" ");
////  delayMicroseconds(500);
//  //Titik 12
//
//  float data13 = analogRead(A14);
//  float adc_value13 = data13 * 4.75 / 1024;
//
//  voltage13 = mapFloat(adc_value13, 0.0, 4.75, -20, 25);
//  Serial.println (voltage13);
//  //Titik 13
//  delayMicroseconds(250); 
}


void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    screen.setCursor(0, row);
    screen.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

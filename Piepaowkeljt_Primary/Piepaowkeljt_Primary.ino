//Gyro sensor code
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>
Adafruit_MPU6050 mpu;
void Gyro_code() {
 /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("A:X");
  Serial.print(a.acceleration.x,3);
  Serial.print("Y");
  Serial.print(a.acceleration.y,3);
  Serial.print("Z");
  Serial.print(a.acceleration.z,3);
  // Serial.println(" m/s^2");
  Serial.println();

  Serial.print("R:X");
  Serial.print(g.gyro.x,3);
  Serial.print("Y");
  Serial.print(g.gyro.y,3);
  Serial.print("Z");
  Serial.print(g.gyro.z,3);
  // Serial.println(" rad/s");
  Serial.println();

  Serial.print("4:");
  Serial.print(temp.temperature);
  // Serial.println(" degC");

  Serial.println("");
}

//Speed sensor code
const byte PulsesPerRevolution = 2;
const unsigned long ZeroTimeout = 100000;
const byte numReadings = 2;

volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;  
unsigned long total; 
unsigned long average;
bool active = 0;

void Speed_code(){
  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000 / PeriodAverage;
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;
  }
  FrequencyReal = FrequencyRaw / 10000;

  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;
  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  // Serial.print("Period: ");
  // Serial.print(PeriodBetweenPulses);
  // Serial.print("\tReadings: ");
  // Serial.print(AmountOfReadings);
  // Serial.print("\tFrequency: ");
  // Serial.print(FrequencyReal);
  // Serial.print("\tRPM: ");
  // Serial.print(RPM);
  // Serial.print("\tTachometer: ");
  // Serial.println(average);

  Serial.print("S-RPM:");
  Serial.println(RPM);
}

//Temperature sensor code:
#include <OneWire.h>
#include "DallasTemperature.h"



// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 5
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void Temp_code(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
		
		// Output the device ID
		Serial.print(i,DEC);

    // Print the data
    float tempC = sensors.getTempC(tempDeviceAddress);
    Serial.print(":");
    Serial.print(tempC);
    Serial.println();
    } 	
  }
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  delay(1000);

  Serial.println("enter start");
  while (Serial.available() == 0) {}     //wait for data available
//Temperature sensor code:
 // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      // printAddress(tempDeviceAddress);
      for (uint8_t i = 0; i < 8; i++) {
        if (tempDeviceAddress[i] < 16) Serial.print("0");
      Serial.print(tempDeviceAddress[i], HEX);
      }

      Serial.println();
		} else {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }


//Gyro sensor code:
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Serial.println("-Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip\n");
    // while (1) {
    //   delay(10);
    // }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("-Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

}

void printArray(float * array, int index){
  Serial.print('[');
  for (int i =0; i< index; i++){
    Serial.print(array[i],3);
    Serial.print(',');
  }
  Serial.println(']');
}

void shiftBack(float * array, int start, int end) {
  for (int i = 0; end - i - 1 >= start; i++){
    array[end - i] = array[end - i - 1];
  }
}

void insertNewValue(float * array, float newValue, int j){
  //assending
  for (int n = 0; n <= j; n++){
    if (n == j){
      array[n] = newValue;
      break;
    }
    if ( newValue <= array[n] ){
      shiftBack(array, n, j);
      array[n] = newValue;
      break;
    }
  }
}

float meanValue(float * ascArr, int size, float errorMargin = 0.002){
  for(int j = 0; j < size/2; j++){
    float total = 0.000;
    for(int i = j; i + j < size; i++){
      total += ascArr[i];
      // Serial.print(ascArr[i],3);
      // Serial.print(',');
    }
    // Serial.println(' ');
    float mean = total/(size - (2 * j));
    // Serial.print("total: ");
    // Serial.println(total);
    // Serial.print("size: ");
    // Serial.println(size - (2.0 * j));
    // Serial.print("mean: ");
    // Serial.println(mean);
    // Serial.println(' ');
    if( -ascArr[j] + mean <= errorMargin && ascArr[size-1-j] - mean <= errorMargin){
      return mean;
    }
  }
  return ascArr[int(size/2)];
}

void loop() {
  if(Serial.available() > 0) {
    String str = Serial.readString();
    if(str == "Adjust Gyro\n" || str == "adjust gyro\n"){
      int size = 100;
      float Acc[3][size];
      float Rot[3][size];
      for (int j = 0; j < size; j++){
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        insertNewValue(Acc[0], a.acceleration.x, j);
        insertNewValue(Acc[1], a.acceleration.y, j);
        insertNewValue(Acc[2], a.acceleration.z, j);
        insertNewValue(Rot[0], g.gyro.x, j);
        insertNewValue(Rot[1], g.gyro.y, j);
        insertNewValue(Rot[2], g.gyro.z, j);
      }
      // printArray(Acc[0], size);
      Serial.print("EA:X");
      Serial.print(meanValue(Acc[0], size),3);
      Serial.print("Y");
      Serial.print(meanValue(Acc[1], size),3);
      Serial.print("Z");
      Serial.print(meanValue(Acc[2], size),3);
      // Serial.println(" m/s^2");
      Serial.println();

      Serial.print("ER:X");
      Serial.print(meanValue(Rot[0], size),3);
      Serial.print("Y");
      Serial.print(meanValue(Rot[1], size),3);
      Serial.print("Z");
      Serial.print(meanValue(Rot[2], size),3);
      // Serial.println(" rad/s");
      Serial.println();
    }
    // Serial.println(str);
    // float testArr[9] = {-0.12, 0.13, 0.24, 0.56, 0.76, 0.81, 0.82 ,2.52, 3.03};
    // printArray(testArr, 9);
    // Serial.println(meanValue(testArr, 9, 1));
  }

    //Gyro sensor code:
      Gyro_code();

    //temperature sensor code:
      Temp_code();
        // delay(1500);
        
    //Speed sensor code:
      Speed_code();
}


void Pulse_Event() {
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();
  if (PulseCounter >= AmountOfReadings)  {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;

    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;
  } else {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
}
//temperature sensor code:
// function to print a device address
// void printAddress(DeviceAddress deviceAddress) {
//   for (uint8_t i = 0; i < 8; i++) {
//     if (deviceAddress[i] < 16) Serial.print("0");
//       Serial.print(deviceAddress[i], HEX);
//   }
// }
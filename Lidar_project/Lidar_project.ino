/**
 * LIDARLite I2C Example
 * Author: Garmin
 * Modified by: Shawn Hymel (SparkFun Electronics)
 * Date: June 29, 2017
 * 
 * Read distance from LIDAR-Lite v3 over I2C
 * 
 * See the Operation Manual for wiring diagrams and more information:
 * http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf
 */
#include "confi.h"
String answer ="";
void setup()
{
  Serial.begin(115200); // Initialize serial connection to display distance readings
  LidarSetup();
  //for Teraranger
  TerrangerSetup(); 

  
}

void loop()
{
  //Lidar
  LidarMeasurement();
  //for Teraanger 
  TerrangerMeasurement();
  delay(20);


}

void LidarSetup(){
  lidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
  lidarLite.configure(0); // Change this number to try out alternate configurations
}

void LidarMeasurement(){
  if ( cal_cnt == 0 ) {
    dist = lidarLite.distance();      // With bias correction
  } else {
    dist = lidarLite.distance(false); // Without bias correction
  }
  maxi = 0;
  for(cal_cnt =0;cal_cnt <25;cal_cnt++)
  {
  if(maxi<dist){
    maxi=dist;
  }    
  }
}

void TerrangerSetup(){
   pinMode(13, OUTPUT);
  Wire.beginTransmission(SENSOR_ADDR);
  Wire.write(short_mode, 2);
  //Wire.write(long_mode, 2); // Sensor is in long mode when powered up
  Wire.endTransmission();
}

void TerrangerMeasurement(){
  Wire.beginTransmission(SENSOR_ADDR);  // Transmit to Evo Mini (THIS IS THE I2C BASE ADDRESS, CHANGE HERE IN CASE IT IS DIFFERENT)
  Wire.write(0x00);                     // Sends measure trigger byte
  Wire.endTransmission();               // Stop transmitting
  
  Wire.requestFrom(SENSOR_ADDR, 3);     // Read back three bytes from Evo Mini (THIS IS THE I2C BASE ADDRESS, CHANGE HERE IN CASE IT IS DIFFERENT)
  buf[0] = Wire.read();                 // First byte of distance
  buf[1] = Wire.read();                 // Second byte of distance
  buf[2] = Wire.read();                 // Byte of checksum
  
  CRC = crc8(buf, 2);                   // Save the "return" checksum in variable "CRC" to compare with the one sent by the TeraRanger
  
  if (CRC == buf[2]) {                  // If the function crc8 return the same checksum than the TeraRanger, then:
    distance = (buf[0]<<8) + buf[1];    // Calculate distance in mm
    cnt++;
    if(maxi2<=distance){
      maxi2 = distance;
    }

  }
  else {
    Serial.println("CRC error!");
  }

  if(cnt>=25){
    answer = "<";
    answer.concat(maxi);
    answer.concat(",");
    answer.concat(maxi2/10);//cm
    answer.concat(">");
    Serial.println(answer);
    cnt = cnt%25;
    maxi2 = 0;
  }
  
}

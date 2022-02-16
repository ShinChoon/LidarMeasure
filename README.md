# LidarMeasure
Arduino+ESP32
This project is for Halfway show of PDP project in Aalto Unversity
It consist of three parts:
1. Lidar project script: running in Arduino Nano to measure the distacne in X and Y direction and upload the value to the Serial bus.
2. ESP32MQTT script: running in the EPS32, connect Wi-Fi and upload the values into MQTT channel.
3. matplot.py: running in the local pc with MQTT server enabled. The script listens to MQTT channel and update the color dot location(x,y) on the chart.


Components: 
Arduino Nano
ESP32 Wrover Module

Garmin Lidar 
Terrabee TOF

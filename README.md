# Temperature and Humidity Control with Serial Adjustment and 7-Segment Display

## Monitoring and Displaying Values:

The current temperature and humidity will be monitored and displayed on a 7-segment display connected to an Arduino Uno

The AM2320 sensor, which communicates via I2C, will be used

The sensor's functionality will be simulated with the help of a second microcontroller that will implement the sensor's functions

## Adjusting Settings via Serial Communication:

The settings for the activation/deactivation thresholds of the fan and humidifier will be adjusted through serial communication

The user can dynamically modify the operating thresholds as needed, without requiring the device to be reprogrammed

## Fan Control:
Based on the data received from the MC that's simulating the sensor, the system will control a fan to regulate the temperature and humidity according to the thresholds set via serial communication

The fan will be controlled using a relay

The fan will be simulated using an LED


## 7-Segment Display:

Special attention will be given to the programming and connection of the 7-segment display to ensure a clear and accurate reading of the temperature and humidity values

Each of the two values (temperature and humidity) will be alternately displayed on the device (consisting of two separate 7-segment displays) at 2-second intervals

## User Feedback:

The system must provide feedback to the user to confirm the receipt and implementation of the new settings via serial communication

This feedback is implemented throught a temporary change in the display
![SystemConections](https://github.com/user-attachments/assets/2327c890-4be2-40f6-98a7-4533a4f5d91f)

# SensorTag2650
C++ application collecting data from TI SensorTag 2650 and pushing records to Thingspeak chanell

**Requirements:**
* Bluetooth dongle with Low energy support
* **hcitool** and **gatttool** for connecting to BLE devices
* cmake
* ThingSpeak account

**Installation:**

`$ git clone https://github.com/durovsky/SensorTag2650.git`

`$ cd SensorTag2650`

`$ mkdir build`

`$ cd build`

`$ cmake ..`

`$ make`

Adopt filepaths in *include/filepaths.h* to your wish. 
Add to your ThingSpeak channel write KEY to include/key.h

**Description:**

This stack contains three executables : sensortag, thingspeak_minute_update, thingspeak_hour_update

* **sensortag** - connects to SensorTag2650 and collects temperature, light and humidity data and saves them to _MINUTE files on disk (location specified by "filepaths.h")
* **thingspeak_minute_update** - is designed to be launched by crontab every minute pushing actual data red from _MINUTE FILES to Thingspeak channel and to _HOUR files for averaging
* **thingpeak_hour_update** - is designed to be launched by crontab every hour - computes average values and pushes results to ThingSpeak channel

**Usage:**
`SensorTag2650/build$ ./sensortag AA:BB:CC:DD:EE:FF`  
where AA:BB:CC:DD:EE:FF is BLE address obtained by sudo hcitool lescan

`SensorTag2650/build$ ./thingspeak_minute_update` 
pushes actual data in _MINUTE files to ThingSpeak channel

`SensorTag2650/build$ ./thingspeak_hour_update` 
pushes average values from _HOUR files to ThingSpeak channel

**Tested configurations**
Ubuntu 14.04.03 LTS, Bluez 5.36, SensorTag2650 fw 1.2, ASUS BT400 dongle
BeagleBone Black Debian 3.8 Bluez 5.36, SensorTag2650 fw 1.2, ASUS BT400 dongle



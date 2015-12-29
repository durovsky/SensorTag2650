/*****************************************************************************************//**
* @file thingspeak_minute_update.cpp
*
* Copyright (c)
* SmartRoboticSystems
* December 2015
*
* This program reads data stored in minute .txt files and pushes them to ThingSpeak actual channel
* fileds and to avg.txt files for an hour averaging. Crontab is used to execute this file every minute
*
* Licensed under the EUPL V.1.1
*
* This Software is provided to You under the terms of the European
* Union Public License (the "EUPL") version 1.1 as published by the
* European Union. Any use of this Software, other than as authorized
* under this License is strictly prohibited (to the extent such use
* is covered by a right of the copyright holder of this Software).
*
* This Software is provided under the License on an "AS IS" basis and
* without warranties of any kind concerning the Software, including
* without limitation merchantability, fitness for a particular purpose,
* absence of defects or errors, accuracy, and non-infringement of
* intellectual property rights other than copyright. This disclaimer
* of warranty is an essential part of the License and a condition for
* the grant of any rights to this Software.
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "socket_client.h"
#include "filepaths.h"
#include "key.h"

int main(int argc, char* argv[])
{
  //============================================
  //Read actual data from _MINUTE files
  //============================================
  std::ifstream temperature_minute;
  std::ifstream humidity_minute;
  std::ifstream light_minute;

  std::string actual_line;

  double temperature = 0;
  double humidity = 0;
  double light = 0;

  // Try to open the _MINUTE file for reading (may be currently being writen by sensortag process)
  do {
    temperature_minute.open(TEMPERATURE_MINUTE_FILEPATH);
  } while((temperature_minute.is_open()) == 0);

  std::getline(temperature_minute,actual_line);
  temperature_minute.close();
  temperature = atof(actual_line.c_str());

  // Try to open the _MINUTE file for reading (may be currently being writen by sensortag process)
  do {
    humidity_minute.open(HUMIDITY_MINUTE_FILEPATH);
  } while((humidity_minute.is_open()) == 0);

  std::getline(humidity_minute,actual_line);
  humidity_minute.close();
  humidity = atof(actual_line.c_str());


  // Try to open the _MINUTE file for reading (may be currently being writen by sensortag process)
  do {
    light_minute.open(LIGHT_MINUTE_FILEPATH);
  } while((light_minute.is_open()) == 0);

  std::getline(light_minute,actual_line);
  light_minute.close();
  light = atof(actual_line.c_str());

  std::cout << "Updating ThingSpeak Data..."
            << "   Temp: " << temperature
            << "   Light: " << light
            << "   Humidity: " << humidity
            << std::endl;

  //=======================================================
  //Push data to ThingSpeak channel
  //=======================================================
  exploringBB::SocketClient sc("thingspeak.com", 80);

  std::ostringstream head, data;

  data << "field1=" << temperature << "&field3=" << humidity << "&field5=" << light << std::endl;
  sc.connectToServer();
  head << "POST /update HTTP/1.1\n"
       << "Host: api.thingspeak.com\n"
       << "Connection: close\n"
       << "X-THINGSPEAKAPIKEY: " << key << "\n"
       << "Content-Type: application/x-www-form-urlencoded\n"
       << "Content-Length: " << std::string(data.str()).length() << "\n\n";
  sc.send(std::string(head.str()));

  sc.send(std::string(data.str()));

  std::string rec = sc.receive(1024);
  //std::cout << "[" << rec << "]" << std::endl;


  //======================================================
  //Write data to _HOUR files for average hour update
  //======================================================

  std::ofstream temperature_hour;
  std::ofstream humidity_hour;
  std::ofstream light_hour;


  //Try to open _HOUR file for writing
  do {
    temperature_hour.open(TEMPERATURE_HOUR_FILEPATH, std::ios_base::app);
  } while((temperature_hour.is_open()) == 0);

  temperature_hour << temperature;
  temperature_hour << std::endl;
  temperature_hour.close();

  //Try to open _HOUR file for writing
  do {
    humidity_hour.open(HUMIDITY_HOUR_FILEPATH, std::ios_base::app);
  } while((humidity_hour.is_open()) == 0);

  humidity_hour << humidity;
  humidity_hour << std::endl;
  humidity_hour.close();

  //Try to open _HOUR file for writing
  do {
    light_hour.open(LIGHT_HOUR_FILEPATH, std::ios_base::app);
  } while((light_hour.is_open()) == 0);

  light_hour << light;
  light_hour << std::endl;
  light_hour.close();

  return(0);
}

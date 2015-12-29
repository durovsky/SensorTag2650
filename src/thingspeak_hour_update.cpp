/*****************************************************************************************//**
* @file thingspeak_hour_update.cpp
*
* Copyright (c)
* SmartRoboticSystems
* December 2015
*
* This program reads data stored in *hour.txt files compute averageof and pushes the result to
* ThingSpeak last week channel fileds. Crontab is used to execute this file every hour
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
#include <vector>
#include <numeric>

#include "socket_client.h"
#include "filepaths.h"
#include "key.h"

int main()
{
  //===============================================================
  //Compute average values
  //===============================================================
  double temperature_avg = 0;
  double humidity_avg = 0;
  double light_avg = 0;

  std::ifstream temperature_hour;
  std::ifstream humidity_hour;
  std::ifstream light_hour;

  std::vector<double> buffer;
  double temp_double;

  //Try to open _HOUR file
  do {
    temperature_hour.open(TEMPERATURE_HOUR_FILEPATH);
  } while((temperature_hour.is_open()) == 0);

  //Compute average temperature
  while(temperature_hour)
  {
    temperature_hour >> temp_double;
    buffer.push_back(temp_double);
  }
  temperature_avg = std::accumulate(buffer.begin(), buffer.end(),0.0f) / buffer.size();
  buffer.clear();

  //Try to open _HOUR file
  do {
    humidity_hour.open(HUMIDITY_HOUR_FILEPATH);
  } while((humidity_hour.is_open()) == 0);

  //Compute average humidity
  while(humidity_hour)
  {
    humidity_hour >> temp_double;
    buffer.push_back(temp_double);
  }
  humidity_avg = std::accumulate(buffer.begin(), buffer.end(),0.0f) / buffer.size();
  buffer.clear();

  //Try to open _HOUR file
  do {
    light_hour.open(LIGHT_HOUR_FILEPATH);
  } while((light_hour.is_open()) == 0);

  //Compute average light
  while(light_hour)
  {
    light_hour >> temp_double;
    buffer.push_back(temp_double);
  }
  light_avg = std::accumulate(buffer.begin(), buffer.end(),0.0f) / buffer.size();
  buffer.clear();

  //======================================================
  //Push data to ThingSpeak
  //======================================================

  //Safety delay (it is possible to push data to thingspeak channel every 15 second and minute update is pushing as well)
  usleep(30000000);

  exploringBB::SocketClient sc("thingspeak.com", 80);
  std::ostringstream head, data;

  data << "field2=" << temperature_avg << "&field4=" << humidity_avg << "&field6=" << light_avg << std::endl;
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

  //=====================================================
  //Clear _HOUR files
  //=====================================================
  std::ofstream temperature_hour_to_clear;
  do {
    temperature_hour_to_clear.open(TEMPERATURE_HOUR_FILEPATH, std::ofstream::out | std::ofstream::trunc);
  } while ((temperature_hour_to_clear.is_open()) == 0);

  temperature_hour_to_clear.close();

  std::ofstream humidity_hour_to_clear;
  do {
    humidity_hour_to_clear.open(HUMIDITY_HOUR_FILEPATH, std::ofstream::out | std::ofstream::trunc);
  } while ((humidity_hour_to_clear.is_open()) == 0);
  humidity_hour_to_clear.close();

  std::ofstream light_hour_to_clear;
  do {
    light_hour_to_clear.open(LIGHT_HOUR_FILEPATH, std::ofstream::out | std::ofstream::trunc);
  } while ((light_hour_to_clear.is_open()) == 0);
  light_hour_to_clear.close();

  return 0;
  
}

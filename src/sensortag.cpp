/*****************************************************************************************//**
* @file sensortag.cpp
*
* Copyright (c)
* SmartRoboticSystems
* December 2015
*
* This program creates connection to SensorTag 2650 using hcitool & gatttool, starts
* temperature, light and humidity measurements, set up and process notifications,
* computes real world data and saves results to files temperature.txt, humidity.txt,
* light.txt on "IoT/data/" filepath. Child process & stdin/stdout redirection is utilized
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "filepaths.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

int createChildProcess(const char* command, char* const arguments[], char* const environment[])
{
  int stdin_pipe[2];
  int stdout_pipe[2];
  int child_num;
  int execve_result;
  char act_char;

  if (pipe(stdin_pipe) < 0)
  {
    perror("allocating pipe for child input redirect");
    return -1;
  }
  if (pipe(stdout_pipe) < 0)
  {
    close(stdin_pipe[PIPE_READ]);
    close(stdin_pipe[PIPE_WRITE]);
    perror("allocating pipe for child output redirect");
    return -1;
  }

  child_num = fork();
  if (0 == child_num)
  {
    // child continues here
    // redirect stdin
    if (dup2(stdin_pipe[PIPE_READ], STDIN_FILENO) == -1)
    {
      perror("redirecting stdin");
      return -1;
    }

    // redirect stdout
    if (dup2(stdout_pipe[PIPE_WRITE], STDOUT_FILENO) == -1)
    {
      perror("redirecting stdout");
      return -1;
    }

    // redirect stderr
    if (dup2(stdout_pipe[PIPE_WRITE], STDERR_FILENO) == -1)
    {
      perror("redirecting stderr");
      return -1;
    }

    // all these are for use by parent only
    close(stdin_pipe[PIPE_READ]);
    close(stdin_pipe[PIPE_WRITE]);
    close(stdout_pipe[PIPE_READ]);
    close(stdout_pipe[PIPE_WRITE]);

    // run child process image
    execve_result = execve(command, arguments, environment);

    // if we get here at all, an error occurred, but we are in the child
    // process, so just exit
    perror("exec of the child process");
    exit(execve_result);
  }
  else if (child_num > 0)
  {
    // parent continues here

    // close unused file descriptors, these are for child only
    close(stdin_pipe[PIPE_READ]);
    close(stdout_pipe[PIPE_WRITE]);

    //==============================================================
    // Connect
    //==============================================================
    const char* connect_message = "connect \r\n";
    if (NULL != connect_message)
      write(stdin_pipe[PIPE_WRITE], connect_message, strlen(connect_message));

    //Read console output
    do {
      read(stdout_pipe[PIPE_READ], &act_char, 1);
      write(STDOUT_FILENO, &act_char, 1);
    } while (act_char != '\n');

    usleep(1000000);

   //==============================================================
   // Start Temperature measurement
   //==============================================================

   //Start measurement
   const char* start_temperature = "char-write-req 0x24 01 \r\n";
   if (NULL != start_temperature)
     write(stdin_pipe[PIPE_WRITE], start_temperature, strlen(start_temperature));

   //Turn on notifications
   const char* read_temperature = "char-write-req 0x22 0100 \r\n";
   if (NULL != read_temperature)
     write(stdin_pipe[PIPE_WRITE], read_temperature, strlen(read_temperature));

   //Set notification period to 2.5 sec
   const char* temperature_period = "char-write-req 0x26 0xFA \r\n";
   if (NULL != temperature_period)
     write(stdin_pipe[PIPE_WRITE], temperature_period, strlen(temperature_period));


   //==============================================================
   // Start Light measurement
   //==============================================================

   //Start measurement
   const char* start_light = "char-write-req 0x44 01 \r\n";
   if (NULL != start_light)
     write(stdin_pipe[PIPE_WRITE], start_light, strlen(start_light));

  //Turn on notifications
   const char* read_light = "char-write-req 0x42 0100 \r\n";
   if (NULL != read_light)
     write(stdin_pipe[PIPE_WRITE], read_light, strlen(read_light));

   //Set notification period to 2.5 sec
   const char* light_period = "char-write-req 0x46 0xFA \r\n";
   if (NULL != light_period)
     write(stdin_pipe[PIPE_WRITE], light_period, strlen(light_period));


   //==============================================================
   // Start Humidity measurement
   //==============================================================

   //Start measurement
   const char* start_humidity = "char-write-req 0x2c 01 \r\n";
   if (NULL != start_humidity)
     write(stdin_pipe[PIPE_WRITE], start_humidity, strlen(start_humidity));

   //Turn on notifications
   const char* read_humidity = "char-write-req 0x2A 0100 \r\n";
   if (NULL != read_humidity)
     write(stdin_pipe[PIPE_WRITE], read_humidity, strlen(read_humidity));

   //Set notification period to 2.5 sec
   const char* humidity_period = "char-write-req 0x2e 0xFA \r\n";
   if (NULL != humidity_period)
     write(stdin_pipe[PIPE_WRITE], humidity_period, strlen(humidity_period));

/*   //==============================================================
   // Start Barometer measurement
   //==============================================================

   //Start measurement
   const char* start_barometer = "char-write-req 0x34 01 \r\n";
   if (NULL != start_barometer)
     write(aStdinPipe[PIPE_WRITE], start_barometer, strlen(start_barometer));

   //Turn on notifications
   const char* read_barometer = "char-write-req 0x32 0100 \r\n";
   if (NULL != read_barometer)
     write(aStdinPipe[PIPE_WRITE], read_barometer, strlen(read_barometer));

   //Set notification period to 2.5 sec
   const char* barometer_period = "char-write-req 0x36 0xFA \r\n";
   if (NULL != barometer_period)
     write(aStdinPipe[PIPE_WRITE], barometer_period, strlen(barometer_period));
*/

   //==============================================================
   // Process notifications
   //==============================================================

   double humidity = 0;
   double temperature = 0;
   double light = 0;
   double barometer = 0;

   const std::string temp_handle = "0x0021";
   const std::string light_handle = "0x0041";
   const std::string humidity_handle = "0x0029";
   const std::string barometer_handle = "0x0031";
   bool new_measurement = false;

   while(1)
   {
     std::string actual_line;

     do {
       read(stdout_pipe[PIPE_READ], &act_char, 1);
       actual_line += act_char;
     } while (act_char != '\n');

     //Temperature notification
     int temperature_index = actual_line.find(temp_handle);

     if(temperature_index != -1)
     {
       new_measurement = true;
       std::string temp;
       unsigned int temperature_bytes[2];

       //Extract bytes from string notifications
       temp = actual_line.substr(temperature_index + 20, 2);
       temperature_bytes[0] =  strtoul(temp.c_str(), 0, 16);

       temp = actual_line.substr(temperature_index + 23,2);
       temperature_bytes[1] =  strtoul(temp.c_str(), 0, 16);

       //Merge bytes
       unsigned int temperature_raw = (temperature_bytes[1] << 8) + temperature_bytes[0];

       //Compute and filter final value
       if((temperature > 0) && (temperature < 32768))
         temperature = temperature_raw/128.0;              //Positive temperature values
       else if (temperature > 32768)
         temperature = (temperature_raw - 65536) / 128.0;  //Negative temperature values
       else
         temperature = temperature;                        //Filtering false zero measurements
     }

     //Light notification
     int light_index = actual_line.find(light_handle);
     if(light_index != -1)
     {
       new_measurement = true;
       std::string temp;
       unsigned int light_bytes[2];

       //Extract bytes from string notifications
       temp = actual_line.substr(light_index + 14, 2);
       light_bytes[0] =  strtoul(temp.c_str(), 0, 16);

       temp = actual_line.substr(light_index + 17, 2);
       light_bytes[1] =  strtoul(temp.c_str(), 0, 16);

       //Compute exponent and fraction according to formula
       unsigned int exponent = (light_bytes[1] & 0b11110000) >> 4;
       unsigned int fraction = ((light_bytes[1] & 0b00001111) << 8) + light_bytes[0];

       //Compute final value
       light = 0.01 * pow(2,exponent) * fraction;
     }

     //Humidity notification
     int humidity_index = actual_line.find(humidity_handle);
     if(humidity_index != -1)
     {
       new_measurement = true;
       std::string temp;
       unsigned int humidity_bytes[2];

       //Extract bytes from string notification
       temp = actual_line.substr(humidity_index + 20, 2);
       humidity_bytes[0] =  strtoul(temp.c_str(), 0, 16);

       temp = actual_line.substr(humidity_index + 23,2);
       humidity_bytes[1] =  strtoul(temp.c_str(), 0, 16);

       //Merge bytes
       unsigned int humidity_raw = (humidity_bytes[1] << 8) + humidity_bytes[0];

       //Clear 2 LSB
       humidity_raw &= ~0x0003;

       //Cast to double
       humidity = static_cast<double>(humidity_raw);

       //Compute final value
       humidity = 125*(humidity/65536) - 6;
     }

     //Print if new measurement occured
     if(new_measurement == true)
     {
       std::cout << "Temp: "        << std::setprecision(4)  << std::setw(5) << std::setfill(' ') << temperature << " [°C]"
                 << "   Light: "    << std::setprecision(4) << std::setw(5) << std::setfill(' ') << light << " [lux]"
                 << "   Humidity: " << std::setprecision(4) << std::setw(5) << std::setfill(' ') << humidity << " [%]"  << std::endl;

       //Clear the flag
       new_measurement = false;

       std::ofstream temperature_minute;
       std::ofstream humidity_minute;
       std::ofstream light_minute;

       //Try to open the _MINUTE file for writing
       do {
         temperature_minute.open(TEMPERATURE_MINUTE_FILEPATH);
       } while((temperature_minute.is_open()) == 0);

       temperature_minute << temperature;
       temperature_minute.close();

       do {
         humidity_minute.open(HUMIDITY_MINUTE_FILEPATH);
       } while((humidity_minute.is_open()) == 0);

       humidity_minute << humidity;
       humidity_minute.close();

       do {
         light_minute.open(LIGHT_MINUTE_FILEPATH);
       } while((light_minute.is_open()) == 0);

       light_minute << light;
       light_minute.close();
     }
   }

   //==============================================================
   // Exit
   //==============================================================
   const char* exit_communication = "exit\r\n";
   if (NULL != exit_communication) {
     write(stdin_pipe[PIPE_WRITE], exit_communication, strlen(exit_communication));
   }

    // done with these in this example program, you would normally keep these
    // open of course as long as you want to talk to the child
    close(stdin_pipe[PIPE_WRITE]);
    close(stdout_pipe[PIPE_READ]);
  }
  else
  {
    // failed to create child
    close(stdin_pipe[PIPE_READ]);
    close(stdin_pipe[PIPE_WRITE]);
    close(stdout_pipe[PIPE_READ]);
    close(stdout_pipe[PIPE_WRITE]);
  }
  return child_num;
}

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    std::cerr << "Error: missing argument! Correct Usage: ./iot AA:BB:CC:DD:EE:FF" << std::endl;
    return(-1);
  }
  else if(argc == 2)
  {
    char* args[4];

    args[0] = const_cast<char*>("gatttool");
    args[1] = const_cast<char*>("-b");
    args[2] = argv[1];
    args[3] = const_cast<char*>("-I");

    std::cout << "Connecting to device: [" << argv[1] << "]" << std::endl;
    std::string hcitool_lecc("sudo hcitool lecc ");
    hcitool_lecc += argv[1];
    system(hcitool_lecc.c_str());

    //Create separate process + redirect console
    createChildProcess("/usr/bin/gatttool",args, NULL);

    std::cout << "Disconnecting from device: [" << argv[1] << "]" << std::endl;
    std::string hcitool_cc("sudo hcitool cc ");
    hcitool_cc += argv[1];
    system(hcitool_cc.c_str());

    return(0);
  }
}

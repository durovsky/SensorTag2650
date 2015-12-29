cmake_minimum_required(VERSION 2.8.3)
project(iot)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/)

#SensorTag executable
SET(SENSORTAG_SOURCES ${PROJECT_SOURCE_DIR}/src/sensortag.cpp)
SET(SENSORTAG_HEADERS ${PROJECT_SOURCE_DIR}/include/filepaths.h)

add_executable(sensortag ${SENSORTAG_SOURCES} ${SENSORTAG_HEADERS})

#ThingSpeak Minute Update
SET(MIN_SOURCES ${PROJECT_SOURCE_DIR}/src/thingspeak_minute_update.cpp
                ${PROJECT_SOURCE_DIR}/src/socket_client.cpp)

SET(HEADERS ${PROJECT_SOURCE_DIR}/include/socket_client.h
            ${PROJECT_SOURCE_DIR}/include/filepaths.h
            ${PROJECT_SOURCE_DIR}/include/key.h)
            
add_executable(thingspeak_minute_update ${MIN_SOURCES} ${MIN_HEADERS})

#ThingSpeak Hour Update
SET(HOUR_SOURCES ${PROJECT_SOURCE_DIR}/src/thingspeak_hour_update.cpp
                 ${PROJECT_SOURCE_DIR}/src/socket_client.cpp)

SET(HOUR_HEADERS ${PROJECT_SOURCE_DIR}/include/socket_client.h
                 ${PROJECT_SOURCE_DIR}/include/filepaths.h
                 ${PROJECT_SOURCE_DIR}/include/key.h)
             
add_executable(thingspeak_hour_update ${HOUR_SOURCES} ${HOUR_HEADERS})







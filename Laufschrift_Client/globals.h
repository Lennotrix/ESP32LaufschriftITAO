#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <iostream>

#undef NO_WIFI // No Wifi mode
#define DEBUG // Debug mode

#define HTTP_MAX_LEN 4096
#define ENDPOINT_MAX_LENGTH 255

// Status Led Pins
#define ERROR_PIN 27
#define BOOT_PIN 26
#define RUNNING_PIN 25

#define MAX_DEVICES 10
#define MATRIX_PACE 40
#define MATRIX_INTENSITY 15 //Value between 1 - 15

#endif
# Arduino LED Matrix Display Project Documentation

## Overview

The Arduino LED Matrix Display project is designed to create a scrolling text display on an LED matrix, fetching phrases from a remote server. It utilizes an ESP32 microcontroller, MD_MAX72xx LED matrix, and various libraries for functionality. Additionally, user interaction is facilitated through the Serial Monitor for updating configuration settings stored in the EEPROM.

## Additional Notes

- Ensure correct libraries are installed in the Arduino IDE.
- Modify server endpoints, credentials, and other constants as needed.
- Customize LED matrix behavior and animations based on project requirements.


## External libraries

1. **ESP32 Development Board:** The main microcontroller.
2. **MD_MAX72xx LED Matrix:** Matrix of LEDs for displaying scrolling text.
3. **HTTPClient:** Facilitates HTTP requests to a remote server.
4. **Arduino_JSON:** Parses and generates JSON data.
5. **EEPROM:** Reads and writes data to the EEPROM.
6. **UrlEncode:** Encodes URLs for communication.
7. **MD_Parola:** Controls LED matrix display and scrolling text.
8. **freertos:** Creates new threads.
9. **iostream:** Input Output Stream.
10. **Arduino:** For programm functionality.
## Files

1. **Laufschrift_Client.ino**
2. **http.h**
3. **http.cpp**
4. **ITAO_EEPROM.h**
5. **ITAO_EEPROM.cpp**
6. **globals.h**
# Main file (`Laufschrift_Cient.ino`)

#### Constants

- `HARDWARE_TYPE`, `CS_PIN`, `CLK_PIN`, `DATA_PIN`,: LED matrix configuration.

#### Objects and Variables

- `MD_Parola`, `MD_MAX72xx`: LED matrix controller.
- `Wifi`: Manages multiple WiFi connections.
- `UrlEncode`: Encodes an URL.
- `freertos`: Creates multiple Tasks.
- `Arduino_JSON`: Serializes and deserializes Json.
- `http`: HTTP client for server communication.
- `ITAO_EEPROM`: Custom library for handling EEPROM data.
- `globals.h`: Global constants.

#### Setup Function

- Initializes pins, LED matrix, WiFi, and other components.
- Initializes EEPROM data.
- Creates a separate thread for booting LED animation.
- Displays initial information on the LED matrix.

#### Loop Function

- Animates the LED matrix.
- Listens for input from the Serial interface.

#### Functions

- `zeigeDaten`: Displays information about the stored data.
- `InitEEPROM`: Initializes EEPROM data.
- `ESPDayCheck`: Restarts the ESP after specific amount of time.
- `animate`: Handles LED matrix animation and fetches phrases from the server.
- `SerialInput`: Processes input from the Serial interface.
- `handleJsonError`: Sets the Error Led to High for 2000ms if Json is incorrect.
- `updateEEPROM`: Inserts the new EEPROM data to the EEPROM.
- `validateJson`: Checks if delivered Json is correct.
- `LedMatrixReset`: Resets and reinitializes the LED matrix.
- `sonderzeichen195`: Converts special characters from ASCII 195 to Unicode values.
- `fillTextBuffer`: Fills the display buffer with text.
- `sendWhatsApp`: Sends a WhatsApp message using the CallMeBot API.
- `Booting`: LED animation for the booting process.
- `PinSetup`: Configures and initializes status LEDs.
- `ErrorLed`, `BootLed`, `RunLed`: Controls status LEDs.
## HTTP library (`http.h`, `http.cpp`)

# HTTP Class Implementation

This CPP file implements the functionalities of the `http` class defined in the corresponding header file. It facilitates HTTP requests and responses and is designed to work with Arduino devices. The file depends on several libraries including `HTTPClient.h`, `Arduino_JSON.h`, `globals.h`, and the custom `http.h`.

## Variables

- `phraseEndpoint`: Stores the endpoint URL for fetching phrases. It changes based on whether the code is in debug mode or not.

## Constructor

The constructor initializes an instance of the `http` class, taking a pointer to an `ITAO_LAUFSCHRIFT_DATEN` object as a parameter. It also performs a login operation.

## Destructor

The destructor doesn't contain any functionality.

## Methods

### GetPhrase

This method retrieves a phrase from an HTTP response. It constructs the endpoint URL using the provided data and sends an HTTP GET request to fetch the phrase.

### appendPhrase

This private method appends a phrase to a JSON object.

### Login

This method performs a login operation over HTTP. It constructs the login endpoint URL using the provided data and sends an HTTP POST request with the username and password. Upon successful login, it retrieves and stores the bearer token.

## Usage

- Include the `http.h` header file in your Arduino sketch.
- Include the necessary libraries such as `HTTPClient.h`, `Arduino_JSON.h`, and `globals.h`.
- Implement an instance of the `http` class, passing the required `ITAO_LAUFSCHRIFT_DATEN` object pointer to the constructor.
- Utilize the class methods to perform HTTP requests and retrieve data.


# EEPROM library (`ITAO_EEPROM.h`, `ITAO_EEPROM.cpp)

This header file defines a C++ class named `ITAO_EEPROM` for interfacing with EEPROM memory on Arduino devices. It provides functionality for writing and reading data to and from EEPROM. The file also includes a structure `ITAO_LAUFSCHRIFT_DATEN` that defines the layout of the data stored in EEPROM.

## Data Structure

### ITAO_LAUFSCHRIFT_DATEN

This structure defines the layout of the data stored in EEPROM. It includes the following fields:
- `api_username`: A character array to store the API username (max 30 characters).
- `api_password`: A character array to store the API password (max 30 characters).
- `router_ssid`: A character array to store the router SSID (max 70 characters).
- `router_password`: A character array to store the router password (max 50 characters).
- `api_endpoint`: A character array to store the API endpoint (max 255 characters).

## Class Methods

### Constructor
 - Initializes an instance of the ITAO_EEPROM class.
 
### Destructor
 - Destroys the instance of the ITAO_EEPROM class.
 - Deletes the dynamically allocated memory of readEEPROM.

### WriteEEPROM
```cpp
template <typename T>
static bool WriteEEPROM(const T& data);
```
 - Writes data to EEPROM. Takes the data to be written as a parameter.

### readEEPROM
```cpp
template <typename T>
static T* ReadEEPROM();
```
 - Reads data from EEPROM. Returns a pointer to the read data.

## Usage
 - Include the ITAO_EEPROM.h header file in your Arduino sketch.
 - Utilize the ITAO_EEPROM class to read from and write to EEPROM.
 - Define the data structure (ITAO_LAUFSCHRIFT_DATEN) according to your requirements before using the class.
# Global (`globals.h`)

This header file, `globals.h`, defines various global constants and macros for an Arduino project. It includes definitions for pins, modes, and other parameters that are used across the project.

## Macros

### NO_WIFI
- This macro, when defined, sets the project to "No WiFi mode", indicating that WiFi functionality is disabled.

### DEBUG
- This macro, when defined, enables debug mode in the project, allowing for debugging statements and behaviors.

## Constants

### HTTP_MAX_LEN
- Defines the maximum length of HTTP data. It is set to 4096 bytes.

### ENDPOINT_MAX_LENGTH
- Defines the maximum length of an endpoint URL. It is set to 255 characters.

### Status LED Pins
- Defines the pin numbers for different status LEDs:
  - `ERROR_PIN`: Pin number for the error LED (27).
  - `BOOT_PIN`: Pin number for the boot LED (26).
  - `RUNNING_PIN`: Pin number for the running LED (25).

### MAX_DEVICES
- Defines the maximum number of devices. It is set to 10.

### MATRIX_PACE
- Defines the pace for the matrix.

### MATRIX_INTENSITY
- Defines the intensity for the matrix. It is a value between 1 and 15.

## Usage
- Include the `globals.h` header file in your Arduino sketch.
- Utilize the defined macros and constants as needed throughout your project.


## Deployment


1. **Initial Setup:**
   - Connect the ESP32 board to the LED matrix and status LEDs.
   - Connect Pin of Matrix: DIN = 27, CS = 21, CLK = 18.
   - Modify EEPROM with **API:** Username, Password, Endpoint. **Wifi**: SSID, Password.
   - To change the EEPROM value you can use this example command:
     ```bash
     #EEPROM%{"EEusername":`your_api_username`,"EEpassword":`your_api_password`,
     "EEssid":`your_ssid`,"EEIPassword":`your_wifi_password`,"EEEndpoint":"`your_endpoint`"};
     ```

2. **Compile and Upload:**
   - Use the Arduino IDE to compile and upload the code to the ESP32 board.

3. **Commands**
   - Commands first character is alway a `#`, followed by the commandname for example`EEPROM`. If there are parameter you want to pass use the `%` character, followed by the parameter in json format.
   
   **List of Commands**
   - EEPROM
   - Restart



## Authors

- [@Lennard](https://www.github.com/Lennotrix)
- [@David](https://github.com/hdnbertdave)


# Arduino LED Matrix Display Project Documentation

## Overview

The Arduino LED Matrix Display project is designed to create a scrolling text display on an LED matrix, fetching phrases from a remote server. It utilizes an ESP32 microcontroller, MD_MAX72xx LED matrix, and various libraries for functionality. Additionally, user interaction is facilitated through the Serial Monitor for updating configuration settings stored in the EEPROM.

## Components Used

1. **ESP32 Development Board:** The main microcontroller.
2. **MD_MAX72xx LED Matrix:** Matrix of LEDs for displaying scrolling text.
3. **WiFiMulti:** Manages multiple WiFi connections.
4. **HTTPClient:** Facilitates HTTP requests to a remote server.
5. **Arduino_JSON:** Parses and generates JSON data.
6. **EEPROM:** Reads and writes data to the EEPROM.
7. **UrlEncode:** Encodes URLs for communication.
8. **MD_Parola:** Controls LED matrix display and scrolling text.

## Project Structure

### Main File (`Laufschrift_Client.ino`)

#### Constants

- `HARDWARE_TYPE`, `MAX_DEVICES`, `CS_PIN`: LED matrix configuration.
- `ERROR_PIN`, `BOOT_PIN`, `RUNNING_PIN`: Status LED pins.

#### Objects and Variables

- `MD_Parola`: LED matrix controller.
- `WiFiMulti`: Manages multiple WiFi connections.
- `http`: HTTP client for server communication.
- `ITAO_EEPROM`: Custom library for handling EEPROM data.

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
- `animate`: Handles LED matrix animation and fetches phrases from the server.
- `SerialInput`: Processes input from the Serial interface.
- `LedMatrixReset`: Resets and reinitializes the LED matrix.
- `sonderzeichen195`: Converts special characters from ASCII 195 to Unicode values.
- `fillTextBuffer`: Fills the display buffer with text.
- `sendWhatsApp`: Sends a WhatsApp message using the CallMeBot API.
- `Booting`: LED animation for the booting process.
- `PinSetup`: Configures and initializes status LEDs.
- `ErrorLed`, `BootLed`, `RunLed`: Controls status LEDs.

### HTTP Library (`http.h, http.cpp`)

#### Constants

- `HTTP_MAX_LEN`: Maximum length for HTTP-related buffers.

#### Objects and Variables

- `uName`, `uPw`: Username and password for server authentication.
- `pBearer`: Bearer token for authentication.

#### Constructor

- Initializes username, password, and obtains a bearer token through login.

#### Destructor

- Not implemented.

#### Functions

- `GetPhrase`: Fetches a phrase from the server and returns it.
- `Login`: Performs user authentication and obtains a bearer token.

### EEPROM Class (`ITAO_EEPROM.h`, `ITAO_EEPROM.cpp`)

#### Constants

- `EE_START`: Starting address in EEPROM for storing the data.

#### Methods

- `bool WriteEEPROM(ITAO_LAUFSCHRIFT_DATEN werte)`: Writes the provided data structure to the EEPROM.
- `ITAO_LAUFSCHRIFT_DATEN ReadEEPROM()`: Reads the data structure from the EEPROM.

### `ITAO_LAUFSCHRIFT_DATEN` Structure

- `EEusername[30]`: Character array for storing the username (max 30 characters).
- `EEpassword[30]`: Character array for storing the password (max 30 characters).
- `EEssid[70]`: Character array for storing the WiFi SSID (max 70 characters).
- `EEIPassword[50]`: Character array for storing the WiFi password (max 50 characters).

## Usage

1. **Initial Setup:**
   - Connect the ESP32 board to the LED matrix and status LEDs.
   - Connect Pin of Matrix: DIN = 27, CS = 21, CLK = 18.
   - Modify EEPROM with **API:** Username, Password. **Wifi**: SSID, Password.

2. **Compile and Upload:**
   - Use the Arduino IDE to compile and upload the code to the ESP32 board.

3. **Monitor Serial Interface:**
   - Open the Serial Monitor to view debug information and input options.

4. **Interact via Serial Interface:**
   - Send commands through the Serial Monitor, including EEPROM data updates.

## Additional Notes

- Ensure correct libraries are installed in the Arduino IDE.
- Modify server endpoints, credentials, and other constants as needed.
- Customize LED matrix behavior and animations based on project requirements.


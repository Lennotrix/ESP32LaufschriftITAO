
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

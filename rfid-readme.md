# RFID-RC522 Arduino Reader/Writer

This project implements a complete RFID tag reader and writer using an Arduino and the MFRC522 RFID module. It allows for both reading existing tags and writing new data to them through serial communication.

## Hardware Requirements

- Arduino board (Uno, Nano, etc.)
- RFID-RC522 module
- RFID tags (MIFARE Classic, Ultralight, etc.)
- Jumper wires

## Wiring Instructions

Connect the RFID-RC522 module to your Arduino using the following pin layout:

| RC522 Pin | Arduino Pin | Description |
|-----------|-------------|-------------|
| SDA (SS)  | 10         | Chip Select |
| SCK       | 13         | Clock       |
| MOSI      | 11         | Data Output |
| MISO      | 12         | Data Input  |
| IRQ       | NC         | Not Connected|
| GND       | GND        | Ground      |
| RST       | 9          | Reset       |
| 3.3V      | 3.3V       | Power       |

## Software Requirements

### Libraries
- SPI.h (built into Arduino IDE)
- MFRC522.h (install via Library Manager)

### Installation
1. Install the Arduino IDE
2. Install the MFRC522 library through the Arduino Library Manager
3. Upload the provided code to your Arduino

## Features

- Continuous tag monitoring
- Automatic tag detection and reading
- Write capability through serial communication
- Support for different MIFARE tag types
- Debug mode for troubleshooting

## Usage

### Reading Tags
1. Power up the Arduino
2. Present an RFID tag to the reader
3. The tag's content will be automatically read and sent through serial

### Writing Tags
1. Open the Serial Monitor (9600 baud)
2. Type the text you want to write
3. Press Send
4. Present the tag when ready
5. Wait for confirmation response:
   - "OK" = Write successful
   - "KO" = Write failed

### Serial Output Messages
- "Detected" - New tag found
- "Gone" - Tag removed
- "OK" - Write operation successful
- "KO" - Write operation failed

## Debug Mode

To enable debug mode:
1. Change `#define DEBUG 0` to `#define DEBUG 1`
2. Upload the modified code
3. Open Serial Monitor to see detailed debug messages

## Supported Tag Types

- MIFARE Ultralight
- MIFARE Classic 1K
- MIFARE Classic 4K
- MIFARE Mini

## Notes

- The code automatically adjusts writing parameters based on tag type
- Writing starts from page 4 to preserve tag metadata
- All text is automatically trimmed before writing
- Maximum text length depends on tag type

## Troubleshooting

1. **No tag detection**
   - Check wiring connections
   - Verify tag type is supported
   - Ensure proper power supply (3.3V)

2. **Write failures**
   - Check if tag is write-protected
   - Keep tag stationary during writing
   - Verify tag has sufficient memory

3. **Inconsistent readings**
   - Reduce metal interference near the antenna
   - Check for proper grounding
   - Maintain stable power supply

## Contributing

Feel free to submit issues and enhancement requests.
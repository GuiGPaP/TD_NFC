# RFID Tag Reader/Writer with Arduino and TouchDesigner

This project implements a complete RFID tag reader/writer system using an Arduino with MFRC522 module and TouchDesigner integration. It provides both hardware interfacing for RFID operations and a visual interface for seamless interaction.

## Project Structure

### Hardware Components
- Arduino board (Uno, Nano, etc.)
- RFID-RC522 module
- RFID tags (MIFARE compatible)
- Jumper wires

### TouchDesigner Components
- **Serial1** (Serial DAT): Handles serial communication with Arduino
- **status** (Constant CHOP): Stores system status with channels:
  - tag_detected: Indicates if a tag is present
  - done: Success status of write operations
  - error: Error status of write operations
- **text_read** (Text DAT): Stores content read from tags
- **text_to_write** (Text DAT): Contains content to be written to tags

## Hardware Setup

### Wiring Instructions

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

### Arduino
- Arduino IDE
- Libraries:
  - SPI.h (built into Arduino IDE)
  - MFRC522.h (install via Library Manager)

### TouchDesigner
- Recent version of TouchDesigner
- Serial communication capabilities

## System Setup

1. Arduino Setup:
   - Install required libraries
   - Upload the provided Arduino sketch
   - Verify serial communication works

2. TouchDesigner Setup:
   - Configure the Serial DAT:
     - Set correct COM port
     - Set baud rate to 9600

## Communication Protocol

### Arduino to TouchDesigner Messages
- `Detected`: New tag presence
- `Gone`: Tag removal
- `OK`: Successful write
- `KO`: Write failure
- Any other message: Tag content

### TouchDesigner to Arduino Messages
- Write content is sourced from text_to_write DAT


## Usage

### Reading Tags
1. Present tag to reader
2. System automatically:
   - Detects tag presence
   - Reads content
   - Updates status
   - Displays content in text_read DAT

### Writing Tags
1. Enter desired content in text_to_write DAT
2. Present tag when ready
3. Trigger send operation
4. Check status channels for:
   - Operation progress
   - Success/failure indication

## Supported Tag Types

- MIFARE Ultralight (48 bytes storage)
- MIFARE Classic 1K
- MIFARE Classic 4K
- MIFARE Mini

## Debug and Troubleshooting

### Arduino Debug Mode
1. Set `#define DEBUG 1` in Arduino code
2. Upload modified code
3. Monitor serial output for detailed messages

### Common Issues

1. **No Tag Detection**
   - Check wiring connections
   - Verify tag compatibility
   - Ensure proper power supply (3.3V)

2. **Write Failures**
   - Verify tag presence
   - Check write protection
   - Confirm content length is appropriate
   - Keep tag stationary during operation

3. **Communication Issues**
   - Verify COM port settings
   - Check serial connection status
   - Ensure correct baud rate (9600)

4. **Inconsistent Operation**
   - Reduce metal interference
   - Check grounding
   - Maintain stable power supply

## Notes and Limitations

- Content length limited by tag type
- MIFARE Ultralight has 48 byte limit
- Always monitor status channels
- Keep tags stationary during operations
- Avoid metal interference with antenna
- Regular system status checking recommended

## Contributing

Feel free to submit:
- Bug reports
- Feature requests
- Pull requests
- Documentation improvements
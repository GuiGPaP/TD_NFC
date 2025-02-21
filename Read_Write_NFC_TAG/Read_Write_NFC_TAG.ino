/*
RFID-RC522 Reader/Writer for Arduino

Connection instructions:
RC522 Pin  ->  Arduino Pin
--------------------------------
SDA (SS)   ->  10
SCK        ->  13
MOSI       ->  11
MISO       ->  12
IRQ        ->  Not connected
GND        ->  GND
RST        ->  9
3.3V       ->  3.3V

Description:
This code implements both reading and writing capabilities for RFID tags using
the MFRC522 module. It includes periodic tag detection and serial communication
for writing new data to tags.

Usage:
- The program continuously checks for tag presence
- When a tag is detected, it reads and outputs its content
- To write data, send the text through serial connection (ending with newline)
- The program responds with:
  * "Detected" when a new tag is present
  * "Gone" when a tag is removed
  * "OK" when writing succeeds
  * "KO" when writing fails
*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          
#define SS_PIN          10         
#define DEBUG           0          // 1: Enable debug mode

MFRC522 mfrc522(SS_PIN, RST_PIN);
String inputString = "";          // String to store received data
bool stringComplete = false;      // Flag to indicate if message is complete
bool lastTagState = false;        // To detect tag state changes
unsigned long lastTagCheck = 0;    // Last timestamp of tag check
const int TAG_CHECK_INTERVAL = 100; // Interval between checks (ms)

void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();
    mfrc522.PCD_Init();
    inputString.reserve(256);
}

void loop() {
    // For writing - If a serial command is received
    if (stringComplete) {
        if (DEBUG) {
          Serial.println("[DEBUG] Serial command received: " + inputString);
        }
        mfrc522.PCD_Init(); // Reinitialize the reader
        delay(50);
        
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            if (writeTag(inputString)) {
                Serial.println("OK");
            } else {
                Serial.println("KO");
            }
        } else {
            Serial.println("KO");
        }
        inputString = "";
        stringComplete = false;
    }

    // Periodic tag presence check
    if (millis() - lastTagCheck >= TAG_CHECK_INTERVAL) {
        lastTagCheck = millis();
        
        // Reinitialize reader before testing tag presence
        mfrc522.PCD_Init();
        delay(50);
        
        bool tagPresent = mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
        
        if (tagPresent && !lastTagState) {
            if (DEBUG) {
              Serial.println("[DEBUG] New tag detected");
            }
            Serial.println("Detected");
            readTag();
            lastTagState = true;
        }
        else if (!tagPresent && lastTagState) {
            if (DEBUG) {
              Serial.println("[DEBUG] Tag removed");
            }
            Serial.println("Gone");
            lastTagState = false;
        }
        
        if (tagPresent) {
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }
    }
}

void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inputString += inChar;
        if (inChar == '\n') {
            stringComplete = true;
        }
    }
}

bool writeTag(String text) {
    MFRC522::StatusCode status;
    byte buffer[16];
    byte maxPage = 15;  // Conservative default value
    
    text.trim();
    if (DEBUG) {
      Serial.println("[DEBUG] ====== Start writeTag() ======");
    }
    // Tag type identification
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (DEBUG) {
      Serial.print("[DEBUG] Tag type: ");
      Serial.println(mfrc522.PICC_GetTypeName(piccType));
    }
    
    // Adjust maximum page based on tag type
    switch (piccType) {
        case MFRC522::PICC_TYPE_MIFARE_UL:
            maxPage = 15;  // MIFARE Ultralight
            break;
        case MFRC522::PICC_TYPE_MIFARE_MINI:
            maxPage = 20;
            break;
        case MFRC522::PICC_TYPE_MIFARE_1K:
            maxPage = 64;
            break;
        case MFRC522::PICC_TYPE_MIFARE_4K:
            maxPage = 256;
            break;
        default:
            maxPage = 15;  // Conservative default value
    }
    if (DEBUG) {
      Serial.print("[DEBUG] Available pages: 4 to ");
      Serial.println(maxPage);
      
      Serial.print("[DEBUG] Text to write (length=");
      Serial.print(text.length());
      Serial.print("): ");
      Serial.println(text);

      // First clear tag content (write zeros)
      Serial.println("[DEBUG] Erasing tag...");
    }
    for (byte pageAddr = 4; pageAddr <= maxPage; pageAddr++) {
        byte emptyPage[4] = {0, 0, 0, 0};
        status = mfrc522.MIFARE_Ultralight_Write(pageAddr, emptyPage, 4);
        
        if (status != MFRC522::STATUS_OK) {
            if (pageAddr > 4) {  // If we managed to erase at least a few pages
                break;  // Stop erasing and continue with writing
            } else {
              if (DEBUG) {
                Serial.print("[DEBUG] Critical error while erasing page ");
                Serial.println(pageAddr);
              }
                return false;
            }
        }
    }
    if (DEBUG) {
      Serial.println("[DEBUG] Erasing complete");
    }
    // Write new content
    byte textLength = text.length();
    text.getBytes(buffer, sizeof(buffer));
    
    byte pageAddr = 4;
    byte bytesWritten = 0;
    
    while (bytesWritten < textLength && pageAddr <= maxPage) {
        byte dataToWrite[4] = {0, 0, 0, 0};
        
        for (byte i = 0; i < 4 && (bytesWritten + i) < textLength; i++) {
            dataToWrite[i] = buffer[bytesWritten + i];
        }
        
        status = mfrc522.MIFARE_Ultralight_Write(pageAddr, dataToWrite, 4);
        
        if (status != MFRC522::STATUS_OK) {
          if (DEBUG) {
            Serial.print("[DEBUG] Write error at page ");
            Serial.println(pageAddr);
          }
            return false;
        }
        if (DEBUG) {
          Serial.print("[DEBUG] Page ");
          Serial.print(pageAddr);
          Serial.println(" written successfully");
        }
        pageAddr++;
        bytesWritten += 4;
    }
    
    return true;
}

void readTag() {
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    String content = "";
    
    if (DEBUG) {
      Serial.println("[DEBUG] ====== Start readTag() ======");
    }
    for (byte page = 4; page < 48; page++) {
        status = mfrc522.MIFARE_Read(page, buffer, &size);
        
        if (status == MFRC522::STATUS_OK) {
            for (byte i = 0; i < 4; i++) {
                if (buffer[i] >= 32 && buffer[i] <= 126) {
                    content += (char)buffer[i];
                } else if (buffer[i] == 0 && content.length() > 0) {
                    Serial.println(content);
                    return;
                }
            }
        } else {
            if (content.length() > 0) {
                Serial.println(content);
            }
            return;
        }
    }
    
    if (content.length() > 0) {
        Serial.println(content);
    }
}


#include <SD.h>
#include <SPI.h>
#include <I2S.h>

//instantiate state machine
enum PhoneState {
  IDLE,
  COLLECTING,
  PLAYING
};

PhoneState state = IDLE;

//create i2s port
I2S i2s(OUTPUT, 7, 9);

//Vars
const int dialPin = 28;
const int SD_CS = 17;
const unsigned long minPulseGap = 50;
const unsigned long digitTimeout = 1000;
const unsigned long numberTimeout = 3000;

int pulseCount = 0;
bool lastDialState = HIGH;
unsigned long lastPulseTime = 0;
unsigned long lastValidPulseTime = 0;
unsigned long lastDigitTime = 0;

char numberBuffer[16];
int bufferIndex = 0;

// Mapping structure
struct InputMapping {
  const char* code;
  const char* filename;
};

InputMapping Map[] = {
  {"12", "charliesphone.wav"},
  {"77668899", "file_7.wav"},

};

const int numMappings = sizeof(Map) / sizeof(InputMapping);
const char* selectedFile = nullptr;
File audioFile;

void setup() {
  pinMode(dialPin, INPUT_PULLUP);
  Serial.begin(115200);
  while(!Serial);
  clearBuffer();

  //Initialise SD Module
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card init failed!");
    //while (true);
  }
  //Initialise i2s
  if (!i2s.begin(44100)) {
    Serial.println("I2S init failed!");
   // while (true);
  }

  Serial.println("System ready.");
}

void loop() {
  switch (state) {
    case IDLE:
      if (digitalRead(dialPin) == LOW) {
        state = COLLECTING;
        Serial.println("Dialing started...");
     
      }
      break;

    case COLLECTING:
      handleRotaryInput();
      //Collect 1 digit at a time
      if (bufferIndex > 0 && (millis() - lastDigitTime > numberTimeout)) {
        Serial.print("Full number: ");
        Serial.println(numberBuffer);

        if (lookupFile(numberBuffer)) {
          Serial.print("Valid number, playing: ");
          Serial.println(selectedFile);
          state = PLAYING;
        } else {
          Serial.println("Invalid number.");
          state = IDLE;
        }

        clearBuffer();
      }
      break;

    case PLAYING:
    //Play corresponding file
      playWav(selectedFile);
      Serial.println("Done playing.");
      selectedFile = nullptr;
      state = IDLE;
      break;
  }
}

void handleRotaryInput() {
  int currentState = digitalRead(dialPin);

  if (lastDialState == HIGH && currentState == LOW) {
    unsigned long now = millis();
    if (now - lastValidPulseTime > minPulseGap) {
      pulseCount++;
      lastValidPulseTime = now;
      lastPulseTime = now;
    }
  }

  if (pulseCount > 0 && (millis() - lastPulseTime > digitTimeout)) {
    int digit = (pulseCount == 10) ? 0 : pulseCount;
    storeDigit(digit);
    Serial.print("Got digit: ");
    Serial.println(digit);
    pulseCount = 0;
    lastDigitTime = millis();
  }

  lastDialState = currentState;
}

void storeDigit(int d) {
  if (bufferIndex < sizeof(numberBuffer) - 1) {
    numberBuffer[bufferIndex++] = '0' + d;
    numberBuffer[bufferIndex] = '\0';
  }
}

void clearBuffer() {
  bufferIndex = 0;
  numberBuffer[0] = '\0';
}

bool lookupFile(const char* code) {
  for (int i = 0; i < numMappings; ++i) {
    if (strcmp(code, Map[i].code) == 0) {
      selectedFile = Map[i].filename;
      return true;
    }
  }
  return false;
}

void playWav(const char* filename) {
  audioFile = SD.open(filename);
  if (!audioFile) {
    Serial.println("Failed to open file.");
    return;
  }

  // Skip WAV header
  for (int i = 0; i < 44; i++) {
    if (audioFile.read() < 0) {
      Serial.println("Failed to skip WAV header.");
      audioFile.close();
      return;
    }
  }

  uint8_t buffer[512];
  while (audioFile.available()) {
    size_t bytesRead = audioFile.read(buffer, sizeof(buffer));
    i2s.write(buffer, bytesRead);
    //Serial.println("speaking.............");
  }
  audioFile.close();
}



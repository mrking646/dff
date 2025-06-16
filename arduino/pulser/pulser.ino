#include <Regexp.h>

const int PIN_CLK = 4;
const int PIN_RESET = 5;
const int PIN_DIN_R = 6;
const int PIN_DIN_C = 7;
const int ALL_0_512 = -1;
const int ALL_1_512 = 999;
const int ALL_1_rc = 998

int pulseCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DIN_R, OUTPUT);
  pinMode(PIN_DIN_C, OUTPUT);
  digitalWrite(PIN_CLK, LOW);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_DIN_R, LOW);
  digitalWrite(PIN_DIN_C, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    MatchState ms;
    ms.Target((char*)message.c_str());

    // Match pattern "r(\d+)c(\d+)"
    char result = ms.Match("r(%d+)c(%d+)");

    if (result == REGEXP_MATCHED) {
      // Extract captured groups
      char rowStr[10], colStr[10];
      ms.GetCapture(rowStr, 0); // row
      ms.GetCapture(colStr, 1); // col

      int row = atoi(rowStr);
      int col = atoi(colStr);

      Serial.print("Row: ");
      Serial.print(row);
      Serial.print(", Column: ");
      Serial.println(col);
      if (row > col) { // row is greater than column, r10c2, for example, shift 8 rows and then input col and shift 2 times
        // so, rxcy, shift row - 1 - col times, 
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(PIN_DIN_R);
        delayMicroseconds(50);
        shiftPulses(row - 1 - col); // already did one
        loadFirstOne(PIN_DIN_C);
        delayMicroseconds(50);
        shiftPulses(col - 1); // already did one

      }
      else if (row < col) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(PIN_DIN_C);
        delayMicroseconds(50);
        shiftPulses(col - 1 - row); // already did one
        loadFirstOne(PIN_DIN_R);
        delayMicroseconds(50);
        shiftPulses(row - 1);

      }
      else if (row == col) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(PIN_DIN_R);
        delayMicroseconds(50);
        loadFirstOne(PIN_DIN_C);
        delayMicroseconds(50);
        shiftPulses(row - 1); // already did one
      }

    } else if (message.toInt() != 0 || message == "0") {

      pulseCount = message.toInt();

      if (pulseCount == ALL_0_512) {
        resetShiftRegister();
        Serial.println("Only reset.");

      } else if (pulseCount == ALL_1_512) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne();
        delayMicroseconds(50);

        for (int i = 0; i < 512; i++) {
          loadFirstOne(); // Push 512 '1's
        }
        Serial.println("All 1!.");


      } else if (pulseCount == ALL_1_rc) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne();
        delayMicroseconds(50);

        for (int i = 0; i < 100; i++) {
          loadFirstOne(PIN_DIN_R); // Push 100 '1's
          delayMicroseconds(50);
          loadFirstOne(PIN_DIN_C); // Push 100 '1's
          delayMicroseconds(50);
        }
        Serial.println("All 1 rc!.");
      } else if (pulseCount > 0 && pulseCount < 513) {
        Serial.print("Pulse count received: ");
        Serial.println(pulseCount);

        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne();
        delayMicroseconds(50);
        shiftPulses(pulseCount - 1);
      } else {
        Serial.println("Invalid or zero input.");
      }
    }
    // else {
    //   Serial.println("Error: Expected format rXXcYY or integer input");
    // }
  }
}

// --- Utility functions ---

void resetShiftRegister() {
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(20);
  clkPulse();
  delayMicroseconds(20);
  digitalWrite(PIN_RESET, LOW);
}

void loadFirstOne(int pin) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  clkPulse();
  digitalWrite(pin, LOW);
}

void shiftPulses(int count) {
  for (int i = 0; i < count; i++) {
    clkPulse();
  }
}

void clkPulse() {
  digitalWrite(PIN_CLK, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_CLK, LOW);
  delayMicroseconds(40);
}

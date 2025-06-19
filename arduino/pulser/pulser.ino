#include <Regexp.h>

const int PIN_CLK = 4;
const int PIN_RESET = 5;
const int PIN_DIN_R = 6;
const int *PIN_DIN_R_ptr = &PIN_DIN_R;
const int PIN_DIN_C = 7;
const int *PIN_DIN_C_ptr = &PIN_DIN_C;
const int ALL_0_512 = -1;
const int ALL_1_512 = 999;
const int ALL_1_rc = 998;

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

      
      if (row > col) { // row is greater than column, r10c2, for example, shift 8 rows and then input col and shift 2 times
        // so, rxcy, shift row - 1 - col times, 
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_R, NULL);
        delayMicroseconds(50);
        shiftPulses(row - 1 - col); // already did one
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_C, NULL);
        delayMicroseconds(50);
        shiftPulses(col - 1); // already did one
        printAddress(row, col);

        
      }
      else if (row < col) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_C, NULL);
        delayMicroseconds(50);
        shiftPulses(col - 1 - row); // already did one
        loadFirstOne(&PIN_DIN_R, NULL);
        delayMicroseconds(50);
        shiftPulses(row - 1);
        printAddress(row, col);

      }
      else if (row == col) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_R, &PIN_DIN_C);
        delayMicroseconds(50);
        // loadFirstOne(PIN_DIN_C);
        // delayMicroseconds(50);
        shiftPulses(row - 1); // already did one
        printAddress(row, col);
      }
     
    } else if (message.toInt() != 0 || message == "0") {

      pulseCount = message.toInt();

      if (pulseCount == ALL_0_512) {
        resetShiftRegister();
        Serial.println("Only reset 512.");

      } else if (pulseCount == ALL_1_512) {
        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_R, NULL);
        delayMicroseconds(50);

        for (int i = 0; i < 512; i++) {
          loadFirstOne(&PIN_DIN_R, NULL); // Push 512 '1's
        }
        Serial.println("All 1!.");


      } else if (pulseCount == ALL_1_rc) {
        resetShiftRegister();
        // delayMicroseconds(50);
        // loadFirstOne(&PIN_DIN_R, NULL);
        // delayMicroseconds(50);

        for (int i = 0; i < 1000; i++) {
          loadFirstOne(&PIN_DIN_R, &PIN_DIN_C);
          // delayMicroseconds(50);
        }
        Serial.println("All 1 rc!.");
      } else if (pulseCount > 0 && pulseCount < 513) {
        Serial.print("Pulse count received: ");
        Serial.println(pulseCount);

        resetShiftRegister();
        delayMicroseconds(50);
        loadFirstOne(&PIN_DIN_R, NULL);
        delayMicroseconds(50);
        shiftPulses(pulseCount - 1);
        Serial.println("pulse count is: ");
        Serial.println(pulseCount);

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
void printAddress(int row, int col) {
    Serial.println("address is: ");
    Serial.print("Row: ");
    Serial.print(row);
    Serial.print(", Column: ");
    Serial.println(col);


}

void resetShiftRegister() {
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(20);
  clkPulse();
  delayMicroseconds(20);
  digitalWrite(PIN_RESET, LOW);
}

void loadFirstOne(int *pin1, int *pin2) {
  if (pin2 == NULL) {
    digitalWrite(*pin1, HIGH);
    delayMicroseconds(10);
    clkPulse();
    digitalWrite(*pin1, LOW);
    delayMicroseconds(40);
  } 
  else {
    digitalWrite(*pin1, HIGH);
    digitalWrite(*pin2, HIGH);
    delayMicroseconds(10);
    clkPulse();
    digitalWrite(*pin1, LOW);
    digitalWrite(*pin2, LOW);
    delayMicroseconds(40);
  
  }
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

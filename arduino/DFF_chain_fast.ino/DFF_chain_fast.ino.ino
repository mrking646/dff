const int PIN_CLK = 4;
const int PIN_RESET = 5;
const int PIN_DIN = 6;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DIN, OUTPUT);

  digitalWrite(PIN_CLK, LOW);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_DIN, LOW);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    int pulseCount = input.toInt();  // Convert to integer (0 if invalid)

    if (pulseCount > 0) {
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
}


void resetShiftRegister() {
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(20); // Allow RESET to stabilize
  
  clkPulse(); // CLK pulse during RESET
  
  delayMicroseconds(20);
  digitalWrite(PIN_RESET, LOW); // End RESET
}

void loadFirstOne() {
  digitalWrite(PIN_DIN, HIGH); // Set DIN HIGH to load '1'
  delayMicroseconds(10); // Ensure DIN is stable before CLK

  clkPulse(); // One CLK to capture the '1' at first flip-flop

  digitalWrite(PIN_DIN, LOW); // Immediately set DIN LOW
}

void shiftPulses(int count) {
  for (int i = 0; i < count; i++) {
    clkPulse(); // Only clocking, DIN stays LOW
  }
}

void clkPulse() {
  digitalWrite(PIN_CLK, HIGH);
  delayMicroseconds(10); // Short high pulse
  digitalWrite(PIN_CLK, LOW);
  delayMicroseconds(40); // Low period between clocks
}

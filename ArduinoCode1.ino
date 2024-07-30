#include <SoftwareSerial.h>

#define capPin A2
#define buz 6
#define pulsePin A1
#define trigPin 3
#define echoPin 2

SoftwareSerial nod(0, 1);

// Thresholds for metal detection
const int thresholdHigh = 16800; // Set a higher threshold for turning off the buzzer
const int thresholdLow = 16600;  // Set a lower threshold for activating the buzzer

bool buzzerState = false; // To track the buzzer state

void setup() {
  Serial.begin(9600);
  nod.begin(9600); // Initiate SoftwareSerial for NodeMCU
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  pinMode(capPin, INPUT);
  pinMode(buz, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  ultrasonicSensorLogic(); // Integrate ultrasonic sensor logic

  int minval = 1023;
  int maxval = 0;
  long unsigned int sum = 0;

  // Read capacitance values multiple times to calculate the sum
  for (int i = 0; i < 256; i++) {
    // reset the capacitor
    pinMode(capPin, OUTPUT);
    digitalWrite(capPin, LOW);
    delayMicroseconds(20);
    pinMode(capPin, INPUT);
    // Apply pulses to the capacitor
    applyPulses();

    // read the charge of capacitor
    int val = analogRead(capPin);
    minval = min(val, minval);
    maxval = max(val, maxval);
    sum += val;
    Serial.println(sum);
  }

  // subtract minimum and maximum value to remove spikes
  sum -= minval;
  sum -= maxval;

  // Activate buzzer if metal detected (below low threshold)
  if (sum < thresholdLow && !buzzerState) {
    digitalWrite(buz, HIGH); // Activate the buzzer
    Serial.print('1');
    Serial.write('0');
    nod.print("Alert! Metal detected."); // Send alert to NodeMCU
    buzzerState = true;
  }
  // Turn off buzzer if above high threshold
  else if (sum > thresholdHigh && buzzerState) {
    digitalWrite(buz, LOW); // Turn off the buzzer
    buzzerState = false;
  }

  delay(500); // Adjust delay as needed
}

void applyPulses() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(pulsePin, HIGH); // take 3.5 uS
    delayMicroseconds(3);
    digitalWrite(pulsePin, LOW); // take 3.5 uS
    delayMicroseconds(3);
  }
}

void ultrasonicSensorLogic() {
  // Ultrasonic sensor logic
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance <= 50) {
    Serial.print(1);
    nod.print("Distance: ");
    nod.print(distance);
    tone(buz, 50); // Lower frequency (50Hz) for a potentially louder sound
    delay(100);    // Keep the buzzer active for 100 milliseconds
    noTone(buz);   // Turn off the buzzer after the delay
  } else {
    noTone(buz); // Ensure the buzzer is off when distance > 20cm
  }
}

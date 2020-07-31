#define PIN_ZEROING 4
#define PIN_SLEEP 3
#define PIN_DIR 5
#define PIN_STEP 6


// PARAMETERS
const double threadsPerRevolution = 2;
const double threadPitch = 1.0;
const double stepsPerRevolution = 200;
const double desiredDistance = 10;
const long frequency = 300; // Hz
const long maxLength = 40;


// DERIVED CONSTANTS
const double distancePerRevolution = threadPitch * threadsPerRevolution;
const double distancePerStep = distancePerRevolution / stepsPerRevolution;
const long halfStepMicroSeconds = 1000000 / frequency / 2;
const long maxPosition = maxLength / distancePerStep;

// STATE
long position = maxPosition;
bool forward = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_SLEEP, OUTPUT);
  pinMode(PIN_ZEROING, INPUT_PULLUP);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  digitalWrite(PIN_SLEEP, LOW);
  digitalWrite(PIN_STEP, LOW);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Zeroing...");

  zeroPosition();
  setForward();
  setSleep(false);
  for (long i = 0; i < maxPosition; i++) {
    stepOnce();
  }
  setSleep(true);

  Serial.println("Zeroing done!");
}

void loop() {
  const String input = Serial.readString();
  const char function = input.charAt(0);
  switch (function) {
    case 'f': setForward(); break;
    case 'b': setBackwards(); break;
    default: return;
  }
  const int distance = input.substring(1).toInt();
  const long steps = distance / distancePerStep;
  Serial.println(steps);
  setSleep(false);
  for (long i = 0; i < steps; i++) {
    stepOnce();
  }
  setSleep(true);
}

void zeroPosition() {
  position = maxPosition;
  setBackwards();
  setSleep(false);
  while(digitalRead(PIN_ZEROING)) {
    stepOnce();
  }
  setSleep(true);
  position = 0;
}

void stepOnce() {
  if (forward) {
    if (position >= maxPosition) {
      return;
    }
    position++;
  }
  else {
    if (position <= 0) {
      return;
    }
    position--;
  }
  digitalWrite(PIN_STEP, HIGH);
  delayMicroseconds(halfStepMicroSeconds);
  digitalWrite(PIN_STEP, LOW);
  delayMicroseconds(halfStepMicroSeconds);
}

void setForward() {
  forward = true;
  digitalWrite(PIN_DIR, LOW);
}

void setBackwards() {
  forward = false;
  digitalWrite(PIN_DIR, HIGH);
}

void setSleep(bool sleep) {
  digitalWrite(PIN_SLEEP, sleep ? LOW : HIGH);
}


#define toggleUp 22
#define toggleDown 23

int switchStateUp = 0;
int switchStateDown = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(toggleUp,INPUT);
  pinMode(toggleDown,INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  switchStateUp = digitalRead(toggleUp);
  switchStateDown = digitalRead(toggleDown);
  
  if (switchStateUp == HIGH && switchStateDown == HIGH) {
    Serial.println("ERROR: Detected paradoxical button states");
  }
  else if (switchStateUp == HIGH) {
    Serial.println("UP")
  }
  else if (switchStateDown == HIGH) {
    Serial.println("DOWN")
  }
  else {
    SErial.println("NEUTRAL");
  }


}

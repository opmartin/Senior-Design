/**
 * Control the motor using a toggle switch
*/


#define pwm 3
#define dir 2
#define pot A0

#define toggleUp 22
#define toggleDown 23

#define motorSpeed 125
#define stallSpeed 0

int switchStateUp = 0;
int switchStateDown = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(toggleUp,INPUT);
  pinMode(toggleDown,INPUT);
  pinMode(pwm,OUTPUT);
  pinMode(dir,OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  switchStateUp = digitalRead(toggleUp);
  switchStateDown = digitalRead(toggleDown);
  
  if (switchStateUp == HIGH && switchStateDown == HIGH) {
    Serial.println("ERROR: Detected paradoxical button states");
  }
  else if (switchStateUp == HIGH) {
    //Serial.println("UP");
    digitalWrite(dir,HIGH);
    analogWrite(pwm,motorSpeed);
  }
  else if (switchStateDown == HIGH) {
    //Serial.println("DOWN");
    digitalWrite(dir,LOW);
    analogWrite(pwm,motorSpeed);
  }
  else {
    //Serial.println("NEUTRAL");
    digitalWrite(dir,HIGH);
    analogWrite(pwm,stallSpeed);
  }

  Serial.println(analogRead(A0));

}

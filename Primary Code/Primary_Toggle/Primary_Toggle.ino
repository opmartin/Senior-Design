/**
 * Controls the motor using a toggle switch.
*/


//Constant Definitions
#define highSpeed 255
#define lowSpeed 255
#define tolerance 50
#define increment 200

//Motor Definitions
#define pwm 3
#define dir 2
#define up HIGH
#define down LOW

//Sensor Definitions
#define pot A1
#define limitHigh 10 //TBD
#define limitLow 11 //TBD

//Control Definitions
#define toggleUp 22
#define toggleDown 23

enum ModeE {
  incrementM,
  smoothM
} mode;

int motorSpeed, currentDirection;
int midPoint, currentPos, idealPos;

void setup() {
  Serial.begin(9600);
  pinMode(pwm,OUTPUT);
  pinMode(dir,OUTPUT);
  pinMode(limitHigh, INPUT);
  pinMode(limitLow, INPUT);
  currentPos = readPot();
  idealPos = currentPos;
  motorSpeed = lowSpeed;
  currentDirection = up;
  mode = smoothM;
}

void loop() {
  currentPos = readPot();

  if( mode == smoothM) {
    int toggleStatus = readToggle();
    int limit = checkLimitSwitch();
    if(limit == 0) {
      digitalWrite(dir, currentDirection);
      analogWrite(pwm, motorSpeed);
    }
  }
  if( mode == incrementM) {
    int preDirection = currentDirection;
    int toggleStatus = readToggle();
    if (preDirection != currentDirection)
      idealPos = idealPos + toggleStatus*increment; //NOTE: this won't work because it adds every frame
    adjustMotorInc();
  }

}

/**
 * Checks the toggle switch and adjusted currentDirection.
 * Returns: 0 if the toggle switch is not pressed.
 *          -1 if the toggle switch is pressed down.
 *          1 if the toggle switch is pressed up.
 */
int readToggle() {
  if(digitalRead(toggleUp == HIGH)) {
    currentDirection = up;
    return 1;
  }
  if(digitalRead(toggleDown == HIGH)) {
    currentDirection = down;
    return -1;
  }
  currentDirection = 0;
  return 0;
}

/**
 * Returns the current potentiometer position
 * Returns: between 0-1023
 */
int readPot(){
  int sensorValue = analogRead(pot); 

  return sensorValue;
}

/**
 * Returns the difference between the current position and a midpoint
 * Returns: between 0-1023
 */
int readPotBi(){
  int sensorValue = analogRead(pot); //Returns between 0 and 1023

  return midPoint - sensorValue;
}

/**
 * Powers motor if the current positions is too far from the ideal postition.
 * Also checks limit switches. If the limit switches are triggered and the
 * motor attempts to keep moving, it will be stopped and an error code will
 * be returned.
 * Returns: 0 if the motor did not need to move
 *          1 if the motor has been powered
 *          <0 if the limit switches are stopping movement 
 *             (check checkLimitSwitch for error codes)
 */
int adjustMotorInc(){
  if (abs(currentPos - idealPos) < tolerance) {
    analogWrite(pwm, 0);
    return 0;
  }

  if (currentPos > idealPos)
    currentDirection = down;
  else
    currentDirection = up;


  int limit = checkLimitSwitch();

  if(limit != 0)
    return limit;

  digitalWrite(dir, currentDirection);
  analogWrite(pwm, motorSpeed);

  return 1;
}

/**
 * Checks to see if limit switches are triggered. If they are, and the
 * current direction would go against the limit switch, the motor is
 * cut and an error number is returned.
 * Returns: 0 if limit switches are not triggered/contradicting direction
 *          -1 if motor is against the lower limit switch
 *          -2 if motor is against the upper limit switch
 */
int checkLimitSwitch() {
  if (digitalRead(limitHigh) == HIGH && currentDirection == up){
    analogWrite(pwm, 0);
    Serial.println("ERROR: Attempting to damage upper limit switch");
    idealPos == currentPos;
    return -2;
  }
  if(digitalRead(limitLow) == HIGH && currentDirection == down){
    analogWrite(pwm, 0);
    Serial.println("ERROR: Attempting to damage lower limit switch");
    idealPos == currentPos;
    return -1;
  }
  return 0;
}


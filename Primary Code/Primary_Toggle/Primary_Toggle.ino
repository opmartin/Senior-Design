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

int (*buttonFunctions[4]) ();

int motorSpeed, currentDirection;
int midPoint, currentPos, idealPos;

void setup() {
  Serial.begin(9600); //Opens Serial
  pinMode(pwm,OUTPUT); //Sets motor pwm to output
  pinMode(dir,OUTPUT); //sets direction pin to output
  pinMode(limitHigh, INPUT); //Sets high limit switch pin
  pinMode(limitLow, INPUT); //Sets low limit switch pin
  currentPos = readPot(); //Sets current position
  idealPos = currentPos; //Sets ideal position to current position
  motorSpeed = lowSpeed; //Default is low speed
  currentDirection = 0; //Current direction is 0
  mode = smoothM; //Default mode is smooth

  setButtons();
}

void loop() {
  currentPos = readPot(); //Updates current position
  printParameters();
  if( mode == smoothM) {
    int toggleStatus = readToggle(); //Checks toggle switch
    if(toggleStatus == 0) {
      return;
    }
    int limit = checkLimitSwitch(); //Checks if limit switch is contradicting
    if(limit == 0) {
      digitalWrite(dir, currentDirection); //Updates direction
      analogWrite(pwm, motorSpeed); //Sets motor speed
    }
  }
  if( mode == incrementM) {
    int preDirection = currentDirection; //Sets original direction (to detect switch change)
    int toggleStatus = readToggle(); //Reads toggle (automatically updates direction)
    if (preDirection != currentDirection) //checks if switch statement changed
      idealPos = idealPos + toggleStatus*increment; //Updates idealPos using a default increment
    adjustMotorInc(); //Adjusts motor speed accordingly
  }

}

/**
 * Checks the toggle switch and adjusted currentDirection.
 * Returns: 0 if the toggle switch is not pressed.
 *          -1 if the toggle switch is pressed down.
 *          1 if the toggle switch is pressed up.
 */
int readToggle() {
  if(digitalRead(toggleUp) == HIGH){
    currentDirection = up;
    return 1;
  }
  if(digitalRead(toggleDown) == HIGH)) {
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

void printParameters() {
  Serial.print("currentPos: ");
  Serial.print(currentPos);
  Serial.print("|| idealPos: ");
  Serial.print(idealPos);
  Serial.print("|| currentDirection: ");
  Serial.println(currentDirection);
  
}

int setButtons () {
  buttonFunctions[0] = buttonOne;
  buttonFunctions[1] = buttonTwo;
  buttonFunctions[2] = buttonThree;
  buttonFunctions[3] = buttonFour;

  return 1;
}

int buttonOne () {
  return 1;
}

int buttonTwo () {
  return 2;
}

int buttonThree () {
  return 3;
}

int buttonFour () {
  return 4;
}


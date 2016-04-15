/**
   Controls the motor using a toggle switch and push buttons.
*/

//Constant Definitions
#define highSpeed 255
#define lowSpeed 127
#define tolerance 20
#define startingIncrement 200
#define incrementUnit 20

//Motor Definitions
#define pwm 3
#define dir 2
#define up HIGH
#define down LOW

//Sensor Definitions
#define pot A0
#define limitHigh 10 //TBD
#define limitLow 11 //TBD

//Push Button Definitions
#define Bu1 18
#define Bu2 19
#define Bu3 20
#define Bu4 21
#define modeNotice 26

//Control Definitions
#define toggleUp 22
#define toggleDown 23

//Display Definitions
#define Display1A 53
#define Display1B 52
#define Display1C 51
#define Display1D 50
#define Display2A 49
#define Display2B 48
#define Display2C 47
#define Display2D 46
#define Display3A 45
#define Display3B 44
#define Display3C 43
#define Display3D 42
#define Display4A 41
#define Display4B 40
#define Display4C 39
#define Display4D 38
#define LED1 33 //TBD
#define LED2 32 //TBD
#define LED3 31 //TBD
#define LED4 30 //TBD
#define LED5 29 //TBD
#define LED6 28 //TBD
#define LED7 27 //TBD
#define LED8 26 //TBD
#define LED9 25 //TBD
#define LED10 24 //TBD
#define LED11 23 //TBD
#define LED12 22 //TBD
#define LED13 21 //TBD
#define LED14 20 //TBD
#define LED15 19 //TBD
#define LED16 18 //TBD

//Display ints
int Digit1, Digit2, Digit3, Digit4;
int displaynum;

enum ModeE {
  incrementM,
  smoothM
} mode;

int (*buttonFunctions[4]) ();

int motorSpeed, currentDirection;
int midPoint, currentPos, idealPos, minPos, maxPos;
int buttonBuffer[4], lastToggle;

int increment;

int previousButtonStates[4];

void setup() {
  Serial.begin(9600); //Opens Serial
  pinMode(pwm, OUTPUT); //Sets motor pwm to output
  pinMode(dir, OUTPUT); //sets direction pin to output
  pinMode(modeNotice, OUTPUT); //sets direction pin to output
  pinMode(limitHigh, INPUT); //Sets high limit switch pin
  pinMode(limitLow, INPUT); //Sets low limit switch pin
  currentPos = readPot(); //Sets current position
  idealPos = currentPos; //Sets ideal position to current position
  motorSpeed = lowSpeed; //Default is low speed
  currentDirection = -1; //Current direction is -1
  mode = smoothM; //Default mode is smooth
  increment = startingIncrement;
  lastToggle = readToggleInc();

  minPos = 100;
  maxPos = 800;

  buttonBuffer[0] = 0;
  buttonBuffer[1] = 0;
  buttonBuffer[2] = 0;
  buttonBuffer[3] = 0;

  pinMode(Bu1, INPUT);
  pinMode(Bu2, INPUT);
  pinMode(Bu3, INPUT);
  pinMode(Bu4, INPUT);

  displaynum = 0;

  pinMode(Display1A, OUTPUT);
  pinMode(Display1B, OUTPUT);
  pinMode(Display1C, OUTPUT);
  pinMode(Display1D, OUTPUT);
  pinMode(Display2A, OUTPUT);
  pinMode(Display2B, OUTPUT);
  pinMode(Display2C, OUTPUT);
  pinMode(Display2D, OUTPUT);
  pinMode(Display3A, OUTPUT);
  pinMode(Display3B, OUTPUT);
  pinMode(Display3C, OUTPUT);
  pinMode(Display3D, OUTPUT);
  pinMode(Display4A, OUTPUT);
  pinMode(Display4B, OUTPUT);
  pinMode(Display4C, OUTPUT);
  pinMode(Display4D, OUTPUT);

  setDisplay();
  setButtons();

  //determineLimits();
}

void determineLimits(){
   calstart=millis();
  digitalWrite(dir, down);
  analogWrite(pwm, motorSpeed);

  while(checkLimitSwitch() == 0) {
    continue;
  }
  delay(20);
  minPos = readPot();

  
  digitalWrite(dir, up);
  analogWrite(pwm, motorSpeed);

  while(checkLimitSwitch() == 0) {
    continue;
  }
  delay(20);
  maxPos = readPot();
  calfinish=millis();
  caltime=(calfinish-calstart)/1000;
  Serial.print(caltime)
  Serial.pringln(" seconds taken to calibrate")
}

void loop() {
  currentPos = readPot(); //Updates current position
  printParameters();

  checkButtons();
  setDisplay();

  if ( mode == smoothM) {
    int toggleStatus = readToggle(); //Checks toggle switch
    if (toggleStatus == 0) {
      return;
    }
    int limit = checkLimitSwitch(); //Checks if limit switch is contradicting
    if (limit == 0) {
      digitalWrite(dir, currentDirection); //Updates direction
      analogWrite(pwm, motorSpeed); //Sets motor speed
    }
  }
  if ( mode == incrementM) {
    int toggleStatus = readToggleInc(); //Reads toggle (automatically updates direction)
    if (toggleStatus != lastToggle) //checks if switch statement changed
      idealPos = idealPos + toggleStatus * increment; //Updates idealPos using a default increment
    adjustMotorInc(); //Adjusts motor speed accordingly
    lastToggle = toggleStatus;
  }

}

/**
   Checks the toggle switch and adjusted currentDirection.
   Returns: 0 if the toggle switch is not pressed.
            -1 if the toggle switch is pressed down.
            1 if the toggle switch is pressed up.
*/
int readToggle() {
  if (digitalRead(toggleUp) == HIGH) {
    currentDirection = up;
    return 1;
  }
  if (digitalRead(toggleDown) == HIGH) {
    currentDirection = down;
    return -1;
  }
  currentDirection = -1;
  return 0;
}

/**
   Checks the toggle switch and adjusted currentDirection.
   Returns: 0 if the toggle switch is not pressed.
            -1 if the toggle switch is pressed down.
            1 if the toggle switch is pressed up.
*/
int readToggleInc() {
  if (digitalRead(toggleUp) == HIGH) {
    return 1;
  }
  if (digitalRead(toggleDown) == HIGH) {
    return -1;
  }
  return 0;
}

/**
   Returns the current potentiometer position
   Returns: between 0-1023
*/
int readPot() {
  int sensorValue = analogRead(pot);

  return sensorValue;
}

/**
   Returns the difference between the current position and a midpoint
   Returns: between 0-1023
*/
int readPotBi() {
  int sensorValue = analogRead(pot); //Returns between 0 and 1023

  return midPoint - sensorValue;
}

/**
   Powers motor if the current positions is too far from the ideal postition.
   Also checks limit switches. If the limit switches are triggered and the
   motor attempts to keep moving, it will be stopped and an error code will
   be returned.
   Returns: 0 if the motor did not need to move
            1 if the motor has been powered
            <0 if the limit switches are stopping movement
               (check checkLimitSwitch for error codes)
*/
int adjustMotorInc() {
  if (idealPos > maxPos)
    idealPos = maxPos;
  if (idealPos < minPos)
    idealPos = minPos;
  
  if (abs(currentPos - idealPos) < tolerance) {
    analogWrite(pwm, 0);
    return 0;
  }

  if (currentPos > idealPos)
    currentDirection = down;
  else
    currentDirection = up;


  int limit = checkLimitSwitch();

  if (limit != 0)
    return limit;

  digitalWrite(dir, currentDirection);
  analogWrite(pwm, motorSpeed);

  return 1;
}

/**
   Checks to see if limit switches are triggered. If they are, and the
   current direction would go against the limit switch, the motor is
   cut and an error number is returned.
   Returns: 0 if limit switches are not triggered/contradicting direction
            -1 if motor is against the lower limit switch
            -2 if motor is against the upper limit switch
*/
int checkLimitSwitch() {
  return 0;
  
  if (digitalRead(limitHigh) == HIGH && currentDirection == up) {
    analogWrite(pwm, 0);
    Serial.println("ERROR: Attempting to damage upper limit switch");
    idealPos == currentPos;
    return -2;
  }
  if (digitalRead(limitLow) == HIGH && currentDirection == down) {
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
  previousButtonStates[0] = digitalRead(Bu1);
  previousButtonStates[1] = digitalRead(Bu2);
  previousButtonStates[2] = digitalRead(Bu3);
  previousButtonStates[3] = digitalRead(Bu4);

  //buttonFunctions[0] = buttonOne;
  buttonFunctions[1] = buttonTwo;
  buttonFunctions[2] = buttonThree;
  buttonFunctions[3] = buttonFour;
  
  buttonFunctions[0] = changeMode;
  buttonFunctions[1] = changeSpeed;
  buttonFunctions[2] = increaseIncrement;
  buttonFunctions[3] = decreaseIncrement;

  return 1;
}

int checkButtons () {
  int currentButtonStates[4];
  currentButtonStates[0] = digitalRead(Bu1);
  currentButtonStates[1] = digitalRead(Bu2);
  currentButtonStates[2] = digitalRead(Bu3);
  currentButtonStates[3] = digitalRead(Bu4);

  for (int i = 0; i < 4; i++) {
    
    if ((currentButtonStates[i] == HIGH) && buttonBuffer[i] < 1) {
      buttonFunctions[i]();
      //Serial.print("pressed ");
      //Serial.println(i);
      //buttonBuffer[i] = 5000;

    }
    else {
      //Serial.println("not pressed");
    }

    if (currentButtonStates[i] == HIGH) {
      buttonBuffer[i] = 3;
    }
    else 
      buttonBuffer[i] -= 1;
      
    previousButtonStates[i] = currentButtonStates[i];
  }


  return 1;
}

int buttonOne () {
  Serial.println("BUTTON ONE PRESSED");
  return 1;
}

int buttonTwo () {
  Serial.println("BUTTON TWO PRESSED");
  return 2;
}

int buttonThree () {
  Serial.println("BUTTON THREE PRESSED");
  return 3;
}

int buttonFour () {
  Serial.println("BUTTON FOUR PRESSED");
  return 4;
}

int changeMode () {
  if (mode == incrementM) {
    mode = smoothM;
    digitalWrite(modeNotice, LOW);
    Serial.println("MODE CHANGED: SMOOTH");
  }
  else if (mode == smoothM) {
    mode = incrementM;
    digitalWrite(modeNotice, HIGH);
    Serial.println("MODE CHANGED: INCREMENT");
  }
  //Serial.println("MODE CHANGED");
  return 'M';
}

int changeSpeed () {
  if (motorSpeed == highSpeed) {
    Serial.println("Speed CHANGED: LOW SPEED");
    motorSpeed = lowSpeed;
  }
  else if (motorSpeed == lowSpeed) {
    Serial.println("Speed CHANGED: HIGH SPEED");
    motorSpeed = highSpeed;
  }
  return 'S';
}

int increaseIncrement () {
  increment += incrementUnit;
  Serial.print("INCREMENT INCREASED: ");
  Serial.println(increment);
  return 'I';
}

int decreaseIncrement () {
  increment -= incrementUnit;
  if (increment < 0)
    increment = 0;
  Serial.print("INCREMENT DECREASED: ");
  Serial.println(increment);
  return 'i';
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//BEGIN DISPLAY
////////////////////////////////////////////////////////////////////////////////////////////////////

int setDisplay () {
  if (mode == incrementM) {
    digitalWrite(modeNotice, HIGH);
  }
  else if (mode == smoothM) {
    digitalWrite(modeNotice, LOW);
  }
  
  displaynum = 1 * currentPos; //Change this factor to change the displayed value to different units

  displaynum = (5* (int)round(displaynum / 5.0)); //Rounds to nearest 5

  //This section separates each digit of the number to be displayed
  if (displaynum < 10) {
    Digit1 = 0;
    Digit2 = 0;
    Digit3 = 0;
    Digit4 = displaynum;
  }

  else if (displaynum > 9 & displaynum < 100) {
    Digit1 = 0;
    Digit2 = 0;
    Digit3 = displaynum / 10;
    Digit4 = displaynum - (Digit3 * 10);
  }

  else if (displaynum > 99 & displaynum < 1000) {
    Digit1 = 0;
    Digit2 = displaynum / 100;
    Digit3 = (displaynum - (Digit2 * 100)) / 10;
    Digit4 = displaynum - (Digit2 * 100) - (Digit3 * 10);
  }

  else {
    Digit1 = displaynum / 1000;
    Digit2 = (displaynum - Digit1 * 1000) / 100;
    Digit3 = ((displaynum - Digit1 * 1000) - Digit2 * 100) / 10;
    Digit4 = (((displaynum - Digit1 * 1000) - Digit2 * 100) - Digit3 * 10);
  }


  //This section writes the binary values to the displays (writing 1 or 0 is the same as HIGH or LOW)
  int var1 = Digit1;
  digitalWrite(Display1A, HIGH && (var1 & B00001000));
  digitalWrite(Display1B, HIGH && (var1 & B00000100));
  digitalWrite(Display1C, HIGH && (var1 & B00000010));
  digitalWrite(Display1D, HIGH && (var1 & B00000001));

  //Digit2 = 4;
  int var2 = Digit2;
  digitalWrite(Display2A, HIGH && (var2 & B00001000));
  digitalWrite(Display2B, HIGH && (var2 & B00000100));
  digitalWrite(Display2C, HIGH && (var2 & B00000010));
  digitalWrite(Display2D, HIGH && (var2 & B00000001));

  //Digit3 = 7;
  int var3 = Digit3;
  digitalWrite(Display3A, HIGH && (var3 & B00001000));
  digitalWrite(Display3B, HIGH && (var3 & B00000100));
  digitalWrite(Display3C, HIGH && (var3 & B00000010));
  digitalWrite(Display3D, HIGH && (var3 & B00000001));

  //Digit4 = 3;
  int var4 = Digit4;
  digitalWrite(Display4A, HIGH && (var4 & B00001000));
  digitalWrite(Display4B, HIGH && (var4 & B00000100));
  digitalWrite(Display4C, HIGH && (var4 & B00000010));
  digitalWrite(Display4D, HIGH && (var4 & B00000001));

  //Serial.print(Digit1);
  //Serial.print(Digit2);
  //Serial.print(Digit3);
  //Serial.println(Digit4);

  setLEDs();

}

int setLEDs() {
  //This section is all for the LEDs.
  if (currentPos >= 64) {
    digitalWrite(LED1, HIGH);
  }

  else {
    digitalWrite(LED1, LOW);
  }

  if (currentPos >= 128) {
    digitalWrite(LED2, HIGH);

  }

  else {
    digitalWrite(LED2, LOW);
  }

  if (currentPos >= 192) {
    digitalWrite(LED3, HIGH);
  }

  else {
    digitalWrite(LED3, LOW);
  }

  if (currentPos >= 256) {
    digitalWrite(LED4, HIGH);
  }
  else {
    digitalWrite(LED4, LOW);
  }

  if (currentPos >= 320) {
    digitalWrite(LED5, HIGH);
  }

  else {
    digitalWrite(LED5, LOW);
  }

  if (currentPos >= 384) {
    digitalWrite(LED6, HIGH);

  }
  else {
    digitalWrite(LED6, LOW);
  }

  if (currentPos >= 448) {
    digitalWrite(LED7, HIGH);
  }
  else {
    digitalWrite(LED7, LOW);
  }

  if (currentPos >= 512) {
    digitalWrite(LED8, HIGH);
  }
  else {
    digitalWrite(LED8, LOW);
  }

  if (currentPos >= 576) {
    digitalWrite(LED9, HIGH);
  }
  else {
    digitalWrite(LED9, LOW);
  }

  if (currentPos >= 640) {
    digitalWrite(LED10, HIGH);
  }
  else {
    digitalWrite(LED10, LOW);
  }

  if (currentPos >= 704) {
    digitalWrite(LED11, HIGH);
  }
  else {
    digitalWrite(LED11, LOW);
  }

  if (currentPos >= 768) {
    digitalWrite(LED12, HIGH);
  }
  else {
    digitalWrite(LED12, LOW);
  }

  if (currentPos >= 832) {
    digitalWrite(LED13, HIGH);
  }
  else {
    digitalWrite(LED13, LOW);
  }

  if (currentPos >= 896) {
    digitalWrite(LED14, HIGH);
  }
  else {
    digitalWrite(LED14, LOW);
  }

  if (currentPos >= 960) {
    digitalWrite(LED15, HIGH);
  }
  else {
    digitalWrite(LED15, LOW);
  }

  if (currentPos >= 1023) {
    digitalWrite(LED16, HIGH);
  }
  else {
    digitalWrite(LED16, LOW);
  }
}


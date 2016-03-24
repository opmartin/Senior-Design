/**
 * Controls the motor through a potentiometer. Moves in both directions.
*/


#define pwm 3
#define dir 2
#define pot A1

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pwm,OUTPUT);
  pinMode(dir,OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (100.0 / 1023.0*2);
  
  if(voltage > 100){
    voltage = voltage - 100;
  }
  else{
    voltage = 100 - voltage;
  }
  
  // print out the value you read:
  //Serial.println(voltage);
  
 
  digitalWrite(dir,HIGH);
  int pwm_value= sensorValue * (255/1023.0*2);
  
  if(pwm_value > 255){
    digitalWrite(dir,HIGH);
    pwm_value = pwm_value - 255;
  }
  else{
    digitalWrite(dir,LOW);
    pwm_value = 255 - pwm_value;
  }
  
  analogWrite(pwm,pwm_value);          // set DIR pin HIGH or LOW
  Serial.println(pwm_value);

  /* 
  for(pwm_value=0;pwm_value<256;pwm_value++){
 
  analogWrite(pwm,pwm_value);                               //incr ease PWM in every 0.1 sec
  delay(50);
 
 
  }
  
  for(pwm_value=255;pwm_value>0;pwm_value--){
 
  analogWrite(pwm,pwm_value);                               //incr ease PWM in every 0.1 sec
  delay(50);
 
 
  }
  */

}

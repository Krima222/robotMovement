#define DIR_PIN_RIGHT 49
#define PWM_PIN_RIGHT 2

#define DIR_PIN_LEFT 51
#define PWM_PIN_LEFT 3

void setup()
{
  Serial.begin(9600);
  pinMode(DIR_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);
}

void loop()
{
  int leftSensor = analogRead(A1);
  int rightSensor = analogRead(A0);

  int cur = (leftSensor - rightSensor + 12) * 0.2;
  //Serial.print(leftSensor);
  //Serial.print(' ');
  //Serial.println(rightSensor);
  Serial.println(cur);
  rightEngineDriver(50 + cur);
  leftEngineDriver(50 - cur);
}

void rightEngineDriver(int a) {
    if (a >= 0) {
      digitalWrite(DIR_PIN_RIGHT, LOW);
      analogWrite(PWM_PIN_RIGHT, a);
    } else if (a < 0) {
      digitalWrite(DIR_PIN_RIGHT, HIGH);
      analogWrite(PWM_PIN_RIGHT, 255 + a);
    }
}

void leftEngineDriver(int a) {
    if (a >= 0) {
      digitalWrite(DIR_PIN_LEFT, LOW);
      analogWrite(PWM_PIN_LEFT, a);
    } else if (a < 0) {
      digitalWrite(DIR_PIN_LEFT, HIGH);
      analogWrite(PWM_PIN_LEFT, 255 + a);
    }
}

//echo 11
//trick 12

#define DIR_PIN_RIGHT 49
#define PWM_PIN_RIGHT 2

#define DIR_PIN_LEFT 51
#define PWM_PIN_LEFT 3
int flag = 3;
void setup()
{
    pinMode(DIR_PIN_RIGHT, OUTPUT);
    pinMode(DIR_PIN_LEFT, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
  int sensor = analogRead(A0);
  if (flag == 1) {
    rightEngineDriver(60);
    leftEngineDriver(60);
    if (sensor > 300) {
      flag = 0;
    }
  } else {
    rightEngineDriver(-60);
    leftEngineDriver(-60);
    delay(2000);
    flag = 1;
  }
  Serial.println(sensor);
  Serial.println(flag);
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

//void setup() {
//  //настройка UART для обмена данными между ардуино и компьютером
//  Serial.begin(9600);
//
//}
//
//void loop() 
//{ 
//  // считываем данные с датчика
//  int adc = analogRead(A1);
//  //передаём данные на компьютер
//  Serial.println(adc);
//  //Задержка
//  delay(500);
//  
//}
 
#define DIR_PIN_RIGHT   49 //НАПРАВЛЕНИЕ 51
#define PWM_PIN_RIGHT  2 //3

//#define EN_PIN_LEFT 4 
//#define DIR_PIN_LEFT   13//НАПРАВЛЕНИЕ
//#define PWM_PIN_LEFT  12

void setup()
{
  pinMode(DIR_PIN_RIGHT,OUTPUT);
}

//LOW -ПОЛОЖИТЕЛЬНОЕ НАПРАВЛЕНИЕ
void loop()
{
    rightM(150);
}

void rightM(int a) {
  if (a >= 0) {
    digitalWrite(DIR_PIN_RIGHT, LOW);
    analogWrite(PWM_PIN_RIGHT, a);
  } 
    if (a < 0) {
    digitalWrite(DIR_PIN_RIGHT, HIGH);
    analogWrite(PWM_PIN_RIGHT, 255 + a);
    }
}

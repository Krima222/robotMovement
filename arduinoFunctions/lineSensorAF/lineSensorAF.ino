//Левый А1
//Правый A0
void setup() {
  //Настройка UART для обмена данными между ардуино и компьютером
  Serial.begin(9600);
}

void loop() 
{ 
  //Считываем данные с датчика
  int adc = analogRead(A1);
  //Передаём данные на компьютер
  Serial.println(adc);
  //Задержка
  delay(500);
}

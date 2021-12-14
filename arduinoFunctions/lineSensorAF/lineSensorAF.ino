void setup() {
  //Настройка UART для обмена данными между ардуино и компьютером
  //левый А1
  //правый A0
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

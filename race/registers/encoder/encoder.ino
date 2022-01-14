//только правое колесо
volatile int counter_R = 0;
volatile int currentStatePD2_R;
volatile int lastStatePD2_R;

//функция вызывается при прерывании на 19 пине
ISR(INT2_vect)
{
  //записываем текущее состояние 19 пина
  currentStatePD2_R = (PIND >> PD2) & 1;
  //если на 19 ножке произошло изменение и текущее состояние = 1
  if (currentStatePD2_R != lastStatePD2_R && currentStatePD2_R == 1) {
    //и если 18 пин не совпадает с 19, то уменьшаем значение с счетчика, иначе увеличиваем
    if (((PIND >> PD3) & 1) != currentStatePD2_R) {
      counter_R --;
    } else {
      counter_R ++;
    }
  }
  //записываем текущее состояние в предыдущее
  lastStatePD2_R = currentStatePD2_R;
}

int main ()
{
  cli();
  //настраиваем 18 и 19 пины на вход
  DDRD &= ~((1 << PD3) | (1 << PD2));
  //разрешаем внешние прерывания на 19 пине
  EIMSK = (1 << INT2);
  //настраиваем прерывания на CHANGE на 19 пине
  EICRA =  (1 << ISC20);
  //даём значение переменной lastStatePD2_R с 18 пина энкодера
  lastStatePD2_R = (PIND >> PD2) & 1;
  sei();
  Serial.begin(9600);
  while (true)
  {
    //выводим данные счётсчика
    Serial.println(counter_R);
  }
}

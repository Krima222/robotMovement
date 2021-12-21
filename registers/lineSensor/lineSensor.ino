// библиотека для работы с портами ввода-вывода
#include <avr/io.h>
// библиотека тактовой частоты
#define F_CRU 16000000UL // 16 MHz
// библиотека для работы с прерываниями
#include <avr/interrupt.h>

//volatile - супер глобальная переменная
volatile int sensRight = 0;
volatile int sensLeft = 0;
volatile int analogPin = A0;
//переменная для пропускания одного считывания
volatile bool trueValue = false;

//функция, запускающаяся при АЦП, код внутри запускается при прирывании,который поменяет значение переменных
ISR(ADC_vect)
{
  if (trueValue)
  {
    if (analogPin == A0) {
      ADMUX |= (1 << MUX0);
      analogPin = A1;
      sensRight = ADCW;
    } else {
      ADMUX &= ~(1 << MUX0);
      analogPin = A0;
      sensLeft = ADCW;
    } 
    trueValue = false;
  } else {
   trueValue = true; //Первый раз пропускает считывание
  }

}

int main()
{
  //запрещает любые прерывания, в этом участке кода
  cli();

  //разрешаем АЦП, запуск преобразования, запуск неприрывных преобразований, разрешаем прерывание от АЦП
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE);

  //тактовую частоту мк делим на 128, частота ацп = 16МГц/128 = 125 KГц (16МГц - частота контроллера)
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2);

  //ОПОРНОЕ НАПРЯЖЕНИЕ - НАПРЯЖЕНИЕ ПИТАНИЯ AVCC
  ADMUX = (1 << REFS0);

  // заканчивается кусок с запретом прерываний
  sei();
  
  Serial.begin(9600);

  while (true)
  {
    //передаём данные в порт, в бесконечном цикле, для постоянного получения значений
    Serial.print(sensLeft);
    Serial.print(" ");
    Serial.println(sensRight);
  }
}

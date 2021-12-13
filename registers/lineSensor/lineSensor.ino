// библиотека для работы с портами ввода-вывода
#include <avr/io.h>
// библиотека тактовой частоты
#define F_CRU 16000000UL // 16 MHz
// библиотека для работы с прерываниями
#include <avr/interrupt.h>

volatile uint16_t adc_data = 0;

//обработчик прерывания при 
ISR(ADC_vect)
{
  adc_data = ADCW; // ADCL | (ADCH << 8)
}

int main() 
{
  // disable interrupts
  cli();
  //разрешаем АЦП, запуск преобразования, запуск неприрывных преобразований
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
  
  //разрешаем прерывание от АЦП
  ADCSRA |= 1 << ADIE;
 
  //тактовую частоту мк делим на 128, частота ацп 125 кГц
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2);
 
  //ОПОРНОЕ НАПРЯЖЕНИЕ - НАПРЯЖЕНИЕ ПИТАНИЯ AVCC
  ADMUX = (1 << REFS0) | (1 << MUX0); //левый датчи у 2 робота
  //ADMUX = (1 << REFS0); //правый датчик у 2 робота

  // enable interrupts
  sei();
  
  //НАСТРОЙКА UART ДЛЯ ОБМЕНА ДАННЫМИ С КОМПЬЮТЕРОМ
  Serial.begin(9600);

  while(true)
  {
    //ПЕРЕДАЁМ СЧИТАННЫЕ ДАННЫЕ НА КОМПЬЮТОР (показания датчика на компьютер)
    Serial.println(adc_data);
  }
}

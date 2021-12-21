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

//переменная, которая пропускает одного считывания
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
   trueValue = true;//первый раз пропускает считывание
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
  
//настроить ножки на выход на 2 и 3, регистр DDR (порт pe4, ре5)
  DDRE |= (1 << PE4)| (1 << PE5);
  DDRL |= (1 << PL0);//49 пин(правое колесо) на выход
  DDRB |= (1 << PB2);//51 пин(левое колесо) на выход

//выбираем режим работы таймера Fast PWM (TOP = ICR3) (изначально все 0, на первом запуске), на регистре TCCR3A
//ВКЛЮЧАЕМ БИТ WGM31
  TCCR3A = (1 << WGM31);
  TCCR3B = (1 << WGM32)| (1 << WGM33);
  TCCR3A |= (1 << COM3B1) | (1 << COM3C1);
//НАСТРАИВАЕМ ПРЕДДЕЛИТЕЛЯ (PRESCALING)clk/1 НАС ИНТЕРЕСУЮТ БИТЫ CS30, КАКОЙ У НЕГО РЕГИСТР
  TCCR3B |= 1 << CS30;
// ЗАПИСЫВАЕТ TOP VALUE, ТАЙМЕР СЧИТАЕТ ОТ 0 ДО TOP
  ICR3 = 3199;

//РАЗРЕШАЕМ ПРЕРЫВАНИЕ
  sei();

  while (true)
  {
//цикл, настраивающий скорость движения колёс (скорость разная, поэтому разное значение)
    int cur = (sensLeft - sensRight + 12) * 2.5;
    rightEngineDriver(500 + cur);
    leftEngineDriver(800 - cur);
  }

}

// 2 функции, которые настраевают движение колёс на езду по линии(на правое и левое колесо)
void rightEngineDriver(int a) {
  if (a >= 0) {
    OCR3B = a;  // ЗАПОЛНЕНИЕ ШИМ (duty cycle) (от 0 до top)
    PORTL &= ~(1 << PL0);
  } else {
    OCR3B = 2500 + a;  // ЗАПОЛНЕНИЕ ШИМ (duty cycle) (от 0 до top)
    PORTL |= (1 << PL0);// для того, чтобы ехал назад (49 HIGH)
  }
}

void leftEngineDriver(int a) {
  if (a >= 0) {
    OCR3C = a;  // ЗАПОЛНЕНИЕ ШИМ (duty cycle) (от 0 до top)
    PORTB &= ~(1 << PB2);
  } else {
    OCR3C = 2500 + a;  // ЗАПОЛНЕНИЕ ШИМ (duty cycle) (от 0 до top)
    PORTB |= (1 << PB2);// для того, чтобы ехал назад (49 HIGH)
  }
}

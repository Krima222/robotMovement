//библиотека для работы с портами ввода-вывода
#include <avr/io.h>
//библиотека тактовой частоты
#define F_CRU 16000000UL // 16 MHz
//библиотека для работы с прерываниями
#include <avr/interrupt.h>

//volatile - супер глобальная переменная
volatile int sensRight = 0;
volatile int sensLeft = 0;
volatile int analogPin = A0;

//переменная для пропускания одного считывания
volatile bool trueValue = false;

//функция, запускающаяся при АЦП, код внутри запускается при прерывании, который поменяет значение переменных
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
   trueValue = true; //первый раз пропускает считывание
  }
}

int main()
{
  Serial.begin(9600);
  //запрещает любые прерывания, в этом участке кода
  cli();

  //разрешаем АЦП, запуск преобразования, запуск неприрывных преобразований, разрешаем прерывание от АЦП
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE);
  //тактовую частоту мк делим на 128, частота ацп = 16МГц/128 = 125 KГц (16МГц - частота контроллера)
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0) | (1 << ADPS2);
  //опорное напряжение - напряжение питания AVCC
  ADMUX = (1 << REFS0);

  
  //настраиваем ножки 2, 3, 49 и 51 на выход, регистр DDR (порты pe4, ре5, pl0, pb2)
  DDRE |= (1 << PE4) | (1 << PE5);
  DDRL |= (1 << PL0);
  DDRB |= (1 << PB2);
  //выбираем режим работы таймера Fast PWM (TOP = ICR3) (изначально все 0, на первом запуске), на регистре TCCR3A
  //включаем бит WGM31
  TCCR3A = (1 << WGM31);
  TCCR3B = (1 << WGM32)| (1 << WGM33);
  TCCR3A |= (1 << COM3B1) | (1 << COM3C1);
  //настраиваем пределитель (prescaling)clk/1, нас интересуют биты CS30
  TCCR3B |= 1 << CS30;
  //записываем top value, таймер считает от 0 до top
  ICR3 = 3199;

  //разрешаем прерывания
  sei();

  while (true)
  {
    //цикл, настраивающий скорость движения колёс (скорость разная, поэтому разное значение)
    int cur = (sensLeft - sensRight + 12) * 2.5;
    rightEngineDriver(500 + cur);
    leftEngineDriver(800 - cur);
    Serial.print("aaaaaaaaaa");
  }
}

// 2 функции, которые настраевают движение колёс на езду по линии(на правое и левое колесо)
void rightEngineDriver(int a) {
  if (a >= 0) {
    OCR3B = a; //заполнение шим (duty cycle) (от 0 до top)
    PORTL &= ~(1 << PL0); //для того, чтобы ехал вперёд (49 LOW)
  } else {
    OCR3B = 2500 + a;
    PORTL |= (1 << PL0); //для того, чтобы ехал назад (49 HIGH)
  }
}

void leftEngineDriver(int a) {
  if (a >= 0) {
    OCR3C = a; //заполнение шим (duty cycle) (от 0 до top)
    PORTB &= ~(1 << PB2); //для того, чтобы ехал вперёд (51 LOW)
  } else {
    OCR3C = 2500 + a;
    PORTB |= (1 << PB2); //для того, чтобы ехал назад (51 HIGH)
  }
}

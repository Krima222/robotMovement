#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include<util/delay.h>

ISR(TIMER4_COMPA_vect) {
  Serial.println();
}

int main (){
  //отключаем прирывания
  cli();
  //настройка режима работы таймера
  TCCR4B = (1 << WGM42);

  //настройка сигнала на ножках OCnA/OCnB/OCnC
  //изменение сигнала на ножках, связанных с работой таймера, нет
  TCCR4A = 0;
  //настройка предделителя clk/1024
  TCCR4B |= (1 << CS42) | (1 << CS40);
  //записывает top value, таймер считает от 0 до top
  OCR4A = 15624;
  //включаем прерывание по сравнению TIMER4_COMPA
  TIMSK4 = 1 << OCIE4A;
  
  Serial.begin(9600);
  //вкл прерывание
  sei();

  while(true){
    Serial.println();
  }
}

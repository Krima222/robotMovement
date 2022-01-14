#define ECho 11  
#define Trig 12//запускмает сигнал при подаче 5 вольт



void setup() {
 
  pinMode(Trig, OUTPUT);
  pinMode(ECho, INPUT);
  Serial.begin(9600);
}

void loop() {
   int duration, distance;
   digitalWrite(Trig, HIGH);
   delayMicroseconds(10);
   digitalWrite(Trig, LOW);
   //длительность сигнала поступающего с Eсho
   duration = pulseIn(ECho, HIGH);
   //рассчитываем растояние в санти-ах
   distance = duration / 58.2;
   Serial.println(distance);
   delay(100);

}

/*каналы
//ширина окружности 21.3
//4 режим работы 
// 0 0  
//вкл прерыван 1 каннал а OCIE4A
//15 624 в регистр ocr
//
#define ENC_A 18//правое колесо
#define ENC_B 19//правое колесо

//храним данные от инкодера
volatile int32_t enc_cnt = 0;
//ф-ция для обработки данных от энкодера с канала А
void enc_A() {
  enc_cnt++;
}
//ф-ция для обработки данных от энкодера с канала А
//void enc_B() {

//}

void setup() {
//ножки на вход
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
//настраиваем внешнее прерывание(номер прирывание 5 прерыв на 18 ножке = enc_A
  attachInterrupt(5, enc_A, CHANGE);
//  attachInterrupt(4, enc_B, CHANGE);
//настройка UART для обмена данными с компом
  Serial.begin(9600);  
}

void loop() {
// шлем данные с инкодера в монитор порта  
  Serial.println(enc_cnt);
  delay(200);
} */

#define DIR_PIN_RIGHT 49
#define PWM_PIN_RIGHT 2

#define DIR_PIN_LEFT 51
#define PWM_PIN_LEFT 3

// Входы поворотного энкодера
#define CLK_R 19
#define DT_R 18
#define CLK_L 20
#define DT_L 21

int counter_R = 0;
int currentStateCLK_R;
int lastStateCLK_R;

int counter_L = 0;
int currentStateCLK_L;
int lastStateCLK_L;

void setup() {
  pinMode(DIR_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);


  // Устанавливаем контакты энкодера как входы
  pinMode(CLK_R,INPUT);
  pinMode(DT_R,INPUT);

  pinMode(CLK_L,INPUT);
  pinMode(DT_L,INPUT);
  // Настройка последовательного монитора
  Serial.begin(9600);
  // Считываем начальное состояние CLK
  lastStateCLK_R = digitalRead(CLK_R);

  lastStateCLK_L = digitalRead(CLK_L);
  
  // Вызов updateEncoder () при обнаружении любого изменения максимума/минимума при прерывании 0 (вывод 2) или прерывании 1 (вывод 3)
  attachInterrupt(5, updateEncoder_R, CHANGE);//18
  attachInterrupt(4, updateEncoder_R, CHANGE);//19

  attachInterrupt(2, updateEncoder_L, CHANGE);//21
  attachInterrupt(3, updateEncoder_L, CHANGE);//20
}
void loop() {
  //Здесь какой-либо полезный код
    Serial.print(counter_L);
    Serial.print("  ");
    Serial.println(counter_R);
    if (counter_R < 350) {
      rightEngineDriver(100);
    } else {
      rightEngineDriver(0);
    }
    
    if (counter_L < 350) {
      leftEngineDriver(100);
    } else {
      leftEngineDriver(0);
    }
}
void updateEncoder_R(){
  // Считываем текущее состояние CLK
  currentStateCLK_R = digitalRead(CLK_R);
  // Если последнее и текущее состояние CLK различаются, то произошел импульс.
  // Реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_R != lastStateCLK_R  && currentStateCLK_R == 1){
    // Если состояние DT отличается от состояния CLK, то
    // энкодер вращается против часовой стрелки, поэтому уменьшаем
    if (digitalRead(DT_R) != currentStateCLK_R) {
      counter_R --;
    } else {
      // Энкодер вращается по часовой стрелке, поэтому увеличиваем
      counter_R ++;
    }

  }
  // Запоминаем последнее состояние CLK
  lastStateCLK_R = currentStateCLK_R;
}

void updateEncoder_L(){
  // Считываем текущее состояние CLK
  currentStateCLK_L = digitalRead(CLK_L);
  // Если последнее и текущее состояние CLK различаются, то произошел импульс.
  // Реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_L != lastStateCLK_L  && currentStateCLK_L == 1){
    // Если состояние DT отличается от состояния CLK, то
    // энкодер вращается против часовой стрелки, поэтому уменьшаем
    if (digitalRead(DT_L) != currentStateCLK_L) {
      counter_L --;
    } else {
      // Энкодер вращается по часовой стрелке, поэтому увеличиваем
      counter_L ++;
    }

  }
  // Запоминаем последнее состояние CLK
  lastStateCLK_L = currentStateCLK_L;
}































void rightEngineDriver(int a) {
    if (a >= 0) {
      digitalWrite(DIR_PIN_RIGHT, LOW);
      analogWrite(PWM_PIN_RIGHT, a);
    } else if (a < 0) {
      digitalWrite(DIR_PIN_RIGHT, HIGH);
      analogWrite(PWM_PIN_RIGHT, 255 + a);
    }
}

void leftEngineDriver(int a) {
    if (a >= 0) {
      digitalWrite(DIR_PIN_LEFT, LOW);
      analogWrite(PWM_PIN_LEFT, a);
    } else if (a < 0) {
      digitalWrite(DIR_PIN_LEFT, HIGH);
      analogWrite(PWM_PIN_LEFT, 255 + a);
    }
}

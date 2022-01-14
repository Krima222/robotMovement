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

  // Устанавливаем контакты энкодера как входы
  pinMode(CLK_R, INPUT);
  pinMode(DT_R, INPUT);

  pinMode(CLK_L, INPUT);
  pinMode(DT_L, INPUT);
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
  //Вывод значений
  Serial.print(counter_L);
  Serial.print("  ");
  Serial.println(counter_R);
}
void updateEncoder_R() {
  // Считываем текущее состояние CLK_R
  currentStateCLK_R = digitalRead(CLK_R);
  // Если последнее и текущее состояние CLK_R различаются, то произошел импульс.
  // Реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_R != lastStateCLK_R  && currentStateCLK_R == 1) {
    // Если состояние DT_R отличается от состояния CLK_R, то
    // энкодер вращается против часовой стрелки, поэтому уменьшаем
    if (digitalRead(DT_R) != currentStateCLK_R) {
      counter_R --;
    } else {
      // Энкодер вращается по часовой стрелке, поэтому увеличиваем
      counter_R ++;
    }

  }
  // Запоминаем последнее состояние CLK_L
  lastStateCLK_R = currentStateCLK_R;
}

void updateEncoder_L() {
  // Считываем текущее состояние CLK
  currentStateCLK_L = digitalRead(CLK_L);
  // Если последнее и текущее состояние CLK различаются, то произошел импульс.
  // Реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_L != lastStateCLK_L  && currentStateCLK_L == 1) {
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

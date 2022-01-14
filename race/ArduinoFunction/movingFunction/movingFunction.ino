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

//хранение состояния
String state;
//текущее состояние
int target_R;
//целевое состояние
int target_L;

void setup() {
  // Устанавливаем
  pinMode(DIR_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  // Устанавливаем контакты энкодера как входы
  pinMode(CLK_R, INPUT_PULLUP);
  pinMode(DT_R, INPUT_PULLUP);
  pinMode(CLK_L, INPUT_PULLUP);
  pinMode(DT_L, INPUT_PULLUP);

  // Считываем начальное состояние CLK
  lastStateCLK_R = digitalRead(CLK_R);
  lastStateCLK_L = digitalRead(CLK_L);

  // Вызов updateEncoder () при обнаружении любого изменения максимума/минимума при прерывании 0 (вывод 2) или прерывании 1 (вывод 3)
  attachInterrupt(4, updateEncoder_R, CHANGE);//19
  attachInterrupt(3, updateEncoder_L, CHANGE);//20
  
  //вызов функции движения робота на заданное растояние в см
  passingDistance(15);
}

void loop() {
  //состояние при котором робот двигается вперёд
  if (state == "forward") {
    rightEngineDriver(100);
    leftEngineDriver(106);
    if (counter_R >= target_R && counter_L >= target_L) {
      state = "stop";
    }
  }
  //состояние при котором робот двигается назад
  if (state == "back") {
    rightEngineDriver(-100);
    leftEngineDriver(-100);
    if (counter_R <= target_R && counter_L <= target_L) {
      state = "stop";
    }
  }
  if (state == "stop") {
    rightEngineDriver(0);
    leftEngineDriver(0);
  }
}

void passingDistance (int distance) {
  //настраиваем состояние в зависимости от положительного, отрицательного значения, выставляем состояние
  if (distance > 0) {
    state = "forward";
  } else if (distance < 0) {
    state = "back";
  }
  //устанавливаем целевое состояние для колёс (17 дискрет = 1 см)
  target_R = counter_R + (distance * 18.5);
  target_L = counter_L + (distance * 18.5);
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

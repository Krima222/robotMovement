//входы датчика расстояния
#define ECho 11
#define Trig 12

//входы двигателей
#define DIR_PIN_RIGHT 49
#define PWM_PIN_RIGHT 2
#define DIR_PIN_LEFT 51
#define PWM_PIN_LEFT 3

//входы энкодера
#define CLK_R 19
#define DT_R 18
#define CLK_L 20
#define DT_L 21

//переменные для энкодера
int counter_R = 0;
int currentStateCLK_R;
int lastStateCLK_R;

int counter_L = 0;
int currentStateCLK_L;
int lastStateCLK_L;

//переменная для счёта дистанции до предмета
int distance;
//переменная для задержки считывания датчика расстояния
int timerStart;

//состояние робота (движется вперёд/назад, поворачивает направо/налево, стоит)
String state;
//целевое состояние колёс
int target_R;
int target_L;
//этап объезда препядствия
int turningState;
//переменная для чередования направлений объезда, изначально справа
//int detourDirection;

//прямое расстояние которое робот прошёл до финиша
int passedDistance;

//коэффициенты усиления пропорциональной, интегрирующей и дифференцирующей составляющих регулятора
float Kp = 1.1;
float Ki = 0;
float Kd = 0;
//целевой результат
float r = 0;

//границы скорости двигателя
float min = -50;
float max = 50;

float integral;
float prevErr;

int speedLastCounter_R;
int speedLastCounter_L;
int speedTimer;
int k;
int diff;

void setup() {
  Serial.begin(9600);
  //настройка пинов для датчика расстояния
  pinMode(Trig, OUTPUT);
  pinMode(ECho, INPUT_PULLUP);

  //настройка пинов двигателей на выход
  pinMode(DIR_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  //настройка пинов энкодера на вход
  pinMode(CLK_R, INPUT_PULLUP);
  pinMode(DT_R, INPUT_PULLUP);
  pinMode(CLK_L, INPUT_PULLUP);
  pinMode(DT_L, INPUT_PULLUP);

  //считываем начальное состояние пинов 19 и 20
  lastStateCLK_R = digitalRead(CLK_R);
  lastStateCLK_L = digitalRead(CLK_L);

  //настройка прерываний на пинах 19 и 20
  attachInterrupt(4, updateEncoder_R, CHANGE); //19
  attachInterrupt(3, updateEncoder_L, CHANGE); //20

  //вызов функции движения робота на заданное растояние в см
  passingDistance(400);
}

void loop() {
  //измеряем расстояние до препятствия
  distanceSensor();

  //состояние при котором робот двигается вперёд
  if (state == "forward") {
    rightEngineDriver(100 + k);
    leftEngineDriver(100 - k); //108
    //если текущее состояние больше, целевого, то выполняется состояние остановки
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
  //состояние при котором осуществляется поворот налево
  if (state == "turnLeft") {
    rightEngineDriver(100);
    leftEngineDriver(-100);
    if (counter_R >= target_R && counter_L <= target_L) {
      state = "stop";
    }
  }
  //состояние при котором осуществляется поворот направо
  if (state == "turnRight") {
    rightEngineDriver(-100);
    leftEngineDriver(100);
    if (counter_R <= target_R && counter_L >= target_L) {
      state = "stop";
    }
  }
  //состояние остановки
  if (state == "stop") {
    rightEngineDriver(0);
    leftEngineDriver(0);
  }

  //объезд препятствий
  if (distance <= 40 && distance >= 25 && turningState == 0 && state != "stop") {
    state = "stop";
    turningState ++;
  }
  if (turningState == 1 && state == "stop") {
    turn(30);
    turningState ++;
  }
  if (turningState == 2 && state == "stop") {
    passingDistance(60);
    turningState ++;
  }
  if (turningState == 3 && state == "stop") {
    turn(-60);
    turningState ++;
    //passedDistance += 972;
  }
  if (turningState == 4 && state == "stop") {
    passingDistance(70);
    turningState ++;
  }
  if (turningState == 5 && state == "stop") {
    turn(30);
    turningState ++;
  }
  if (turningState == 6 && state == "stop") {
    passingDistance(400);
    turningState = 0;
    //passedDistance += 972;
  }

  //  if (state == "stop") { потенциально более оптимизированный код (пока, не стоит проверяет всего 1 условие, а не 6)
  //    if (turningState == 1) {
  //      turn(30);
  //      turningState ++;
  //    }
  //    if (turningState == 2) {
  //      passingDistance(60);
  //      turningState ++;
  //    }
  //    if (turningState == 3) {
  //      turn(-60);
  //      turningState ++;
  //    }
  //    if (turningState == 4) {
  //      passingDistance(70);
  //      turningState ++;
  //    }
  //    if (turningState == 5) {
  //      turn(30);
  //      turningState ++;
  //    }
  //    if (turningState == 6) {
  //      passingDistance(400);
  //      turningState = 0;
  //    }
  //  }

  //  if (detourDirection == 0) { чередование направления объезда, изначально объезжает справа
  //    if (distance <= 40 && distance >= 25 && turningState == 0) {
  //      state = "stop";
  //      turningState ++;
  //    }
  //    if (turningState == 1 && state == "stop") {
  //      turn(30);
  //      turningState ++;
  //    }
  //    if (turningState == 2 && state == "stop") {
  //      passingDistance(60);
  //      turningState ++;
  //    }
  //    if (turningState == 3 && state == "stop") {
  //      turn(-60);
  //      turningState ++;
  //    }
  //    if (turningState == 4 && state == "stop") {
  //      passingDistance(70);
  //      turningState ++;
  //    }
  //    if (turningState == 5 && state == "stop") {
  //      turn(30);
  //      turningState ++;
  //    }
  //    if (turningState == 6 && state == "stop") {
  //      passingDistance(400);
  //      turningState = 0;
  //      detourDirection = 1;
  //    }
  //  } else {
  //    if (distance <= 40 && distance >= 25 && turningState == 0) {
  //      state = "stop";
  //      turningState ++;
  //    }
  //    if (turningState == 1 && state == "stop") {
  //      turn(-30);
  //      turningState ++;
  //    }
  //    if (turningState == 2 && state == "stop") {
  //      passingDistance(60);
  //      turningState ++;
  //    }
  //    if (turningState == 3 && state == "stop") {
  //      turn(60);
  //      turningState ++;
  //    }
  //    if (turningState == 4 && state == "stop") {
  //      passingDistance(70);
  //      turningState ++;
  //    }
  //    if (turningState == 5 && state == "stop") {
  //      turn(-30);
  //      turningState ++;
  //    }
  //    if (turningState == 6 && state == "stop") {
  //      passingDistance(400);
  //      turningState = 0;
  //      detourDirection = 0;
  //    }

  //уравнивание скорости колёс
  if (millis() - speedTimer >= 100) {
    diff = (counter_R - speedLastCounter_R) - (counter_L - speedLastCounter_L);
    speedLastCounter_R = counter_R;
    speedLastCounter_L = counter_L;
    
//    if (state == "forward") { //если едет вперёд, прибавляем проеханное количество дискрет за каждые 100мс
//      passedDistance += counter_L - speedLastCounter_L;
//    }
    
    speedTimer = millis();
  }
  k = PIDRegulator(diff , 100);

  Serial.print(distance);
  Serial.print(" ");
  Serial.println(counter_L - speedLastCounter_L);

//  //сканирование
//  if (state == "stop" && scanStatus == 0 && turningState == 0) {
//    turn(20);
//    scanStatus++;
//  }
//  if (scanStatus == 1 && state == "stop") {
//    turn(-20);
//    scanStatus++;
//  }
//  if (scanStatus == 2 && obstacleDetected == "no" && state == "stop") {
//    turn(-20);
//    scanStatus++;
//  }
//  if (scanStatus == 3 && state == "stop") {
//    turn(20);
//    scanStatus++;
//  }
//  if (scanStatus == 4 && obstacleDetected == "no" && state == "stop") { //если препятствий нет, едем дальше
//    passingDistance(25);
//  }
//
//  if ((scanStatus == 1 || scanStatus == 2) && distance <= 40 && distance >= 25 && obstacleDetected == "no") {
//    obstacleDetected = "right";
//  }
//  if ((scanStatus == 3 || scanStatus == 4) && distance <= 40 && distance >= 25 && obstacleDetected == "no") {
//    obstacleDetected = "left";
//  }
//
//  if (scanStatus == 2 && obstacleDetected == "right" && state == "stop") { //если справа было препятствие объезжаем
//    scanStatus = 0;
//    detourDirection = 1; //влево
//    turningState++;
//  }
//  if (scanStatus == 4 && obstacleDetected == "left" && state == "stop") { //если слева было препятствие объезжаем
//    scanStatus = 0;
//    detourDirection = 0; //вправо
//    turningState++;
//  }

//  if (passedDistance >= 7200) { останавливаем всё, когда доехал
//    state = "finish";
//    rightEngineDriver(0);
//    leftEngineDriver(0);
//  }
}

//int scanStatus;
//String obstacleDetected = "no";



void passingDistance (int distance) {
  //настраиваем состояние в зависимости от положительного, отрицательного значения, выставляем состояние
  if (distance > 0) {
    state = "forward";
  } else if (distance < 0) {
    state = "back"; 
  }
  //устанавливаем целевое состояние для колёс (18,5 дискрет = 1 см) состояние при котором он останавливается
  target_R = counter_R + (distance * 18.5);
  target_L = counter_L + (distance * 18.5);
}

void turn (int angle) {
  //записываем состояние робота при повороте направо/налево
  if (angle > 0) {
    state = "turnRight";
  } else {
    state = "turnLeft";
  }
  //записываем целевое состояние ((ширина колеи * п) / 360 * 18 = 4) (18 дискрет = 1 см)
  target_R = counter_R - (angle * 4.1);
  target_L = counter_L + (angle * 4.1);
}

void updateEncoder_R() {
  //считываем текущее состояние CLK_R
  currentStateCLK_R = digitalRead(CLK_R);
  //если последнее и текущее состояние CLK_R различаются, то произошел импульс.
  //реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_R != lastStateCLK_R  && currentStateCLK_R == 1) {
    //если состояние DT_R отличается от состояния CLK_R, то
    //энкодер вращается против часовой стрелки, поэтому уменьшаем
    if (digitalRead(DT_R) != currentStateCLK_R) {
      counter_R --;
    } else {
      //энкодер вращается по часовой стрелке, поэтому увеличиваем
      counter_R ++;
    }
  }
  //запоминаем последнее состояние CLK_L
  lastStateCLK_R = currentStateCLK_R;
}

void updateEncoder_L() {
  //считываем текущее состояние CLK
  currentStateCLK_L = digitalRead(CLK_L);
  //если последнее и текущее состояние CLK различаются, то произошел импульс.
  //реагируем только на одно изменение состояния, чтобы избежать двойного счета
  if (currentStateCLK_L != lastStateCLK_L  && currentStateCLK_L == 1) {
    //если состояние DT отличается от состояния CLK, то
    //энкодер вращается против часовой стрелки, поэтому уменьшаем
    if (digitalRead(DT_L) != currentStateCLK_L) {
      counter_L --;
    } else {
      //энкодер вращается по часовой стрелке, поэтому увеличиваем
      counter_L ++;
    }
  }
  //запоминаем последнее состояние CLK
  lastStateCLK_L = currentStateCLK_L;
}

//функции для настройки направления и скорости двигателей
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

//функция для измерения расстояния до припятствия
void distanceSensor () {
  if (millis() - timerStart >= 300) {
    int duration;
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);
    //измеряем длительность сигнала поступающего с Eсho
    duration = pulseIn(ECho, HIGH);
    //рассчитываем растояние в сантиметрах
    distance = duration / 58.2;
    timerStart = millis();
  }
}

float PIDRegulator(float current, float dt) {
  // ошибка
  float const error = r - current;

  // пропорциональная составляющая
  float const Pout = error * Kp;

  // интегрирующая составляющая
  integral += error * dt;
  float const Iout = integral * Ki;

  // производная составляющая
  float const deriviate = (error - prevErr) / dt;
  float const Dout = Kd * deriviate;

  float output = Pout + Iout + Dout;
  prevErr = error;

  if (output > max)
    output = max;
  else if (output < min)
    output = min;

  return output;
}

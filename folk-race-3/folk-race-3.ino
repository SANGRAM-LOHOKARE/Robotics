#define MODE '90deg'

//Body measurements
#define LENGTH 16 //pls check once
#define WIDTH 14

//Sensor positions [considering the top-centre corner to be the reference point (0,0)](btw top-centre of the boundary, not that of the car body)
#if (MODE == '90deg')
  #define IR_A_X
  #define IR_A_Y
  #define IR_B_X
  #define IR_B_Y
  #define IR_C_X  
  #define IR_C_Y
#elif (MODE == '45deg')
  //something something
#endif

//Motor directions
#define HALT 0
#define MOVE_FORWARD 1
#define MOVE_BACKWARD 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4

#define SPEED 255   //(0-255)
#define SPEED_LOW_LIMIT 70

#define MIN_DIST T2

#define THRESHOLD 7.75
#define CRITICAL_DIST 13


//Pin config
#define IR_A_PIN A6
// #define IR_B_PIN A6
#define IR_C_PIN A7

#define AIN1 2
#define AIN2 4
#define PWMA 5

#define BIN1 7
#define BIN2 8
#define PWMB 6

#define STBY 9

#define US_TRIG_A A3
#define US_ECHO_A A2

#define US_TRIG_K A4
#define US_ECHO_K A5

#define US_TRIG_B A1
#define US_ECHO_B A0



#define T1 2.1
#define T2 6  //earlier (4.6)
#define T3 11.4
#define T4 22.8



// float delta_A = 0;
// float delta_B = 0;
// float delta_C = 0;

float ir_dist_A = 0;
// float us_B = 0;
float ir_dist_B = 0;


float us_A = 0;
float us_C = 0;
float us_B = 0;


int movement = HALT;
int speed = 0;

enum ObstaclePattern
{
    OPEN,

    LEFT,
    CENTER,
    RIGHT,

    LEFT_CENTER,
    CENTER_RIGHT,
    LEFT_RIGHT,

    ALL,

    UNKNOWN
};

ObstaclePattern pattern;


void motor_setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // // Motor pair A forward
  // digitalWrite(AIN1, HIGH);
  // digitalWrite(AIN2, LOW);
  // analogWrite(PWMA, 200);

  // // Motor pair B forward
  // digitalWrite(BIN1, HIGH);
  // digitalWrite(BIN2, LOW);
  // analogWrite(PWMB, 200);
}

void us_setup() {
  pinMode(US_TRIG_A, OUTPUT);
  pinMode(US_ECHO_A, INPUT);
  pinMode(US_TRIG_B, OUTPUT);
  pinMode(US_ECHO_B, INPUT);
  pinMode(US_TRIG_K, OUTPUT);
  pinMode(US_ECHO_K, INPUT);
}

void setup() {
  motor_setup();
  us_setup();

  delay(2000); //initial delay
}


void loop() {
  updateSensors();

  print_distance();

  classifyPattern();
  printObstacleState();

  if(pattern == OPEN)
    trackFollowing();
  else
    handleObstacle();


  change_speed();

  printMovement();
  // update_direction();
  // Serial.println(movement);
  
  update_motor_state();
  delay(100);
}

void updateSensors(){
  readUltrasonic();
  readIR();
}

void readUltrasonic(){
  us_A = get_ultrasound(US_TRIG_A, US_ECHO_A);
  us_C = get_ultrasound(US_TRIG_B, US_ECHO_B);
  us_B = get_ultrasound(US_TRIG_K, US_ECHO_K);
}

void readIR() {
  float ir_raw_A = analogRead(IR_A_PIN);
  delay(10);
  // float ir_raw_B = analogRead(IR_B_PIN);
  // delay(10);
  float ir_raw_C = analogRead(IR_C_PIN);
  delay(10);


  ir_dist_A = ir_raw_to_distance(ir_raw_A);
  // us_B = ir_raw_to_distance(ir_raw_B);
  ir_dist_B = ir_raw_to_distance(ir_raw_C);
}

float ir_raw_to_distance(float ir_raw_val) {
  float voltage = (ir_raw_val * 5.0) / 1023.0;
  
  float dist = 12.08 * pow(voltage, -1.058);
  // float dist = 12.298 / (voltage - 0.12);
  
  return dist;
}

float get_ultrasound(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(10);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 30000); 
  if (duration == 0) return 400.0;               
  
  return duration * 0.01715;
}

bool obstacleLeft(){
  return us_A < THRESHOLD;
}

bool obstacleCenter(){
  return us_B < T2;
}

bool obstacleRight(){
  return us_C < THRESHOLD;
}

void classifyPattern()
{
    bool L = obstacleLeft();
    bool C = obstacleCenter();
    bool R = obstacleRight();

    if(!L && !C && !R)
        pattern = OPEN;

    else if(L && !C && !R)
        pattern = LEFT;

    else if(!L && C && !R)
        pattern = CENTER;

    else if(!L && !C && R)
        pattern = RIGHT;

    else if(L && C && !R)
        pattern = LEFT_CENTER;

    else if(!L && C && R)
        pattern = CENTER_RIGHT;

    else if(L && !C && R)
        pattern = LEFT_RIGHT;

    else if(L && C && R)
        pattern = ALL;

    else
        pattern = UNKNOWN;
}

void printObstacleState()
{
    Serial.print("L:");
    Serial.print(obstacleLeft());

    Serial.print(" C:");
    Serial.print(obstacleCenter());

    Serial.print(" R:");
    Serial.print(obstacleRight());

    Serial.print("  --> ");

    printPattern();
}

void printPattern()
{
  switch(pattern)
  {
    case OPEN:
      Serial.println("Pattern : OPEN");
      break;

    case LEFT:
      Serial.println("Pattern : LEFT");
      break;

    case CENTER:
      Serial.println("Pattern : CENTER");
      break;

    case RIGHT:
      Serial.println("Pattern : RIGHT");
      break;

    case LEFT_CENTER:
      Serial.println("Pattern : LEFT_CENTER");
      break;

    case CENTER_RIGHT:
      Serial.println("Pattern : CENTER_RIGHT");
      break;

    case LEFT_RIGHT:
      Serial.println("Pattern : LEFT_RIGHT");
      break;

    case ALL:
      Serial.println("Pattern : ALL");
      break;

    default:
      Serial.println("Pattern : UNKNOWN");
  }
}


…#define US_ECHO_L A2

  else if(us_B < 30){
    // speed = (7.2835 * (us_B - T2)) + 70;
    speed = (((us_B - T2) * (255 - SPEED_LOW_LIMIT)) / (30 - T2)) + SPEED_LOW_LIMIT;
  }
  else {
    speed = 255;
  }
}

void printMovement()
{
  Serial.print("Movement : ");

  switch(movement)
  {
    case MOVE_FORWARD:
      Serial.println("FORWARD");
      break;

    case MOVE_BACKWARD:
      Serial.println("BACKWARD");
      break;

    case MOVE_LEFT:
      Serial.println("LEFT");
      break;

    case MOVE_RIGHT:
      Serial.println("RIGHT");
      break;

    default:
      Serial.println("HALT");
      break;
  }
}


// void change_speed() {
//   if(us_B < T2) {
//     speed = 255;
//   }
//   else if(us_B < T3) {
//     speed = 120;
//   }
//   else if(us_B < T4) {
//     speed = 190;
//   }
//   else {
//     speed = 255;
//   }
// }


void trackFollowing()
{
  if(ir_dist_L)
  movement = MOVE_FORWARD;
}


void reverseBriefly()
{
  movement = MOVE_BACKWARD;
  Serial.println("Movement : BACKWARD");
  update_motor_state();
  delay(250);
}


int bestDirection()
{ 
  if(ir_dist_A > ir_dist_B)
    return MOVE_LEFT;
  else
    return MOVE_RIGHT;
}


void handleObstacle()
{
  switch(pattern)
  {
    case CENTER:
      reverseBriefly();
      updateSensors();
      movement = bestDirection();

    case LEFT:
      reverseBriefly();
      movement = MOVE_RIGHT;
      update_motor_state();
      delay(100);
      break;

    case RIGHT:
      reverseBriefly();
      movement = MOVE_LEFT;
      update_motor_state();
      delay(100);
      break;

    case LEFT_CENTER:
      reverseBriefly();
      movement = MOVE_RIGHT;
      update_motor_state();
      delay(100);
      break;

    case CENTER_RIGHT:
      reverseBriefly();
      movement = MOVE_LEFT;
      update_motor_state();
      delay(100);
      break;

    case LEFT_RIGHT:
      movement = bestDirection();
      update_motor_state();
      delay(100);
      break;

    case ALL:
      reverseBriefly();
      update_motor_state();
      delay(50);
      updateSensors();
      movement = bestDirection();
      break;

    default:
      movement = HALT;
  }
}


// int update_direction() {
//   if(us_B >= MIN_DIST) {
//     movement = MOVE_FORWARD;
//   }
//   else if(ir_dist_A < MIN_DIST && ir_dist_B >= MIN_DIST) {
//     movement = MOVE_RIGHT;
//   }
//   else if(ir_dist_B < MIN_DIST && ir_dist_A >= MIN_DIST) {
//     movement = MOVE_LEFT;
//   }
//   else {
//     movement = MOVE_BACKWARD;
//     update_motor_state();
//     delay(500);
//   }

//   return movement;
// }


int update_direction() {
  if(us_B >= MIN_DIST) {
    if(ir_dist_A >= MIN_DIST && ir_dist_B >= MIN_DIST) {
      movement = MOVE_FORWARD;
    }
    else if(ir_dist_A >= MIN_DIST && ir_dist_B < MIN_DIST) {
      movement = MOVE_LEFT;
    }
    else if(ir_dist_A < MIN_DIST && ir_dist_B >= MIN_DIST) {
      movement = MOVE_RIGHT;
    }
  }
  else {
    movement = MOVE_BACKWARD;
    update_motor_state();
    delay(300);
    if(ir_dist_A > ir_dist_B)
      movement = MOVE_LEFT;
    else
      movement = MOVE_RIGHT;
  }
}


// int update_direction() {
//   if(us_B < MIN_DIST) {
//     movement = MOVE_BACKWARD;
//     update_motor_state();
//     delay(500);
//   }
//   if(ir_dist_A < MIN_DIST && ir_dist_B >= MIN_DIST) {
//     movement = MOVE_RIGHT;
//   }
//   else if(ir_dist_B < MIN_DIST && ir_dist_A >= MIN_DIST) {
//     movement = MOVE_LEFT;
//   }
//   else {
//     movement = MOVE_FORWARD;
//   }

//   return movement;
// }


// Here due to inverted connections to motor drive ports, A is now the right pair of motors, and B is the left pair of motors
void update_motor_state() {
  switch(movement) {
    case MOVE_FORWARD : {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, speed);

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, speed);
        break;
    }
    case MOVE_BACKWARD : {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, speed);

        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, speed);
        break;
    }
    case MOVE_LEFT : {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, speed);

        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        analogWrite(PWMB, speed);
        break;
    }
    case MOVE_RIGHT : {

        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        analogWrite(PWMA, speed);

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, speed);
        break;
    }
    default : {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, LOW);
        analogWrite(PWMA, speed);

        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, LOW);
        analogWrite(PWMB, speed);
    }
  }
}

void print_distance() {
  Serial.print("L IR: ");
  Serial.print(ir_dist_A);
  Serial.print("\tL US: ");
  Serial.print(us_A);
  Serial.print("\tC US: ");
  Serial.print(us_B);
  Serial.print("\tR US: ");
  Serial.print(us_C);
  Serial.print("\tR IR: ");
  Serial.println(ir_dist_B);
}

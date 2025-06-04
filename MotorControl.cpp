

#include <SoftwareSerial.h>
//1번
#define TRIG1 13 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO1 12 //ECHO 핀 설정 (초음파 받는 핀)
//2번
#define TRIG2 11 //통신 핀으로 핀 변경
#define ECHO2 10
//3번
#define TRIG3 9 
#define ECHO3 8 
//4번
#define TRIG4 A2
#define ECHO4 A3 
//초음파 탐지 거리
#define limitdist 40

//모터 제한 핀
#define speedpin1 A5
#define speedpin2 A4
#define IN1 4    
#define IN2 5
#define IN3 6
#define IN4 7
#define maxspeed 255

/******************************************************************* */
#define forwardtime 1400 //직진 시간
#define turntime 400 //좌회전 우회전 시간
#define boosttime 1000 //회전 후 직진 시간
#define obstacletime 1200 //장애물 감지시 회피 시간
#define reversetime 2500 //역회전 시간
#define cleartime 300 // 버퍼 비우는 시간
#define loopdelaytime 200 //loop문 딜레이 시간
/******************************************************************* */

unsigned long time_previous=0;
unsigned long time_current=0;

void setup() {
    Serial.begin(38400);

    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);  
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);
    pinMode(TRIG3, OUTPUT);
    pinMode(ECHO3, INPUT);  
    pinMode(TRIG4, OUTPUT);
    pinMode(ECHO4, INPUT);

    pinMode(speedpin1, OUTPUT);
    pinMode(speedpin2, OUTPUT);
    pinMode(IN1, OUTPUT);    
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);    
    pinMode(IN4, OUTPUT);
}

void loop() {
    unsigned long current_time=millis();

    if (current_time - time_previous >= loopdelaytime) {
        time_previous=current_time;
        updateTagPositions();  // 명령 읽고 모터 제어
    }
}

// 시리얼 통신으로 명령 받기
char getTagPositions() {
    char incomingChar = "";
    if (Serial.available() > 0) {
        incomingChar = Serial.read();  // 한 바이트씩 읽기
        Serial.print("area: ");
        Serial.println(incomingChar);
        Serial.println("------------------- ");
        return incomingChar;
    }
    return incomingChar;z
}

// 장애물 회피 동작
void avoidObstacles() {
    stopMotors();
    delay(500);
    if (checksens(TRIG2, ECHO2) <= limitdist || checksens(TRIG1, ECHO1) <= limitdist) {  // 좌측에 장애물 감지
        time_previous = millis();
        while (millis() - time_previous < obstacletime) {
            turnRight(maxspeed);
            }
    } else if (checksens(TRIG3, ECHO3) <= limitdist || checksens(TRIG4, ECHO4) <= limitdist) {  // 우측에 장애물 감지
        time_previous = millis();
        while (millis() - time_previous < obstacletime) {
            turnLeft(maxspeed);
            }
    }
    stopMotors();
}

// 장애물 감지
bool checkObstacles() {
    if (checksens(TRIG2, ECHO2) <= limitdist || checksens(TRIG3, ECHO3) <= limitdist||checksens(TRIG1, ECHO1) <= limitdist || checksens(TRIG4, ECHO4) <= limitdist) {
        Serial.println("Obstacle detected!");
        Serial.println("------------------- ");
        return true;
    }
    return false;
}

// 명령 읽고 동작 수행
void updateTagPositions() {
    char incomingChar='\n';
    while(1){
        incomingChar = getTagPositions();
        if(incomingChar=='l'||incomingChar=='r'||incomingChar=='e'||incomingChar=='g'||incomingChar=='s'){
            break;
        }
    }


    if (incomingChar == 'l') { // 좌회전
        time_previous = millis();
        while (millis() - time_previous < turntime) {
            if (checkObstacles()) {
                stopMotors();
                avoidObstacles();
                return;  // 장애물 감지 시 회피 후 동작 종료
            }
            turnLeft(maxspeed);
        }
        
        // driveForwardForTime 대신 바로 전진 동작 처리
        unsigned long time_previous1 = millis();
        while (millis() - time_previous1 < boosttime) {
            if (checkObstacles()) {
                avoidObstacles();
                Serial.println("나는 탈출 할게");
                break;
            }
            driveForward(maxspeed);  // 직접 함수 호출 대신 전진
        }

        Serial.println("순간부스터 사용");
        stopMotors();
    } 
    else if (incomingChar == 'r') { // 우회전
        time_previous = millis();
        while (millis() - time_previous < turntime) {
            if (checkObstacles()) {
                stopMotors();
                avoidObstacles();
                return;  // 장애물 감지 시 회피 후 동작 종료
            }
            turnRight(maxspeed);
        }

        // driveForwardForTime 대신 바로 전진 동작 처리
        unsigned long time_previous1 = millis();
        while (millis() - time_previous1 < boosttime) {
            if (checkObstacles()) {
                avoidObstacles();
                Serial.println("나는 탈출 할게");
                break;
            }
            driveForward(maxspeed);  // 직접 함수 호출 대신 전진
        }

        Serial.println("순간부스터 사용");
        stopMotors();
    } 
    else if (incomingChar == 'g') { // 직진
        unsigned long time_previous1 = millis();
        while (millis() - time_previous1 < forwardtime) {
            if (checkObstacles()) {
                avoidObstacles();
                //Serial.println("나는 탈출 할게");
                break;
            }
            driveForward(maxspeed);
        }
        stopMotors();
    } 
    else if (incomingChar == 'e') { // 역회전
        time_previous = millis();
        while (millis() - time_previous < reversetime) {
            if (checkObstacles()) {
                stopMotors();
                avoidObstacles();
                return;  // 장애물 감지 시 회피 후 동작 종료
            }
            turnLeft(maxspeed);
        }
        stopMotors();
    } 
    else { // 정지 명령
        stopMotors();
    }

    // 시리얼 버퍼 클리어
    time_previous = millis();
    while (millis() - time_previous < cleartime) {
        Serial.read();
    }
}


// 초음파 센서 거리 측정 함수
long checksens(int trigPin, int echoPin) {
    long duration, distance_us;
    long totalDistance = 0;
    int readings = 3; // 평균을 내기 위한 샘플링 횟수

    for (int i = 0; i < readings; i++) {
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        distance_us = duration * 17 / 1000;
        if (distance_us != 0) {
            totalDistance += distance_us;
        }
    }
    return totalDistance / readings; // 평균 거리 반환
}

// 모터 제어 함수
void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("stop");
}

void driveForward(int speed) {
    digitalWrite(IN1, LOW);   // HIGH -> LOW
    digitalWrite(IN2, HIGH);  // LOW -> HIGH
    digitalWrite(IN3, LOW);   // HIGH -> LOW
    digitalWrite(IN4, HIGH);  // LOW -> HIGH
    analogWrite(speedpin1, speed);
    analogWrite(speedpin2, speed);
    Serial.println("go");
}

void turnRight(int speed) {
    digitalWrite(IN1, LOW);   // HIGH -> LOW
    digitalWrite(IN2, HIGH);  // LOW -> HIGH
    digitalWrite(IN3, HIGH);  // LOW -> HIGH
    digitalWrite(IN4, LOW);   // HIGH -> LOW
    analogWrite(speedpin1, speed);
    analogWrite(speedpin2, speed);
    Serial.println("right");
}

void turnLeft(int speed) {
    digitalWrite(IN1, HIGH);  // LOW -> HIGH
    digitalWrite(IN2, LOW);   // HIGH -> LOW
    digitalWrite(IN3, LOW);   // HIGH -> LOW
    digitalWrite(IN4, HIGH);  // LOW -> HIGH
    analogWrite(speedpin1, speed);
    analogWrite(speedpin2, speed);
    Serial.println("left");
}
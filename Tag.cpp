#include <SoftwareSerial.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <math.h> 

SoftwareSerial mySerial(2, 3); // RX=2, TX=3
const float anchor1_x = 0.2;
const float anchor1_y = -0.1155;
const float anchor2_x = -0.2;
const float anchor2_y = -0.1155;
const float anchor3_x = 0;
const float anchor3_y = 0.2309;
bool received1 = false;
bool received2 = false;
bool received3 = false;

float distance0 = 0, distance1 = 0, distance2 = 0;
float prev_distance0 = 0, prev_distance1 = 0, prev_distance2 = 0; // 이전 거리값 저장
const float h = 0.95; // h 값 설정

void setup() {
    Serial.begin(38400);
    mySerial.begin(115200);
    mySerial.println("AT+anchor_tag=0");
    delay(500);
    mySerial.println("AT+RST");
    delay(500);
    mySerial.println("AT+interval=1");
    delay(500);
    mySerial.println("AT+switchdis=1");
    delay(500);
}

void loop() {
    if (mySerial.available()) {
        String data = mySerial.readStringUntil('\n'); // 데이터 읽기
        if (data.startsWith("distance0:")) {
            distance0 = data.substring(10).toFloat();
            received1 = true;
        } else if (data.startsWith("distance1:")) {
            distance1 = data.substring(10).toFloat();
            received2 = true;
        } else if (data.startsWith("distance2:")) {
            distance2 = data.substring(10).toFloat();
            received3 = true;
        }
    }
    
    if (received1 && received2 && received3) {
        float new_distance0 = sqrt(pow(distance0, 2) - pow(h, 2));
        float new_distance1 = sqrt(pow(distance1, 2) - pow(h, 2));
        float new_distance2 = sqrt(pow(distance2, 2) - pow(h, 2));
        //Serial.print(new_distance0);
        //Serial.print(", ");
        //Serial.print(new_distance1);
        //Serial.print(", ");
        //Serial.println(new_distance2);
        // 변화량 계산
        float change0 = (new_distance0 - prev_distance0);
        float change1 = (new_distance1 - prev_distance1);
        float change2 = (new_distance2 - prev_distance2);
        // 변화량 비교 및 출력
        if ((new_distance2 > (new_distance0 + 0.8)) && (new_distance2 > (new_distance1 + 0.8))) {
            Serial.println('e'); // 예외 처리
        } else if (   (new_distance0 < 1) || (new_distance1 < 1) || (new_distance2 < 1) || (((new_distance0 < 1.5) || (new_distance1 < 1.5) || (new_distance2 < 1.5)) && (change2 > 0.2))  ) {
            Serial.println('s'); // d가 1 미만
        } else if ((fabs(new_distance0 - new_distance1) <= 0.26) && (new_distance0 > new_distance2) && (new_distance1 > new_distance2)) {
        //|| ((change0 < -0.05) && (change1 < -0.05) && (change2 < -0.05) && ((new_distance2 + 0.15) < new_distance1 && (new_distance2 + 0.15) < new_distance0))) {
            Serial.println('g');
        } else if (((change0 > 0.15) && (change1 < 0.05)) || (new_distance0 > (new_distance1 + 0.1))
        || ((-0.15 < change0 <= 0.15) && (-0.15 < change1 <= 0.15) && (new_distance0 > (new_distance1 + 0.1)))) {
        //|| ((change0 < -0.05) && (change1 < -0.05) && (change2 < -0.05) && (  ((new_distance1 + 0.1) < new_distance0) && ((new_distance1 + 0.1) < new_distance2)))) {
            Serial.println('l'); // new_distance0 변화량이 더 큼
        } else if (((change1 > 0.15) && (change0 < 0.05)) || (new_distance1 > (new_distance0 + 0.1)) 
        || ((-0.15 < change0 <= 0.15) && (-0.15 < change1 <= 0.15) && (new_distance1 > (new_distance0 + 0.1)))) {
        //|| ((change0 < -0.05) && (change1 < -0.05) && (change2 < -0.05) && (  ((new_distance0 + 0.1) < new_distance1) && ((new_distance0 + 0.1) < new_distance2)))) {
            Serial.println('r'); // new_distance1 변화량이 더 큼
        } else {
          Serial.println("a");
        }
        // 현재 거리값을 이전 값으로 저장
        prev_distance0 = new_distance0;
        prev_distance1 = new_distance1;
        prev_distance2 = new_distance2;

        received1 = received2 = received3 = false;
    }
}
/*[하드웨어 설명서]
USB host shield가 있는 우노

>LCD
SDA:A4
SCL:A5

>토글 버튼: A3

>RF통신
MISO: 12
SCK: 13
MOSI: 11
CE: 7
CSN: 8 
VCC: 3.3V
*/

#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8);
const byte address[6] = "01001"; // 주소값을 부여합니다. 송신기와 수신기를 동일한 주소로 입력 해야합니다.

// 바코드를 저장할 전역 변수 선언
String barcode = "";  // 바코드 데이터가 저장될 변수
String name="";
signed long price=0;
signed long total=0;
signed long num=0;
signed long num1=0;
signed long num2=0;

// 사용자 정의 HID 리포트 파서 클래스
class MyParser : public HIDReportParser {
public:
    MyParser();  // 생성자
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);

private:
    bool newScan;  // 새로운 스캔이 시작되었는지 여부
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    void OnScanFinished();
};

// USB 및 HID 관련 객체 선언
USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser Parser; // HID 리포트 파서 객체

// LCD 객체 선언
LiquidCrystal_I2C lcd(0x27, 20, 4);    

MyParser::MyParser() : newScan(true) {}  // 생성자에서 newScan 초기화

void setup() {
    Serial.begin(38400);

    lcd.init();  // LCD 초기 설정
    lcd.backlight(); // LCD 백라이트 켜기
    lcd.setCursor(0,0); 
  
  //처음 텍스트가 LCD에 나타날 위치  lcd.setCursor(열, 행)
  //밑에 사진을 통해 출력시작 위치를 확인할 수 있습니다.
  
    lcd.print("Price: "); 
    lcd.print(price);
    
    lcd.setCursor(0,1); 
    
    lcd.print("Total: "); 
    lcd.print(total);

    if (Usb.Init() == -1) {
        Serial.println("OSC did not start.");
        while (1); // 오류 발생 시 무한 루프
    }

    Hid.SetReportParser(0, &Parser); // HID 리포트 파서 설정

    radio.begin();
    radio.openWritingPipe(address); // 데이터를 보낼 수신의 주소를 설정합니다.
    radio.setPALevel(RF24_PA_HIGH); // 전원공급에 관한 파워레벨을 설정합니다.
    radio.stopListening(); // 모듈을 송신기로 설정합니다.
}

void loop() {
    Usb.Task();
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // 시리얼 입력을 줄바꿈까지 읽음
        char text[input.length() + 1]; // 문자 배열 버퍼 생성
        input.toCharArray(text, input.length() + 1); // String을 char 배열로 변환

        // 수신기에게 메시지 전송
        radio.write(&text, sizeof(text));
        Serial.println(input);
    }
}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    // 에러 또는 빈 상태인 경우 리턴
    if (buf[2] == 1 || buf[2] == 0) return;

    for (uint8_t i = 7; i >= 2; i--) {
        // 빈 값이면 건너뜀
        if (buf[i] == 0) continue;

        // 엔터 키가 눌렸을 경우 스캔 종료
        if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
            OnScanFinished();
        } else {
            // 대문자 키 여부를 판단하여 처리
            OnKeyScanned(i > 2, buf[0], buf[i]);
        }
        return;
    }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
    // 알파벳 문자 변환
    if (key >= 0x04 && key <= 0x1d) {
        return upper ? (key - 4 + 'A') : (key - 4 + 'a');
    }
    // 숫자 변환
    else if (key >= 0x1e && key <= 0x27) {
        return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
    }

    return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
    if (newScan) {
        Serial.print("P");  // 새로운 스캔이 시작되면 "P"를 출력
        newScan = false;    // 다음 문자에 대해 "P"가 출력되지 않도록 설정
    }
    
    uint8_t ascii = KeyToAscii(upper, mod, key);
    Serial.print((char)ascii);
    barcode += (char)ascii;  // barcode 변수에 현재 문자 추가
}

void MyParser::OnScanFinished() {
    Serial.println();  // 스캔 종료 후 줄바꿈
    newScan = true;    // 새로운 스캔이 시작될 수 있도록 설정

    if (barcode == "1234") {
        price = 5000;
        name= "A";
    } else if (barcode == "5678") {
        price = 3000;
        name="B";
    } else if (barcode == "abcd") {
        price = 1000;
        name="C";
        
    } else {
        price = 0;
    }
    // 스캔이 완료되었으므로 barcode 변수에 저장된 바코드 데이터를 LCD에 출력
    lcd.clear(); // LCD 화면 지우기

    int i=analogRead(3);
    if(i>1000){//4.8V
        lcd.setCursor(0, 0); // 첫 번째 줄로 커서 이동
        lcd.print("Canceling: -"); 
        lcd.print(price); // 가격 출력

        const char text[] = "C";
        radio.write(&text, sizeof(text)); // 해당 메세지를 수신자에게 보냅니다.
        Serial.println(text);

        if ((price == 5000) && (num == 0)) {
          total=total;
        } else if ((price == 3000) && (num1 == 0)) {
          total=total;
        } else if ((price == 1000) && (num2 == 0)) {
          total=total;
        } else {
          total-=price;
        }

        if (price == 5000) {
          num-=1;
        } else if (price == 3000) {
          num1-=1;
        } else if (price == 1000) {
          num2-=1;
        }

    }
    else{
        lcd.setCursor(0, 0); // 첫 번째 줄로 커서 이동
        lcd.print("Price: "); 
        lcd.print(price); // 가격 출력
        
        const char text[] = "P";
        radio.write(&text, sizeof(text)); // 해당 메세지를 수신자에게 보냅니다.
        Serial.println(text);
    
        total+=price;

        if (price == 5000) {
          num+=1;
        } else if (price == 3000) {
          num1+=1;
        } else if (price == 1000) {
          num2+=1;
        }
    }
    if(total<0){
      total=0;
    }
    if(num<=0){
      num=0;
    }
    if(num1<=0){
      num1=0;
    }
    if(num2<=0){
      num2=0;
    }

    lcd.setCursor(0, 1); // 두 번째 줄로 커서 이동
    lcd.print("Total: "); 
    lcd.print(total); // 총액 출력

    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 1); // 두 번째 줄로 커서 이동
    lcd.print("Total: "); 
    lcd.print(total); // 총액 출력

    lcd.setCursor(0, 0); // 첫 번째 줄로 커서 이동
    lcd.print("name: "); 
    lcd.print(name); // 상품명 출력
    lcd.print("  x");

    if (name == "A") {
      lcd.print(num);
    } else if (name == "B") {
      lcd.print(num1);
    } else if (name == "C") {
      lcd.print(num2);
    }

    barcode = "";  // barcode 변수를 초기화하여 다음 스캔에 대비
}
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>
#include <Servo.h>

#define LINE_TOKEN " "

char auth[] = " ";
char ssid[] = " ";
char pass[] = " ";

Servo myServo;
const int servoPin = D3;

void setup() {
  Serial.begin(115200);
  myServo.attach(servoPin);
  Blynk.begin(auth, ssid, pass, "elec.cmtc.ac.th", 8080);

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);
}

BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    myServo.write(180);
    LINE.notify("เปิดประตู");
    LINE.notifySticker(2, 34);  // ส่ง Line Sticker ด้วย PackageID 3 , StickerID 240
    // ตัวอย่างส่ง รูปภาพ ด้วย url
    // LINE.notifyPicture("https://preview.ibb.co/j6G51n/capture25610417181915334.png");
  } else
    myServo.write(0);
}

void loop() {
  Blynk.run();
}
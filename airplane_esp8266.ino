
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


//wifi
#define AP_SSID "airplanex"
#define AP_PSW  "wuhuqifei!"
const int SERVER_PORT = 5000;
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer WiFi_Server(SERVER_PORT);
WiFiClient *WiFi_Client[1] = {0};
WiFiClient WiFi_Client_x;

//pwm
#define pwmMIN  195                           //215
#define pwmMAX  388                           //373

//#define pwmMID  293  //50HZ--215--373--293 //67HZ--290--500--393
//197--387水平，竖直：198--388
int flag;

//int pwmCount0 = pwmMID;
//int pwmCount1 = pwmMID; //293
//int pwmCount2 = pwmMID; //293
//int pwmCount3 = pwmMID; //293
//int pwmCount4 = pwmMID; //293
//

String str_order;
int safeCheck;


void setup() {
  flag = 0;
  ////WIFI
  // 设置波特率
  Serial.begin(115200);
  // 设置wifi模块为AP模式
  WiFi.mode(WIFI_AP);
  // 设置IP地址网关和子网掩码
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(AP_SSID, AP_PSW);
  Serial.println("IP address = ");
  Serial.print(WiFi.softAPIP());
  //启动wifiserver服务
  WiFi_Server.begin();
  //Serial.println("Server online.");

  ////PWM
  pwm.begin();
  pwm.setPWMFreq(50); //50

  //pwm.setPWM(0, 0, pwmMID);
  //pwm.setPWM(1, 0, pwmMID);
  //pwm.setPWM(2, 0, pwmMID);
  //pwm.setPWM(3, 0, pwmMID);
  //pwm.setPWM(4, 0, pwmMID);
  //pwm.setPWM(5, 0, pwmMID);
}

void loop() {
  // 判断是否有新的连接

  WiFi_Client_x = WiFi_Server.available();
  // 判断这个连接是否有效
  if (WiFi_Client_x.connected()) {
    Serial.println("连接有效");
    flag = 1 ;
    Serial.println("flag：" + String(flag));
    WiFi_Client_x.setNoDelay(true);            // 如果有效，那么设置连接发送数据不延时
    Serial.println("在外面");
    Serial.println((int)WiFi_Client[0]);
    if (WiFi_Client[0] == 0 || !WiFi_Client[0]->connected()) {
      // 我们新建一个TCP连接，然后把这个连接放入我们的连接池，放入后跳出循环
      WiFi_Client[0] = new WiFiClient(WiFi_Client_x);
      Serial.println("进来了a");
      Serial.println((int)WiFi_Client[0]);
      Serial.println("进来了b");
      //break;
    }
  }
  int station_num = WiFi.softAPgetStationNum();
  if (station_num == 0 && flag == 1) {
    pwm.setPWM(4, 0, 250);
    WiFi_Client[0] = 0;
    //pwm.setPWM(4,0,250);
    //Serial.println("flag为1,stanum为0,发出迫降指令！");
    flag = 0 ;
    
  }
  if (WiFi_Client[0] != 0 && WiFi_Client[0]->connected()) {
    // 如果有效，尝试获取tcp发送的数据
    if (WiFi_Client[0]->available()) {
      // 这里说明有数据，我们直接读取tcp连接发送的数据
      String data = WiFi_Client[0]->readStringUntil('\n');

      /////////////
      str_order = data.substring(0, 5);
      safeCheck = data.substring(5, data.length()).toInt();

      //ch3,油门
      if (str_order == "throt") {
        if (safeCheck < pwmMAX && safeCheck > pwmMIN) {
          pwm.setPWM(2, 0, safeCheck);
          //Serial.println("ch3,throt油门：" + data);
        }
      }
      //ch4,旋转
      if (str_order == "yawww") {
        if (safeCheck < pwmMAX && safeCheck > pwmMIN) {
          pwm.setPWM(3, 0, safeCheck);
          //Serial.println("ch4,yawww旋转：" + data);
        }
      }
      //ch2,俯仰角
      if (str_order == "pitch") {
        if (safeCheck < pwmMAX && safeCheck > pwmMIN) {
          pwm.setPWM(1, 0, safeCheck);
          //Serial.println("ch3,pitch俯仰角：" + data);
        }
      }
      //ch1,翻转角
      if (str_order == "rolll") {
        if (safeCheck < pwmMAX && safeCheck > pwmMIN) {
          pwm.setPWM(0, 0, safeCheck);
          //Serial.println("ch4,rolll翻转角：" + data);
        }
      }
      //ch5,飞行模式
      if (str_order == "fmode") {
        pwm.setPWM(4, 0, safeCheck);
        //Serial.println("ch5,fmode飞行模式：" + data);
      }

      //舵机下
      if (str_order == "seekL") {
        pwm.setPWM(6, 0, safeCheck);
        Serial.println("true link");
      }
      //舵机上
      if (str_order == "seekR") {
        pwm.setPWM(7, 0, safeCheck);
      }

    }

  }
}

#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>

// 定义 OLED 屏幕 (I2C 接口)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 定义 DHT11 传感器
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  // 初始化屏幕
  u8g2.begin();
  // 开启中文字库支持
  u8g2.enableUTF8Print();
  
  // 初始化传感器
  dht.begin();

  // 开机画面
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // 使用中文字体
  u8g2.drawUTF8(0, 20, "AI桌面助手已启动");
  u8g2.drawUTF8(0, 40, "等待电脑发消息...");
  u8g2.sendBuffer();
  
  Serial.println("ESP32 启动完毕！");
}

void loop() {
  // 1. 读取温湿度（每隔2秒读一次防止传感器过热）
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 2000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      // 把温湿度发给电脑，让电脑端的 Python 获取
      Serial.print("ENV:"); 
      Serial.print(t);
      Serial.print(",");
      Serial.println(h);
    }
    lastRead = millis();
  }

  // 2. 接收电脑发来的 AI 回复
  if (Serial.available() > 0) {
    String incomingMessage = Serial.readStringUntil('\n');
    
    // 屏幕清屏
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312); 
    
    // 显示 AI 的回复
    u8g2.drawUTF8(0, 15, "AI 说:");
    // 假设回复不长，直接在第二行显示
    u8g2.drawUTF8(0, 35, incomingMessage.c_str()); 
    
    u8g2.sendBuffer(); // 刷新屏幕显示
    
    // 让板载LED闪一下，表示收到了
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
  }
}

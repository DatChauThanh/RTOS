#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "FreeRTOSConfig.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

// Khởi tạo đối tượng màn hình OLED
Adafruit_SH1106G display(128, 64, &Wire);

#define DHTPIN GPIO_NUM_14           // Chân dữ liệu của cảm biến DHT11
#define DHTTYPE DHT11       // Loại cảm biến DHT11       
#define FIRESENSOR_PIN GPIO_NUM_4
#define MOTIONSENSOR_PIN GPIO_NUM_5
#define HUMR_PIN GPIO_NUM_26
#define SOUND_PIN  GPIO_NUM_27
#define MQ3_PIN GPIO_NUM_25
#define JOYSTICK_X GPIO_NUM_35     // Chân GPIO cho trục X của joystick
#define JOYSTICK_Y GPIO_NUM_34

DHT dht(DHTPIN, DHTTYPE);

float humidity = 0;
float temperature = 0;
void dht11_task(void *pvParameter) {  
  for (;;) {  
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();        
    vTaskDelay(500); // Delay 2 giây        
  }
}

int fireState = 0;
void firesensor_task(void *pvParameter){
  pinMode(FIRESENSOR_PIN, INPUT);
  while(1)
  {
    fireState = analogRead(FIRESENSOR_PIN);
    vTaskDelay(200);
  }
  
}

int soundState = 0;
String sound = "NO";
void soundsensor_task(void *pvParameter){
  pinMode(SOUND_PIN,INPUT);
  while(1)
  {
    soundState = analogRead(SOUND_PIN);
    Serial.println(soundState);
    vTaskDelay(200);
  }  
}

int HumState;
int percent;
void humr_task(void *pvParameter){
  pinMode(HUMR_PIN,INPUT);
  while(1)
  {
    HumState = analogRead(HUMR_PIN);
    percent = map(HumState, 0, 1023, 0, 100);
    vTaskDelay(200);
  }  
}

int x = 0; 
int y = 0;
void joystick_task(void *pvParameter) {  
  for (;;) {    
    x = analogRead(JOYSTICK_X);
    y = analogRead(JOYSTICK_Y);
    vTaskDelay(200); // Delay 0.2 giây
    
  }
}

int mq3 = 0;
const float referenceVoltage = 3.3; 
void MQ3_task(void *pvParameter){ 
  while(1)
  {
    mq3 = analogRead(MQ3_PIN);  // Đọc giá trị từ cảm biến
    // float voltage = mq3 * referenceVoltage / 4095;  // Chuyển đổi giá trị ADC thành điện áp
    // float ppm = map(voltage, 0.0, referenceVoltage, 0.0, 10000.0);  // Ánh xạ giá trị điện áp sang PPM  
    vTaskDelay(200);  // Tần số đọc cảm biến     
  }  
}

void oled_task(void *pvParameter){
  // Khởi tạo màn hình OLED
  display.begin(0x3c,true );
  // Tắt điểm ảnh
  display.clearDisplay();

  // Đặt màu cho chữ
  display.setTextColor(SH110X_WHITE);
  
  while(true)
  {     
    // Tắt điểm ảnh
    display.clearDisplay();
    
    display.setCursor(0, 0);
    display.println("MQ3: " + String(mq3));
    display.setCursor(0, 12);
    display.println("JoyX: " + String(x));
    display.setCursor(0, 24);
    display.println("JoyY: " + String(y));
    display.setCursor(0, 36);
    display.println("HumR: " + String(percent));    
    display.setCursor(0, 48);
    display.println("Sound: " + String(soundState));    
    display.setCursor(64, 0);
    display.println("Tem: " + String(temperature, 1) + "C");
    display.setCursor(64, 12);
    display.println("Hum: " + String(humidity, 1) + "%");
    display.setCursor(64, 24);
    display.println("Fire: " + String(fireState));
 
    display.display();
    vTaskDelay(500);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht.begin();  
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  xTaskCreate(firesensor_task, "firesensor_task", 4096, NULL, 1, NULL);
  xTaskCreate(soundsensor_task, "sound_task", 4096, NULL, 2, NULL);
  xTaskCreate(humr_task, "humr_task", 4096, NULL, 3, NULL);
  xTaskCreate(dht11_task, "dht11_task", 4096, NULL, 4, NULL); 
  xTaskCreate(joystick_task, "joystick_task", 4096, NULL, 5, NULL); 
  xTaskCreate(MQ3_task, "MQ3_task", 4096, NULL, 6, NULL);
  xTaskCreate(oled_task, "OLED_task", 4096, NULL, 7, NULL); 
}

void loop() {
  // không cần làm gì trong hàm loop()
}

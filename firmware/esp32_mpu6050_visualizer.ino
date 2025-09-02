#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  Serial.begin(115200);
  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1) yield();
  }
  Serial.println("Found a MPU-6050 sensor");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  lcdOpening("MPU6050");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // --- Hitung orientasi sederhana ---
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;
  float yaw   = atan2(az, sqrt(ax * ax + ay * ay)) * 180.0 / PI;

  // --- Kirim ke serial dalam format: pitch,roll,yaw ---
  Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", pitch, roll, yaw, ax, ay, az);

  // --- Tampilkan di OLED ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Pitch, Roll, Yaw:");
  display.setCursor(0,12);
  display.printf("P: %.1f", pitch);
  display.setCursor(64,12);
  display.printf("R: %.1f", roll);
  display.setCursor(0,24);
  display.printf("Y: %.1f", yaw);

  display.setCursor(0,40);
  display.println("Acc X,Y,Z:");
  display.setCursor(0,52);
  display.printf("%.1f,%.1f,%.1f", ax, ay, az);

  display.display();
  delay(20);
}

void lcdOpening(const char *teks){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,10);
  display.print("Test Sensor");
  display.setTextSize(2);
  display.setCursor(0,30);
  display.print(teks);

  display.display();
  delay(2000);
}

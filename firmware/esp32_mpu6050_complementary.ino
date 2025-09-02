#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// Mahony filter variables
float Kp = 2.0 * 0.5; // Proportional gain
float Ki = 0.0;     // Integral gain (set to 0 for now, can be tuned)
float q0 = 1.0, q1 = 0.0, q2 = 0.0, q3 = 0.0; // Quaternion elements representing the estimated orientation
float exInt = 0.0, eyInt = 0.0, ezInt = 0.0; // Scaled integral error

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1) yield();
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  lcdOpening("MPU6050");
  lastTime = millis();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;
  float gx = g.gyro.x * 180.0 / PI; // deg/s
  float gy = g.gyro.y * 180.0 / PI; // deg/s
  float gz = g.gyro.z * 180.0 / PI; // deg/s - Deklarasi gz di sini

  // Accelerometer angle
  float accPitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float accRoll  = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  // Delta time
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  // Convert gyroscope to radians/s
  gx *= PI / 180.0;
  gy *= PI / 180.0;
  gz *= PI / 180.0; // Yaw gyro in rad/s

  // Mahony filter update
  MahonyAHRSupdate(gx, gy, gz, ax, ay, az, dt);

  // Kirim quaternion ke serial
  Serial.printf("%.4f,%.4f,%.4f,%.4f\n", q0, q1, q2, q3);

  // OLED (opsional, bisa dihapus jika tidak digunakan)
  // Untuk menampilkan di OLED, kita bisa konversi quaternion ke Euler angles di sini
  // atau menampilkan nilai quaternion mentah. Untuk saat ini, saya akan menampilkan
  // nilai quaternion mentah.
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Quaternion:");
  display.setCursor(0,12);
  display.printf("q0: %.2f", q0);
  display.setCursor(0,24);
  display.printf("q1: %.2f", q1);
  display.setCursor(0,36);
  display.printf("q2: %.2f", q2);
  display.setCursor(0,48);
  display.printf("q3: %.2f", q3);
  display.display();
  delay(10);
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

// Mahony AHRS algorithm
void MahonyAHRSupdate(float Gx, float Gy, float Gz, float Ax, float Ay, float Az, float dt) {
  float recipNorm;
  float vx, vy, vz;
  float ex, ey, ez;

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((Ax == 0.0f) && (Ay == 0.0f) && (Az == 0.0f))) {
    // Normalise accelerometer measurement
    recipNorm = 1.0f / sqrt(Ax * Ax + Ay * Ay + Az * Az);
    Ax *= recipNorm;
    Ay *= recipNorm;
    Az *= recipNorm;

    // Estimated direction of gravity and flux (v and w)
    vx = 2*(q1*q3 - q0*q2);
    vy = 2*(q0*q1 + q2*q3);
    vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

    // Error is sum of cross product between estimated and measured direction of gravity
    ex = (Ay * vz - Az * vy);
    ey = (Az * vx - Ax * vz);
    ez = (Ax * vy - Ay * vx);

    // Integrate error proportional to Ki (integral gain)
    if(Ki > 0.0f) {
      exInt += ex * dt; // Accumulate integral error
      eyInt += ey * dt;
      ezInt += ez * dt;
    } else {
      exInt = 0.0f; // Prevent integral bias if Ki is zero
      eyInt = 0.0f;
      ezInt = 0.0f;
    }

    // Apply proportional and integral feedback
    Gx += Kp * ex + Ki * exInt;
    Gy += Kp * ey + Ki * eyInt;
    Gz += Kp * ez + Ki * ezInt;
  }

  // Integrate rate of change of quaternion
  Gx *= (0.5f * dt); // pre-multiply common factors
  Gy *= (0.5f * dt);
  Gz *= (0.5f * dt);
  q0 += (-q1 * Gx - q2 * Gy - q3 * Gz);
  q1 += (q0 * Gx + q2 * Gz - q3 * Gy);
  q2 += (q0 * Gy - q1 * Gz + q3 * Gx);
  q3 += (q0 * Gz + q1 * Gy - q2 * Gx);

  // Normalise quaternion
  recipNorm = 1.0f / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}

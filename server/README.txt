# MPU6050 Game Server

## Setup & Installation

1. Install Node.js dependencies:
   ```
   npm install
   ```

2. Update COM port di file serial-socket.js sesuai dengan port ESP32 Anda:
   ```javascript
   const port = new SerialPort('COM3', { baudRate: 115200 }); // Ganti COM3
   ```

3. Jalankan server:
   ```
   npm start
   ```
   atau untuk development:
   ```
   npm run dev
   ```

## Cara Kerja

- Server membaca data dari ESP32 melalui Serial port
- Data diteruskan ke browser melalui WebSocket (port 8080)
- Format data yang diharapkan: "R:roll,P:pitch,Y:yaw" atau "roll:pitch:yaw"

## Troubleshooting

- Pastikan ESP32 terhubung dan port serial sudah benar
- Pastikan tidak ada aplikasi lain yang menggunakan port serial
- Cek Device Manager untuk melihat port COM yang aktif

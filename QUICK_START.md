# Quick Start Guide - MPU6050 3D Game

## Langkah-langkah Cepat

### 1. Setup Server
```bash
cd server
npm install
# Edit serial-socket.js untuk port COM yang benar
npm start
```

### 2. Upload Firmware ke ESP32
- Buka Arduino IDE
- Upload file `firmware/esp32_mpu6050_complementary.ino` atau `esp32_mpu6050_visualizer.ino`

### 3. Jalankan Game
- Buka `visualizer/index.html` di browser
- atau untuk monitoring data: buka `visualizer/monitor.html`

### 4. Test Koneksi
1. Pastikan ESP32 terhubung dan firmware running
2. Server menampilkan "WebSocket server running..."
3. Browser menampilkan status "Connected" di panel sensor

## Kontrol Game
- **Sensor MPU6050**: Tilt ESP32 untuk menggerakkan karakter
- **Manual**: Gunakan tombol di layar atau keyboard (WASD/Arrow keys)

## File Penting
- `server/serial-socket.js` - WebSocket server
- `visualizer/index.html` - Game utama  
- `visualizer/monitor.html` - Monitor data sensor
- `firmware/*.ino` - Kode Arduino untuk ESP32

## Troubleshooting Cepat
- **No data**: Cek port COM di serial-socket.js
- **Connection error**: Restart server dan refresh browser
- **Game tidak jalan**: Pastikan browser mendukung ES6 modules

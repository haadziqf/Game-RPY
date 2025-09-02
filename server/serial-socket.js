const SerialPort = require('serialport');
const WebSocket = require('ws');
const config = require('./config');

console.log('MPU6050 Game Server Starting...');
console.log(`Serial Port: ${config.serial.port}`);
console.log(`WebSocket Port: ${config.websocket.port}`);

const port = new SerialPort(config.serial.port, { 
  baudRate: config.serial.baudRate 
});

const wss = new WebSocket.Server({ 
  port: config.websocket.port 
});

// Data validation function
function validateSensorData(roll, pitch, yaw) {
  return (
    roll >= config.data.validation.minRoll && roll <= config.data.validation.maxRoll &&
    pitch >= config.data.validation.minPitch && pitch <= config.data.validation.maxPitch &&
    yaw >= config.data.validation.minYaw && yaw <= config.data.validation.maxYaw
  );
}

// Enhanced data processing
port.on('data', (data) => {
  const rawData = data.toString().trim();
  
  if (config.logging.logRawData) {
    console.log('Raw data:', rawData);
  }
  
  // Try to parse and validate data
  try {
    let roll, pitch, yaw;
    let isParsed = false;
    
    // Try different data formats
    if (rawData.includes('R:') && rawData.includes('P:') && rawData.includes('Y:')) {
      const rollMatch = rawData.match(/R:([-\d.]+)/);
      const pitchMatch = rawData.match(/P:([-\d.]+)/);
      const yawMatch = rawData.match(/Y:([-\d.]+)/);
      
      if (rollMatch && pitchMatch && yawMatch) {
        roll = parseFloat(rollMatch[1]);
        pitch = parseFloat(pitchMatch[1]);
        yaw = parseFloat(yawMatch[1]);
        isParsed = true;
      }
    } else if (rawData.includes(':')) {
      const values = rawData.split(':');
      if (values.length === 3) {
        roll = parseFloat(values[0]);
        pitch = parseFloat(values[1]);
        yaw = parseFloat(values[2]);
        isParsed = true;
      }
    }
    
    // Validate and send data
    if (isParsed && validateSensorData(roll, pitch, yaw)) {
      if (config.logging.logParsedData) {
        console.log(`Parsed data - Roll: ${roll}, Pitch: ${pitch}, Yaw: ${yaw}`);
      }
      
      // Send to all connected WebSocket clients
      wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(rawData);
        }
      });
    } else if (isParsed && config.logging.logErrors) {
      console.log('Invalid sensor data values:', roll, pitch, yaw);
    }
  } catch (error) {
    if (config.logging.logErrors) {
      console.error('Data parsing error:', error.message);
    }
  }
});

// Handle serial port events
port.on('open', () => {
  console.log('Serial port opened successfully');
});

port.on('error', (err) => {
  console.error('Serial port error:', err.message);
});

// Handle WebSocket connections
wss.on('connection', (ws) => {
  console.log('New WebSocket client connected');
  
  ws.on('close', () => {
    console.log('WebSocket client disconnected');
  });
  
  ws.on('error', (error) => {
    console.error('WebSocket error:', error.message);
  });
});

console.log(`WebSocket server running on ws://${config.websocket.host}:${config.websocket.port}`);
console.log('Waiting for ESP32 data...');

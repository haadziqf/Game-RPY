// Configuration file for MPU6050 Game Server
module.exports = {
    // Serial Port Configuration
    serial: {
        port: 'COM3',           // Change this to your ESP32 COM port
        baudRate: 115200,       // ESP32 baud rate
        autoDetect: false       // Set to true to auto-detect ESP32 port
    },
    
    // WebSocket Server Configuration  
    websocket: {
        port: 8080,             // WebSocket server port
        host: 'localhost'       // Server host
    },
    
    // Data Processing Configuration
    data: {
        // Expected data formats (in order of preference)
        formats: [
            'R:{roll},P:{pitch},Y:{yaw}',  // Format 1: R:10.5,P:5.2,Y:180.3
            '{roll}:{pitch}:{yaw}'         // Format 2: 10.5:5.2:180.3
        ],
        
        // Data validation
        validation: {
            minRoll: -180,
            maxRoll: 180,
            minPitch: -90,
            maxPitch: 90,
            minYaw: -180,
            maxYaw: 180
        }
    },
    
    // Logging Configuration
    logging: {
        logRawData: false,      // Log all received serial data
        logParsedData: true,    // Log successfully parsed sensor data
        logErrors: true         // Log parsing errors
    }
};

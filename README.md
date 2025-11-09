# ESP32 Sensor Server

A Java servlet-based server for collecting and monitoring ESP32 sensor data with a web dashboard.

## Features

- **Dual Server Support**: Works with both public servers (domain name) and local servers (default gateway)
- **Real-time Dashboard**: Web-based dashboard showing all sensor readings
- **Threshold Management**: Update sensor thresholds via web interface
- **Browser Notifications**: Automatic browser notifications when sensors exceed thresholds
- **RESTful API**: Clean REST API for ESP32 communication
- **Polling System**: ESP32 polls server every 2 seconds for threshold updates

## Sensors Supported

- **MQ1, MQ2, MQ3**: Gas sensors (analog ADC values)
- **DHT11**: Temperature and humidity sensor
- **Dust Sensor**: Sharp GP2Y10 dust density sensor
- **WiFi Signal**: Signal strength monitoring
- **Uptime**: Device uptime tracking

## Project Structure

```
esp32-server/
├── pom.xml                          # Maven configuration
├── run.bat                          # Windows run script
├── run.sh                           # Linux/Mac run script
├── README.md                        # This file
├── QUICKSTART.md                    # Quick start guide
├── BUILD_AND_RUN.md                 # Detailed build/run instructions
├── DEPLOYMENT.md                    # Deployment guide
├── src/
│   ├── main/
│   │   ├── java/
│   │   │   └── com/esp32/
│   │   │       ├── model/           # Data models
│   │   │       │   ├── SensorData.java
│   │   │       │   └── Thresholds.java
│   │   │       ├── service/         # Business logic
│   │   │       │   └── DataService.java
│   │   │       ├── servlet/         # REST endpoints
│   │   │       │   ├── DataServlet.java      # POST /api/data
│   │   │       │   ├── PollServlet.java      # GET /api/poll
│   │   │       │   ├── ThresholdServlet.java # GET/POST /api/thresholds
│   │   │       │   ├── LatestDataServlet.java # GET /api/latest
│   │   │       │   └── DashboardServlet.java # GET /
│   │   │       ├── server/          # Embedded server
│   │   │       │   └── EmbeddedServer.java   # Main server class
│   │   │       └── filter/
│   │   │           └── CORSFilter.java
│   │   └── webapp/
│   │       └── WEB-INF/
│   │           ├── dashboard.html   # Web dashboard
│   │           └── web.xml          # Servlet configuration
└── sketch_nov9a.ino                 # ESP32 Arduino code
```

## Building and Running the Server

### Prerequisites

- Java 11 or higher
- Maven 3.6 or higher

### Quick Start (Embedded Server - No WAR deployment needed)

**Build and run in one command:**

**Windows:**
```batch
mvn clean compile exec:java
```

**Linux/Mac:**
```bash
mvn clean compile exec:java
```

**Or use the run scripts:**

**Windows:**
```batch
run.bat
```

**Linux/Mac:**
```bash
chmod +x run.sh
./run.sh
```

**Custom port:**
```bash
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
```

The server will start on port 8085 (or specified port) and be accessible at:
- Dashboard: http://localhost:8085/
- API Base: http://localhost:8085/api/

### Alternative: Build WAR for Deployment

If you want to deploy to a servlet container (Tomcat, etc.):

1. **Package as WAR**:
   ```bash
   mvn clean package
   ```

2. **Deploy to servlet container**:
   - Copy `target/esp32-server-1.0.0.war` to your servlet container's webapps directory
   - Or use your IDE to deploy directly

## ESP32 Configuration

### WiFi Setup

1. Open `sketch_nov9a.ino`
2. Update WiFi credentials:
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```

### Server Configuration

The ESP32 automatically switches between public and local servers based on availability:

**Public Server Configuration:**
```cpp
const char* PUBLIC_SERVER_URL = "https://your-domain.com";
```

**Local Server Configuration:**
```cpp
const int LOCAL_SERVER_PORT = 8085;  // Port for local server
// Local server URL is automatically constructed from gateway IP: http://<gateway-ip>:8085
```

**Server Switching Logic:**
1. ESP32 always tries the **public server first**
2. If public server returns HTTP 200, it continues using the public server
3. If public server fails, it automatically switches to the **local server** (gateway IP:8085)
4. If local server returns HTTP 200, it continues using the local server
5. If the current server fails, it automatically tries the other server
6. If both servers fail, it resets and tries public server first again next time

This ensures maximum reliability - the ESP32 will automatically use whichever server is available.

### Required Libraries

Install these libraries in Arduino IDE:
- `DHT sensor library` by Adafruit
- `ArduinoJson` by Benoit Blanchon (version 6.x)

## API Endpoints

### POST /api/data
ESP32 posts sensor data to this endpoint.

**Request Body**:
```json
{
  "temperature": 25.5,
  "humidity": 60.0,
  "mq1": 250,
  "mq2": 280,
  "mq3": 500,
  "dust": 45.2,
  "wifiSignal": -65,
  "uptime": 3600
}
```

**Response**:
```json
{
  "status": "success",
  "message": "Data received"
}
```

### GET /api/poll
ESP32 polls this endpoint every 2 seconds to check for threshold updates.

**Response** (when update available):
```json
{
  "update": true,
  "thresholds": {
    "mq1": 350,
    "mq2": 300,
    "mq3": 600,
    "temperature": 30.0,
    "dust": 50.0
  }
}
```

**Response** (no update):
```json
{
  "update": false
}
```

### GET /api/latest
Get the latest sensor data (used by dashboard).

**Response**:
```json
{
  "temperature": 25.5,
  "humidity": 60.0,
  "mq1": 250,
  "mq2": 280,
  "mq3": 500,
  "dust": 45.2,
  "wifiSignal": -65,
  "uptime": 3600,
  "timestamp": "2024-01-01T12:00:00"
}
```

### GET /api/thresholds
Get current thresholds.

**Response**:
```json
{
  "mq1": 300,
  "mq2": 300,
  "mq3": 600,
  "temperature": 30.0,
  "dust": 50.0
}
```

### POST /api/thresholds
Update thresholds (from web dashboard).

**Request Body**:
```json
{
  "mq1": 350,
  "mq2": 300,
  "mq3": 600,
  "temperature": 30.0,
  "dust": 50.0
}
```

**Response**:
```json
{
  "status": "success",
  "message": "Thresholds updated"
}
```

### GET /
Web dashboard (HTML page)

## Web Dashboard Features

- **Real-time Updates**: Dashboard polls server every 2 seconds
- **Visual Alerts**: Sensor values turn red when thresholds are exceeded
- **Browser Notifications**: Desktop notifications when thresholds are exceeded
- **Threshold Management**: Update thresholds directly from the dashboard
- **Connection Status**: Shows online/offline status
- **Uptime Display**: Shows device uptime in human-readable format

## Browser Notifications

The dashboard will request notification permission on first load. Notifications are sent when:
- Any MQ sensor exceeds its threshold
- Temperature exceeds threshold
- Dust density exceeds threshold

Notifications have a 10-second cooldown per sensor to prevent spam.

## Troubleshooting

### ESP32 Cannot Connect to Server

1. **Check WiFi credentials**: Ensure SSID and password are correct
2. **Check server URL**: For local server, ensure ESP32 and server are on same network
3. **Check firewall**: Ensure server port (default 8085) is accessible
4. **Check gateway IP**: For local server, verify default gateway is correct

### Server Not Receiving Data

1. **Check CORS**: CORS filter should allow all origins (already configured)
2. **Check servlet mapping**: Ensure web.xml is properly configured
3. **Check logs**: Check servlet container logs for errors

### Dashboard Not Updating

1. **Check browser console**: Open developer tools and check for JavaScript errors
2. **Check API endpoints**: Verify `/api/latest` returns data
3. **Check notification permission**: Ensure browser allows notifications

## License

This project is provided as-is for educational and personal use.


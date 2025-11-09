# Quick Start Guide

## Prerequisites

- Java 11 or higher
- Maven 3.6 or higher

## Build and Run

### Option 1: Using Maven (Recommended)

**Windows:**
```batch
mvn clean compile exec:java
```

**Linux/Mac:**
```bash
mvn clean compile exec:java
```

### Option 2: Using Run Scripts

**Windows:**
```batch
run.bat
```

**Linux/Mac:**
```bash
chmod +x run.sh
./run.sh
```

### Option 3: Custom Port

**Windows:**
```batch
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
```

**Linux/Mac:**
```bash
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
```

## Access the Server

Once the server starts, you'll see:
```
========================================
ESP32 Sensor Server Started!
========================================
Server running on: http://localhost:8085
Dashboard: http://localhost:8085/
API Base: http://localhost:8085/api/
========================================
```

- **Dashboard**: Open http://localhost:8085/ in your browser
- **API Endpoints**: 
  - POST /api/data - ESP32 posts sensor data
  - GET /api/poll - ESP32 polls for threshold updates
  - GET /api/latest - Get latest sensor data
  - GET/POST /api/thresholds - Get/update thresholds

## Configure ESP32

1. Update WiFi credentials in `sketch_nov9a.ino`:
   ```cpp
   const char* WIFI_SSID = "your-wifi-ssid";
   const char* WIFI_PASSWORD = "your-wifi-password";
   ```

2. For local server (PC hotspot):
   ```cpp
   #define USE_PUBLIC_SERVER false
   ```
   The ESP32 will automatically use the gateway IP as the server.

3. For public server:
   ```cpp
   #define USE_PUBLIC_SERVER true
   const char* SERVER_URL = "https://your-domain.com";
   ```

4. Upload the sketch to ESP32.

## Stopping the Server

Press `Ctrl+C` to stop the server.

## Troubleshooting

### Port Already in Use

If port 8085 is already in use, specify a different port:
```bash
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
```

### Build Errors

Make sure you have Java 11+ and Maven installed:
```bash
java -version
mvn -version
```

### ESP32 Cannot Connect

1. Make sure the server is running
2. Check that ESP32 and server are on the same network
3. For local server, verify the gateway IP matches the server IP
4. Check firewall settings

### Dashboard Not Loading

1. Check server logs for errors
2. Verify the server is running on the correct port
3. Check browser console for JavaScript errors


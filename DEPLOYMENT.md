# Deployment Guide

## Quick Start

### 1. Build the Project

```bash
mvn clean package
```

This will create `target/esp32-server-1.0.0.war`

### 2. Deploy to Tomcat

1. Copy `target/esp32-server-1.0.0.war` to `$CATALINA_HOME/webapps/`
2. Start Tomcat: `$CATALINA_HOME/bin/startup.sh` (Linux/Mac) or `$CATALINA_HOME/bin/startup.bat` (Windows)
3. Access dashboard at: `http://localhost:8085/esp32-server-1.0.0/`

### 3. Configure ESP32

1. Open `sketch_nov9a.ino` in Arduino IDE
2. Update WiFi credentials:
   ```cpp
   const char* WIFI_SSID = "YourWiFiName";
   const char* WIFI_PASSWORD = "YourWiFiPassword";
   ```
3. For local server (PC hotspot):
   ```cpp
   #define USE_PUBLIC_SERVER false
   ```
4. For public server:
   ```cpp
   #define USE_PUBLIC_SERVER true
   const char* SERVER_URL = "https://your-domain.com";
   ```
5. Upload to ESP32

## Server Configuration

### Local Server Setup (PC Hotspot)

1. **Enable PC Hotspot**:
   - Windows: Settings > Network & Internet > Mobile hotspot
   - Mac: System Preferences > Sharing > Internet Sharing
   - Linux: Use `nmcli` or Network Manager

2. **Note the Hotspot IP** (usually 192.168.137.1 on Windows)

3. **Run Server on PC**:
   - Deploy WAR to Tomcat on port 8085
   - Ensure firewall allows port 8085

4. **ESP32 Configuration**:
   - Connect ESP32 to PC hotspot
   - Set `USE_PUBLIC_SERVER false`
   - ESP32 will automatically use gateway IP as server

### Public Server Setup

1. **Deploy to Cloud**:
   - Deploy WAR to cloud servlet container (AWS, Google Cloud, etc.)
   - Configure domain name and SSL certificate

2. **ESP32 Configuration**:
   - Set `USE_PUBLIC_SERVER true`
   - Set `SERVER_URL` to your domain (e.g., "https://esp32.example.com")

## Testing

### Test Server Endpoints

1. **Check Dashboard**:
   ```
   http://localhost:8085/esp32-server-1.0.0/
   ```

2. **Test API** (using curl):
   ```bash
   # Get latest data
   curl http://localhost:8085/esp32-server-1.0.0/api/latest
   
   # Get thresholds
   curl http://localhost:8085/esp32-server-1.0.0/api/thresholds
   
   # Post test data
   curl -X POST http://localhost:8085/esp32-server-1.0.0/api/data \
     -H "Content-Type: application/json" \
     -d '{"temperature":25.5,"humidity":60.0,"mq1":250,"mq2":280,"mq3":500,"dust":45.2,"wifiSignal":-65,"uptime":3600}'
   ```

### Test ESP32 Connection

1. Open Serial Monitor (115200 baud)
2. Check for:
   - "WiFi connected!"
   - "Posting data to: ..."
   - "HTTP Response code: 200"

## Troubleshooting

### ESP32 Cannot Connect

- **Check WiFi**: Ensure ESP32 and server are on same network
- **Check Gateway**: Verify gateway IP matches server IP
- **Check Firewall**: Ensure port 8085 is open
- **Check URL**: Verify server URL is correct

### Server Not Receiving Data

- **Check Logs**: Check Tomcat logs for errors
- **Check CORS**: CORS filter should be working (already configured)
- **Test Manually**: Use curl to test POST endpoint

### Dashboard Not Loading

- **Check Path**: Ensure accessing correct URL
- **Check Browser Console**: Look for JavaScript errors
- **Check Network Tab**: Verify API calls are successful

## Port Configuration

Default port is 8085. To change:

1. **Tomcat**: Edit `$CATALINA_HOME/conf/server.xml`
2. **Update ESP32**: Update server URL with new port

## Security Considerations

- **Production**: Remove CORS wildcard (`*`) and specify allowed origins
- **HTTPS**: Use HTTPS for public servers
- **Authentication**: Add authentication for threshold updates
- **Rate Limiting**: Consider adding rate limiting for API endpoints


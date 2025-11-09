# Build and Run Instructions

## Quick Start

### Method 1: One Command (Recommended)

**Windows:**
```batch
mvn clean compile exec:java
```

**Linux/Mac:**
```bash
mvn clean compile exec:java
```

This will:
1. Clean previous builds
2. Compile the project
3. Start the embedded server on port 8085

### Method 2: Using Run Scripts

**Windows:**
```batch
run.bat
```

**Linux/Mac:**
```bash
chmod +x run.sh
./run.sh
```

### Method 3: Separate Build and Run

**Build:**
```bash
mvn clean compile
```

**Run:**
```bash
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer"
```

## Custom Port

To run on a different port (e.g., 9090):

```bash
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
```

## What Happens When You Run

1. Maven downloads dependencies (first time only)
2. Compiles Java source files
3. Starts embedded Jetty server
4. Server listens on port 8085 (or specified port)
5. Dashboard available at: http://localhost:8085/
6. API endpoints available at: http://localhost:8085/api/

## Server Output

When the server starts successfully, you'll see:

```
========================================
ESP32 Sensor Server Started!
========================================
Server running on: http://localhost:8085
Dashboard: http://localhost:8085/
API Base: http://localhost:8085/api/
Webapp directory: [path]
========================================
Press Ctrl+C to stop the server
========================================
```

## Stopping the Server

Press `Ctrl+C` in the terminal where the server is running.

## Troubleshooting

### Port Already in Use

If you get an error about port 8085 being in use:

1. **Use a different port:**
   ```bash
   mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="9090"
   ```

2. **Or find and stop the process using port 8085:**
   ```bash
   # Linux/Mac
   lsof -i :8085
   kill -9 <PID>
   
   # Windows
   netstat -ano | findstr :8085
   taskkill /PID <PID> /F
   ```

### Build Errors

**Check Java version:**
```bash
java -version
```
Should be Java 11 or higher.

**Check Maven version:**
```bash
mvn -version
```
Should be Maven 3.6 or higher.

**Clean and rebuild:**
```bash
mvn clean
mvn compile
```

### Server Starts But Dashboard Not Loading

1. Check server logs for errors
2. Verify webapp directory exists: `src/main/webapp/WEB-INF/dashboard.html`
3. Check browser console for JavaScript errors
4. Try accessing API directly: http://localhost:8085/api/latest

### Dependencies Not Downloading

**Check internet connection and Maven settings:**
```bash
mvn dependency:resolve
```

**If behind proxy, configure Maven settings:**
Edit `~/.m2/settings.xml` or `%USERPROFILE%\.m2\settings.xml`

## Development Tips

### Running in IDE

1. Open project in your IDE (IntelliJ IDEA, Eclipse, etc.)
2. Run `com.esp32.server.EmbeddedServer` main class
3. Set program arguments for custom port if needed

### Hot Reload

For development with hot reload, use an IDE with hot reload capabilities or use JRebel.

### Debugging

1. Run server in debug mode from IDE
2. Set breakpoints in servlet classes
3. Test API endpoints using curl or Postman

## Production Deployment

For production, you may want to:

1. **Build WAR file:**
   ```bash
   mvn clean package
   ```

2. **Deploy to servlet container:**
   - Tomcat
   - Jetty
   - WildFly
   - etc.

See `DEPLOYMENT.md` for detailed deployment instructions.


# Deploying to Linux Server (No Maven)

## Quick Deploy

On your server (`root@toandz:~/sketch_nov9a#`):

```bash
# Make build script executable
chmod +x build-server.sh

# Run build
./build-server.sh
```

This will:
1. Download all required dependencies
2. Compile all Java source files
3. Create an executable JAR
4. Package everything in `deploy/` directory

## Running the Server

### Option 1: Direct Run
```bash
cd deploy
./start.sh 8080
```

### Option 2: Manual Run
```bash
cd deploy
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer 8080
```

### Option 3: Background Process
```bash
cd deploy
nohup ./start.sh 8080 > server.log 2>&1 &
```

### Option 4: Systemd Service (Recommended for Production)
```bash
# Copy service file
sudo cp deploy/esp32-server.service /etc/systemd/system/

# Reload systemd
sudo systemctl daemon-reload

# Enable service (start on boot)
sudo systemctl enable esp32-server

# Start service
sudo systemctl start esp32-server

# Check status
sudo systemctl status esp32-server

# View logs
sudo journalctl -u esp32-server -f
```

## Deployment Directory Structure

```
deploy/
├── esp32-server.jar          # Main executable JAR
├── lib/                      # All dependencies
│   ├── gson-2.10.1.jar
│   ├── jetty-server-9.4.51.v20230217.jar
│   ├── jetty-servlet-9.4.51.v20230217.jar
│   ├── jetty-webapp-9.4.51.v20230217.jar
│   └── servlet-api-4.0.1.jar
├── webapp/                   # Web application
│   └── WEB-INF/
│       ├── dashboard.html
│       ├── sensor-data.html
│       └── web.xml
├── start.sh                  # Start script
└── esp32-server.service      # Systemd service file
```

## Port Configuration

Default port is 8080. To change:

1. **Command line:**
   ```bash
   ./start.sh 9090
   ```

2. **Systemd service:**
   Edit `/etc/systemd/system/esp32-server.service` and change the port in `ExecStart`

## Firewall Configuration

If you need to open the port:

```bash
# UFW
sudo ufw allow 8080/tcp

# firewalld
sudo firewall-cmd --permanent --add-port=8080/tcp
sudo firewall-cmd --reload

# iptables
sudo iptables -A INPUT -p tcp --dport 8080 -j ACCEPT
```

## Access the Server

Once running, access:
- Dashboard: `http://your-server-ip:8080/`
- Sensor Data: `http://your-server-ip:8080/sensor-data.html`
- API: `http://your-server-ip:8080/api/`

## Troubleshooting

### Build Fails - Dependencies Not Downloading

If `wget` or `curl` fails, manually download dependencies:

```bash
mkdir -p target/lib
cd target/lib

# Download each file manually
wget https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar
wget https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar
wget https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar
wget https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar
wget https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar

cd ../..
./build-server.sh
```

### Compilation Errors

Check Java version:
```bash
java -version
```

Should be Java 11 or higher. If not, install JDK:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install openjdk-11-jdk

# CentOS/RHEL
sudo yum install java-11-openjdk-devel
```

### Port Already in Use

Check what's using port 8080:
```bash
sudo netstat -tulpn | grep 8080
# or
sudo lsof -i :8080
```

Kill the process or use a different port.

### Server Won't Start

Check logs:
```bash
# If running directly
cd deploy
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer 8080

# If using systemd
sudo journalctl -u esp32-server -n 50
```

## Updating the Server

To update after code changes:

```bash
# Rebuild
./build-server.sh

# Restart service (if using systemd)
sudo systemctl restart esp32-server

# Or stop old process and start new one
pkill -f EmbeddedServer
cd deploy
./start.sh 8080 &
```


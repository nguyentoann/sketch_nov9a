#!/bin/bash

# Build script for ESP32 Server (Linux Server - No Maven)
# Run this on your server: root@toandz:~/sketch_nov9a#

set -e  # Exit on error

echo "=========================================="
echo "Building ESP32 Sensor Server"
echo "=========================================="

# Check Java version
echo "Checking Java version..."
java -version

# Create directories
echo "Creating directories..."
mkdir -p target/classes
mkdir -p target/lib
mkdir -p target/webapp/WEB-INF
mkdir -p deploy

# Download dependencies if not present
echo "Checking dependencies..."
if [ ! -f "target/lib/gson-2.10.1.jar" ]; then
    echo "Downloading dependencies from Maven Central..."
    cd target/lib
    
    echo "  - Gson..."
    wget -q https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar || \
    curl -L -o gson-2.10.1.jar https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar
    
    echo "  - Jetty Server..."
    wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar || \
    curl -L -o jetty-server-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar
    
    echo "  - Jetty Servlet..."
    wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar || \
    curl -L -o jetty-servlet-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar
    
    echo "  - Jetty Webapp..."
    wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar || \
    curl -L -o jetty-webapp-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar
    
    echo "  - Servlet API..."
    wget -q https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar || \
    curl -L -o servlet-api-4.0.1.jar https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar
    
    cd ../..
    echo "Dependencies downloaded successfully!"
else
    echo "Dependencies already present, skipping download."
fi

# Build classpath
CLASSPATH="target/lib/servlet-api-4.0.1.jar:target/lib/gson-2.10.1.jar:target/lib/jetty-server-9.4.51.v20230217.jar:target/lib/jetty-servlet-9.4.51.v20230217.jar:target/lib/jetty-webapp-9.4.51.v20230217.jar"

# Find all Java files
echo "Finding Java source files..."
JAVA_FILES=$(find src/main/java -name "*.java" | tr '\n' ' ')

if [ -z "$JAVA_FILES" ]; then
    echo "ERROR: No Java files found in src/main/java/"
    exit 1
fi

# Compile Java sources
echo "Compiling Java sources..."
javac -d target/classes -cp "$CLASSPATH" -sourcepath src/main/java $JAVA_FILES

if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed!"
    exit 1
fi

echo "Compilation successful!"

# Copy webapp resources
echo "Copying webapp resources..."
cp -r src/main/webapp/* target/webapp/ 2>/dev/null || true

# Create JAR manifest
echo "Creating JAR manifest..."
mkdir -p target/META-INF
cat > target/META-INF/MANIFEST.MF << 'EOF'
Manifest-Version: 1.0
Main-Class: com.esp32.server.EmbeddedServer
Class-Path: lib/gson-2.10.1.jar lib/jetty-server-9.4.51.v20230217.jar lib/jetty-servlet-9.4.51.v20230217.jar lib/jetty-webapp-9.4.51.v20230217.jar lib/servlet-api-4.0.1.jar
EOF

# Create executable JAR
echo "Creating executable JAR..."
cd target
jar cfm esp32-server.jar META-INF/MANIFEST.MF -C classes .
cd ..

# Create deployment package
echo "Creating deployment package..."
rm -rf deploy/*
cp target/esp32-server.jar deploy/
cp -r target/lib deploy/
cp -r target/webapp deploy/

# Create run script
cat > deploy/start.sh << 'RUNSCRIPT'
#!/bin/bash
PORT=${1:-8080}
cd "$(dirname "$0")"
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer $PORT
RUNSCRIPT

chmod +x deploy/start.sh

# Create systemd service file (optional)
cat > deploy/esp32-server.service << 'SERVICEFILE'
[Unit]
Description=ESP32 Sensor Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/root/sketch_nov9a/deploy
ExecStart=/usr/bin/java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer 8080
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
SERVICEFILE

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Deployment package created in: deploy/"
echo ""
echo "Files:"
echo "  - esp32-server.jar (main executable)"
echo "  - lib/ (dependencies)"
echo "  - webapp/ (web files)"
echo "  - start.sh (run script)"
echo ""
echo "To run the server:"
echo "  cd deploy"
echo "  ./start.sh [port]"
echo ""
echo "Or manually:"
echo "  cd deploy"
echo "  java -cp 'esp32-server.jar:lib/*' com.esp32.server.EmbeddedServer 8080"
echo ""
echo "To install as systemd service (optional):"
echo "  sudo cp deploy/esp32-server.service /etc/systemd/system/"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable esp32-server"
echo "  sudo systemctl start esp32-server"
echo ""


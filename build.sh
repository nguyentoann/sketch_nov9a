#!/bin/bash

# Build script for ESP32 Server (without Maven)
# This script compiles and packages the project using only Java

echo "=========================================="
echo "Building ESP32 Sensor Server"
echo "=========================================="

# Create directories
mkdir -p target/classes
mkdir -p target/lib
mkdir -p target/webapp/WEB-INF

# Download dependencies if not present
if [ ! -f "target/lib/gson-2.10.1.jar" ]; then
    echo "Downloading dependencies..."
    mkdir -p target/lib
    cd target/lib
    
    echo "Downloading Gson..."
    curl -L -o gson-2.10.1.jar https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar
    
    echo "Downloading Servlet API..."
    curl -L -o servlet-api-4.0.1.jar https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar
    
    echo "Downloading Jetty Util..."
    curl -L -o jetty-util-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util/9.4.51.v20230217/jetty-util-9.4.51.v20230217.jar
    
    echo "Downloading Jetty IO..."
    curl -L -o jetty-io-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-io/9.4.51.v20230217/jetty-io-9.4.51.v20230217.jar
    
    echo "Downloading Jetty HTTP..."
    curl -L -o jetty-http-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-http/9.4.51.v20230217/jetty-http-9.4.51.v20230217.jar
    
    echo "Downloading Jetty XML..."
    curl -L -o jetty-xml-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-xml/9.4.51.v20230217/jetty-xml-9.4.51.v20230217.jar
    
    echo "Downloading Jetty Server..."
    curl -L -o jetty-server-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar
    
    echo "Downloading Jetty Servlet..."
    curl -L -o jetty-servlet-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar
    
    echo "Downloading Jetty Webapp..."
    curl -L -o jetty-webapp-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar
    
    echo "Downloading Jetty Util Ajax..."
    curl -L -o jetty-util-ajax-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util-ajax/9.4.51.v20230217/jetty-util-ajax-9.4.51.v20230217.jar
    
    cd ../..
fi

# Build classpath (include all Jetty dependencies)
CLASSPATH="target/lib/servlet-api-4.0.1.jar:target/lib/gson-2.10.1.jar:target/lib/jetty-util-9.4.51.v20230217.jar:target/lib/jetty-io-9.4.51.v20230217.jar:target/lib/jetty-http-9.4.51.v20230217.jar:target/lib/jetty-xml-9.4.51.v20230217.jar:target/lib/jetty-server-9.4.51.v20230217.jar:target/lib/jetty-servlet-9.4.51.v20230217.jar:target/lib/jetty-webapp-9.4.51.v20230217.jar:target/lib/jetty-util-ajax-9.4.51.v20230217.jar"

# Find all Java files
JAVA_FILES=$(find src/main/java -name "*.java")

echo "Compiling Java sources..."
javac -d target/classes -cp "$CLASSPATH" -sourcepath src/main/java $JAVA_FILES

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Copying webapp resources..."
cp -r src/main/webapp/* target/webapp/ 2>/dev/null || true

echo "Creating JAR manifest..."
mkdir -p target/META-INF
cat > target/META-INF/MANIFEST.MF << EOF
Manifest-Version: 1.0
Main-Class: com.esp32.server.EmbeddedServer
Class-Path: lib/servlet-api-4.0.1.jar lib/gson-2.10.1.jar lib/jetty-util-9.4.51.v20230217.jar lib/jetty-io-9.4.51.v20230217.jar lib/jetty-http-9.4.51.v20230217.jar lib/jetty-xml-9.4.51.v20230217.jar lib/jetty-server-9.4.51.v20230217.jar lib/jetty-servlet-9.4.51.v20230217.jar lib/jetty-webapp-9.4.51.v20230217.jar lib/jetty-util-ajax-9.4.51.v20230217.jar
EOF

echo "Creating executable JAR..."
cd target
jar cfm esp32-server.jar META-INF/MANIFEST.MF -C classes .
cd ..

echo "Creating deployment package..."
mkdir -p deploy
cp target/esp32-server.jar deploy/
cp -r target/lib deploy/
cp -r target/webapp deploy/

# Create run script
cat > deploy/run.sh << 'RUNSCRIPT'
#!/bin/bash
cd "$(dirname "$0")"
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer "$@"
RUNSCRIPT

chmod +x deploy/run.sh

# Create run script with port option
cat > deploy/start.sh << 'RUNSCRIPT'
#!/bin/bash
PORT=${1:-8085}
cd "$(dirname "$0")"
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer $PORT
RUNSCRIPT

chmod +x deploy/start.sh

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Deployment files are in: deploy/"
echo ""
echo "To run the server:"
echo "  cd deploy"
echo "  ./start.sh [port]"
echo ""
echo "Or:"
echo "  java -cp 'esp32-server.jar:lib/*' com.esp32.server.EmbeddedServer [port]"
echo ""


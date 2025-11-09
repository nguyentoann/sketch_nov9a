#!/bin/bash

echo "========================================"
echo "Building ESP32 Sensor Server"
echo "========================================"
mvn clean compile

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "========================================"
echo "Starting ESP32 Sensor Server"
echo "========================================"
echo ""
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="8085"


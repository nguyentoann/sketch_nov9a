ESP32 Sensor Server - Deployment Package
==========================================

This package contains everything needed to run the server.

Contents:
  - esp32-server.jar    : Main executable JAR
  - lib\                : All required dependencies
  - webapp\             : Web application files
  - start.sh            : Start script for Linux
  - esp32-server.service : Systemd service file

To deploy on server:
  1. Transfer this entire directory to your server
  2. On server, run: chmod +x start.sh
  3. Run: ./start.sh 8085

Or install as systemd service:
  sudo cp esp32-server.service /etc/systemd/system/
  sudo systemctl daemon-reload
  sudo systemctl enable esp32-server
  sudo systemctl start esp32-server


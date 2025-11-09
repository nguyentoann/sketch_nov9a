@echo off
REM Package deployment files for server transfer

echo ==========================================
echo Packaging for Server Transfer
echo ==========================================

REM Create server package directory
if exist "server-package" rmdir /s /q server-package
mkdir server-package

REM Copy all deployment files
echo Copying deployment files...
xcopy /E /I /Y deploy\* server-package\ >nul

REM Create Linux start script
echo Creating Linux start script...
(
echo #!/bin/bash
echo PORT=${1:-8085}
echo cd "$(dirname "$0")"
echo java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer $PORT
) > server-package\start.sh

REM Create systemd service file
echo Creating systemd service file...
(
echo [Unit]
echo Description=ESP32 Sensor Server
echo After=network.target
echo.
echo [Service]
echo Type=simple
echo User=root
echo WorkingDirectory=/root/sketch_nov9a/server-package
echo ExecStart=/usr/bin/java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer 8085
echo Restart=always
echo RestartSec=10
echo.
echo [Install]
echo WantedBy=multi-user.target
) > server-package\esp32-server.service

REM Create README for server
(
echo ESP32 Sensor Server - Deployment Package
echo ==========================================
echo.
echo This package contains everything needed to run the server.
echo.
echo Contents:
echo   - esp32-server.jar    : Main executable JAR
echo   - lib\                : All required dependencies
echo   - webapp\             : Web application files
echo   - start.sh            : Start script for Linux
echo   - esp32-server.service : Systemd service file
echo.
echo To deploy on server:
echo   1. Transfer this entire directory to your server
echo   2. On server, run: chmod +x start.sh
echo   3. Run: ./start.sh 8085
echo.
echo Or install as systemd service:
echo   sudo cp esp32-server.service /etc/systemd/system/
echo   sudo systemctl daemon-reload
echo   sudo systemctl enable esp32-server
echo   sudo systemctl start esp32-server
echo.
) > server-package\README.txt

REM Create tar.gz archive (if tar is available)
where tar >nul 2>&1
if %errorlevel% equ 0 (
    echo Creating tar.gz archive...
    tar -czf esp32-server-deploy.tar.gz server-package
    echo.
    echo Archive created: esp32-server-deploy.tar.gz
    echo   (Extracts to: server-package/)
    echo.
) else (
    echo.
    echo Note: tar not found. Creating ZIP archive instead...
    powershell -Command "Compress-Archive -Path server-package -DestinationPath esp32-server-deploy.zip -Force"
    echo.
    echo Archive created: esp32-server-deploy.zip
    echo   (Extracts to: server-package/)
    echo.
)

echo ==========================================
echo Package ready for server transfer!
echo ==========================================
echo.
echo Files to transfer:
echo   Option 1: Transfer entire 'server-package' folder
echo   Option 2: Transfer archive (esp32-server-deploy.tar.gz or .zip)
echo.
echo Transfer methods:
echo   1. SCP: scp -r server-package root@your-server:/root/
echo   2. SFTP: Use FileZilla or WinSCP
echo   3. Archive: scp esp32-server-deploy.tar.gz root@your-server:/root/
echo.
pause


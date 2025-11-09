@echo off
echo ========================================
echo Building ESP32 Sensor Server
echo ========================================
call mvn clean compile

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo Starting ESP32 Sensor Server
echo ========================================
echo.
mvn exec:java -Dexec.mainClass="com.esp32.server.EmbeddedServer" -Dexec.args="8085"

pause


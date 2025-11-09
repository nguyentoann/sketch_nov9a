@echo off
set PORT=%1
if "%PORT%"=="" set PORT=8085
cd /d "%~dp0"
java -cp "esp32-server.jar;lib\*" com.esp32.server.EmbeddedServer %PORT%

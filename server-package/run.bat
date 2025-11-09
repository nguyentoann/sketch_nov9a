@echo off
cd /d "%~dp0"
java -cp "esp32-server.jar;lib\*" com.esp32.server.EmbeddedServer %*

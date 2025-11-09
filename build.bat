@echo off
REM Build script for ESP32 Server (Windows, without Maven)

echo ==========================================
echo Building ESP32 Sensor Server
echo ==========================================

REM Create directories
if not exist "target\classes" mkdir target\classes
if not exist "target\lib" mkdir target\lib
if not exist "target\webapp\WEB-INF" mkdir target\webapp\WEB-INF

REM Download dependencies if not present
if not exist "target\lib\gson-2.10.1.jar" (
    echo Downloading dependencies...
    cd target\lib
    
    echo Downloading Gson...
    curl -L -o gson-2.10.1.jar https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar
    
    echo Downloading Servlet API...
    curl -L -o servlet-api-4.0.1.jar https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar
    
    echo Downloading Jetty Util...
    curl -L -o jetty-util-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util/9.4.51.v20230217/jetty-util-9.4.51.v20230217.jar
    
    echo Downloading Jetty IO...
    curl -L -o jetty-io-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-io/9.4.51.v20230217/jetty-io-9.4.51.v20230217.jar
    
    echo Downloading Jetty HTTP...
    curl -L -o jetty-http-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-http/9.4.51.v20230217/jetty-http-9.4.51.v20230217.jar
    
    echo Downloading Jetty XML...
    curl -L -o jetty-xml-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-xml/9.4.51.v20230217/jetty-xml-9.4.51.v20230217.jar
    
    echo Downloading Jetty Server...
    curl -L -o jetty-server-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar
    
    echo Downloading Jetty Servlet...
    curl -L -o jetty-servlet-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar
    
    echo Downloading Jetty Webapp...
    curl -L -o jetty-webapp-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar
    
    echo Downloading Jetty Util Ajax...
    curl -L -o jetty-util-ajax-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util-ajax/9.4.51.v20230217/jetty-util-ajax-9.4.51.v20230217.jar
    
    cd ..\..
)

REM Build classpath (include all Jetty dependencies)
set CLASSPATH=target\lib\servlet-api-4.0.1.jar;target\lib\gson-2.10.1.jar;target\lib\jetty-util-9.4.51.v20230217.jar;target\lib\jetty-io-9.4.51.v20230217.jar;target\lib\jetty-http-9.4.51.v20230217.jar;target\lib\jetty-xml-9.4.51.v20230217.jar;target\lib\jetty-server-9.4.51.v20230217.jar;target\lib\jetty-servlet-9.4.51.v20230217.jar;target\lib\jetty-webapp-9.4.51.v20230217.jar;target\lib\jetty-util-ajax-9.4.51.v20230217.jar

echo Compiling Java sources...
for /r src\main\java %%f in (*.java) do (
    javac -d target\classes -cp "%CLASSPATH%" "%%f"
    if errorlevel 1 (
        echo Compilation failed!
        exit /b 1
    )
)

echo Copying webapp resources...
xcopy /E /I /Y src\main\webapp\* target\webapp\ >nul

echo Creating JAR manifest...
if not exist "target\META-INF" mkdir target\META-INF
(
echo Manifest-Version: 1.0
echo Main-Class: com.esp32.server.EmbeddedServer
echo Class-Path: lib/servlet-api-4.0.1.jar lib/gson-2.10.1.jar lib/jetty-util-9.4.51.v20230217.jar lib/jetty-io-9.4.51.v20230217.jar lib/jetty-http-9.4.51.v20230217.jar lib/jetty-xml-9.4.51.v20230217.jar lib/jetty-server-9.4.51.v20230217.jar lib/jetty-servlet-9.4.51.v20230217.jar lib/jetty-webapp-9.4.51.v20230217.jar lib/jetty-util-ajax-9.4.51.v20230217.jar
) > target\META-INF\MANIFEST.MF

echo Creating executable JAR...
cd target
jar cfm esp32-server.jar META-INF\MANIFEST.MF -C classes .
cd ..

echo Creating deployment package...
if not exist "deploy" mkdir deploy
copy target\esp32-server.jar deploy\ >nul
xcopy /E /I /Y target\lib deploy\lib\ >nul
xcopy /E /I /Y target\webapp deploy\webapp\ >nul

REM Create run script
(
echo @echo off
echo cd /d "%%~dp0"
echo java -cp "esp32-server.jar;lib\*" com.esp32.server.EmbeddedServer %%*
) > deploy\run.bat

(
echo @echo off
echo set PORT=%%1
echo if "%%PORT%%"=="" set PORT=8085
echo cd /d "%%~dp0"
echo java -cp "esp32-server.jar;lib\*" com.esp32.server.EmbeddedServer %%PORT%%
) > deploy\start.bat

echo.
echo ==========================================
echo Build completed successfully!
echo ==========================================
echo.
echo Deployment files are in: deploy\
echo.
echo To run the server:
echo   cd deploy
echo   start.bat [port]
echo.
pause


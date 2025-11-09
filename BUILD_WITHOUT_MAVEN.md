# Building Without Maven

This project can be built using only Java (no Maven required).

## Quick Build

### Linux/Mac:
```bash
chmod +x build.sh
./build.sh
```

### Windows:
```batch
build.bat
```

## What the Build Script Does

1. **Downloads Dependencies**: Automatically downloads required JAR files from Maven Central
2. **Compiles Java**: Compiles all Java source files
3. **Copies Resources**: Copies webapp files (HTML, etc.)
4. **Creates JAR**: Packages everything into an executable JAR
5. **Creates Deployment Package**: Creates a `deploy/` directory with everything needed

## Deployment Package Structure

```
deploy/
├── esp32-server.jar          # Main executable JAR
├── lib/                      # All required dependencies
│   ├── gson-2.10.1.jar
│   ├── jetty-server-9.4.51.v20230217.jar
│   ├── jetty-servlet-9.4.51.v20230217.jar
│   ├── jetty-webapp-9.4.51.v20230217.jar
│   └── servlet-api-4.0.1.jar
├── webapp/                   # Web application files
│   └── WEB-INF/
│       ├── dashboard.html
│       ├── sensor-data.html
│       └── web.xml
├── start.sh                  # Start script (Linux/Mac)
└── run.sh                    # Run script (Linux/Mac)
```

## Running the Server

### Linux/Mac:
```bash
cd deploy
./start.sh 8085
```

### Windows:
```batch
cd deploy
start.bat 8085
```

### Manual (Any OS):
```bash
cd deploy
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer 8085
```

## Requirements

- Java 11 or higher
- `curl` or `wget` (for downloading dependencies)
- `jar` command (usually included with JDK)

## Troubleshooting

### Dependencies Not Downloading

If the build script can't download dependencies, you can manually download them:

1. Create `target/lib/` directory
2. Download these files:
   - https://repo1.maven.org/maven2/com/google/code/gson/gson/2.10.1/gson-2.10.1.jar
   - https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-server/9.4.51.v20230217/jetty-server-9.4.51.v20230217.jar
   - https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-servlet/9.4.51.v20230217/jetty-servlet-9.4.51.v20230217.jar
   - https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-webapp/9.4.51.v20230217/jetty-webapp-9.4.51.v20230217.jar
   - https://repo1.maven.org/maven2/javax/servlet/javax.servlet-api/4.0.1/javax.servlet-api-4.0.1.jar

3. Run the build script again

### Compilation Errors

Make sure you have Java 11 or higher:
```bash
java -version
```

### Missing jar Command

The `jar` command is part of the JDK. Make sure you have JDK installed, not just JRE.

## Alternative: Using Maven Wrapper

If you prefer Maven, you can use the Maven Wrapper:

```bash
# Download Maven Wrapper (one time)
curl -L https://maven.apache.org/wrapper/maven-wrapper.jar -o .mvn/wrapper/maven-wrapper.jar

# Then use: ./mvnw instead of mvn
```

But the build scripts provided work without Maven!


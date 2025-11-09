# Quick Fix for Missing jetty-security on Server

## Option 1: Quick Fix (Download Missing JAR on Server)

On your server, run:

```bash
cd /root/sketch_nov9a/server-package/lib

# Download the missing dependency
wget https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-security/9.4.51.v20230217/jetty-security-9.4.51.v20230217.jar

# Or using curl
curl -L -o jetty-security-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-security/9.4.51.v20230217/jetty-security-9.4.51.v20230217.jar

# Go back and try running again
cd ..
./start.sh 8085
```

## Option 2: Transfer Updated Package

I've rebuilt the package with the missing dependency. Transfer the new package:

**On Windows:**
```bash
scp esp32-server-deploy.tar.gz root@toandz.id.vn:/root/sketch_nov9a/
```

**On Server:**
```bash
cd /root/sketch_nov9a
rm -rf server-package
tar -xzf esp32-server-deploy.tar.gz
cd server-package
chmod +x start.sh
./start.sh 8085
```

## What Was Missing

The `jetty-security` JAR was missing. This is required by `jetty-webapp` for security handling.


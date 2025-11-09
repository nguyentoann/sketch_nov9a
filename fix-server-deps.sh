#!/bin/bash
# Quick fix script to download missing jetty-security dependency on server

echo "Downloading missing jetty-security dependency..."

cd lib

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-security/9.4.51.v20230217/jetty-security-9.4.51.v20230217.jar || \
curl -L -o jetty-security-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-security/9.4.51.v20230217/jetty-security-9.4.51.v20230217.jar

if [ -f "jetty-security-9.4.51.v20230217.jar" ]; then
    echo "✓ jetty-security downloaded successfully!"
    echo ""
    echo "Now try running the server again:"
    echo "  ./start.sh 8085"
else
    echo "✗ Failed to download jetty-security"
    exit 1
fi

cd ..


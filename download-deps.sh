#!/bin/bash
# Quick script to download missing Jetty dependencies

cd target/lib

echo "Downloading missing Jetty dependencies..."

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util/9.4.51.v20230217/jetty-util-9.4.51.v20230217.jar || \
curl -L -o jetty-util-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util/9.4.51.v20230217/jetty-util-9.4.51.v20230217.jar

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-io/9.4.51.v20230217/jetty-io-9.4.51.v20230217.jar || \
curl -L -o jetty-io-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-io/9.4.51.v20230217/jetty-io-9.4.51.v20230217.jar

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-http/9.4.51.v20230217/jetty-http-9.4.51.v20230217.jar || \
curl -L -o jetty-http-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-http/9.4.51.v20230217/jetty-http-9.4.51.v20230217.jar

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-xml/9.4.51.v20230217/jetty-xml-9.4.51.v20230217.jar || \
curl -L -o jetty-xml-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-xml/9.4.51.v20230217/jetty-xml-9.4.51.v20230217.jar

wget -q https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util-ajax/9.4.51.v20230217/jetty-util-ajax-9.4.51.v20230217.jar || \
curl -L -o jetty-util-ajax-9.4.51.v20230217.jar https://repo1.maven.org/maven2/org/eclipse/jetty/jetty-util-ajax/9.4.51.v20230217/jetty-util-ajax-9.4.51.v20230217.jar

cd ../..
echo "Dependencies downloaded!"


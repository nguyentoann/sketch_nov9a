#!/bin/bash
PORT=${1:-8085}
cd "$(dirname "$0")"
java -cp "esp32-server.jar:lib/*" com.esp32.server.EmbeddedServer $PORT


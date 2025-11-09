# Transfer Compiled Files to Server

## Build Completed Successfully! ✅

The server has been compiled and packaged on your Windows machine.

## Package Location

- **Archive**: `esp32-server-deploy.tar.gz` (in project root)
- **Folder**: `server-package/` (in project root)

## Transfer to Server

### Option 1: Using SCP (Recommended)

From your Windows machine (using PowerShell or Git Bash):

```bash
# Transfer the archive
scp esp32-server-deploy.tar.gz root@toandz.id.vn:/root/sketch_nov9a/

# Or transfer the entire folder
scp -r server-package root@toandz.id.vn:/root/sketch_nov9a/
```

### Option 2: Using SFTP (WinSCP, FileZilla)

1. Connect to your server: `root@toandz.id.vn`
2. Navigate to `/root/sketch_nov9a/`
3. Upload `esp32-server-deploy.tar.gz` or the entire `server-package` folder

### Option 3: Manual Copy

1. Copy `server-package` folder to a USB drive or network share
2. On server, copy it to `/root/sketch_nov9a/`

## On Your Server

After transferring, SSH into your server and run:

```bash
# If you transferred the archive
cd /root/sketch_nov9a
tar -xzf esp32-server-deploy.tar.gz

# Or if you transferred the folder, it's already extracted
cd server-package

# Make start script executable
chmod +x start.sh

# Run the server
./start.sh 8085
```

## Quick Start on Server

```bash
# Navigate to deployment directory
cd /root/sketch_nov9a/server-package

# Make executable
chmod +x start.sh

# Start server on port 8085
./start.sh 8085
```

## Install as Systemd Service (Optional)

```bash
cd /root/sketch_nov9a/server-package

# Copy service file
sudo cp esp32-server.service /etc/systemd/system/

# Reload systemd
sudo systemctl daemon-reload

# Enable and start
sudo systemctl enable esp32-server
sudo systemctl start esp32-server

# Check status
sudo systemctl status esp32-server
```

## What's Included

- `esp32-server.jar` - Main executable
- `lib/` - All dependencies (9 JAR files)
- `webapp/` - Dashboard and sensor data pages
- `start.sh` - Start script
- `esp32-server.service` - Systemd service file

## Verify Build

The JAR file should be in:
- `deploy/esp32-server.jar` (local)
- `server-package/esp32-server.jar` (for transfer)

Size should be around 10-20 KB (classes only, dependencies are separate).


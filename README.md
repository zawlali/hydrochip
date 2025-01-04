# Hydrochip - ESP32 Plant Monitoring System

An automated plant monitoring and watering system using ESP32, moisture sensors, and DHT11 temperature/humidity sensor. The system connects to Blynk IoT platform for remote monitoring and control.

## Features

- Soil moisture monitoring
- Temperature and humidity monitoring
- Automated watering system
- Remote control via Blynk app
- OTA (Over-The-Air) updates support

## Hardware Requirements

- ESP32 Development Board
- Soil Moisture Sensor
- DHT11 Temperature/Humidity Sensor
- Water Pump
- Relay Module
- Power Supply

## Software Setup (Windows)

### 1. Install Visual Studio Code
1. Download VSCode from [https://code.visualstudio.com/](https://code.visualstudio.com/)
2. Run the installer and follow the installation wizard
3. Launch VSCode after installation

### 2. Install Git
1. Download Git from [https://git-scm.com/download/win](https://git-scm.com/download/win)
2. Run the installer and use the default settings
3. Open Command Prompt or PowerShell to verify installation:
   ```
   git --version
   ```

### 3. Clone the Repository
1. Open Command Prompt or PowerShell
2. Navigate to your desired directory
3. Clone the repository:
   ```
   git clone https://github.com/yourusername/hydrochip.git
   ```
4. Open the project in VSCode:
   ```
   code hydrochip
   ```

### 4. Install PlatformIO Extension
1. Open VSCode
2. Click on the Extensions icon in the left sidebar (or press Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click "Install"
5. Wait for the installation to complete and reload VSCode when prompted

### 5. Find ESP32 COM Port
1. Connect your ESP32 to your computer via USB
2. Open Device Manager (Right-click Start → Device Manager)
3. Expand "Ports (COM & LPT)"
4. Look for "Silicon Labs CP210x USB to UART Bridge (COM X)" or similar
5. Note the COM port number (e.g., COM3)

### 6. Configure platformio.ini
1. Open your project in VSCode
2. Locate the `platformio.ini` file
3. Update the following lines:
    - upload_port = COMX ; Replace X with your COM port number
    - monitor_port = COMX ; Replace X with your COM port number

### 7. Configure WiFi Settings
1. Open `src/main.ino`
2. Locate these lines near the top:
    - #define WF_SSID "OPPO_8E9986_2.4G" // Replace with your WiFi name
    - #define PASS "UEPFE4Vd" // Replace with your WiFi password
3. Replace the placeholder values with your actual WiFi credentials

## Building and Uploading

1. Open the project in VSCode
2. Click the PlatformIO icon in the left sidebar
3. Under "Project Tasks", expand "esp32doit-devkit-v1"
4. Click "Build" to compile the project
5. Click "Upload" to flash the ESP32
# Walk With Me: Smart Following Shopping Cart

## Project Overview
"Walk With Me" is a smart shopping cart system that autonomously follows the user using UWB-based positioning, performs obstacle avoidance using ultrasonic sensors, and integrates RF communication and barcode recognition.  
The system is designed to assist users who have difficulty pushing carts (e.g., elderly or disabled) and aims to support unmanned retail environments by automating both navigation and checkout.

This project was submitted to the 2025 ECE IF Exhibition at the University of Seoul.

## Key Features
- UWB (DWM1000)-based user positioning and 12-zone classification for dynamic tracking  
- Adaptive motor control logic based on zone alignment and distance regulation  
- Ultrasonic sensor-based obstacle detection and avoidance using interrupt-driven maneuvers  
- Barcode scanning and weight sensor integration, with real-time display on OLED and LCD  
- RF communication between modules for reliable wireless coordination  

## Project Structure
```
ECE_IF-main/
├── Arduino/
│   ├── motor_control.ino           # Motor control based on user positioning
│   ├── uwb_tracking.ino            # UWB-based distance tracking and zone classification
│   ├── ultrasonic_avoidance.ino    # Obstacle detection and avoidance logic
│   └── display_control.ino         # OLED/LCD output and barcode interaction
├── Images/
│   └── system_diagram.jpg          # System diagram
└── README.md                       # This file
```

## Hardware Components
| Component            | Description                                      |
|----------------------|--------------------------------------------------|
| DWM1000 UWB module   | Measures user distance using TWR-based ranging   |
| HC-SR04 × 4          | Ultrasonic sensors for obstacle detection        |
| Barcode Scanner      | Scans product barcodes                           |
| HX711 + Load Cell    | Measures product weight                          |
| OLED / LCD           | Displays product and cart status                 |
| L298N + DC Motors    | Controls cart movement                           |
| RF Transceiver       | Wireless module communication (e.g., nRF24L01)   |
| Arduino Uno / Mega   | Main microcontroller for system control          |

## Development Environment
- **Arduino IDE**: v1.8.19 or later  
- **Board Manager Version**: AVR boards (1.8.3)  
- **Upload Speed**: 115200 baud  

## Required Libraries
Install the following libraries using the Arduino Library Manager:
- Adafruit_GFX  
- Adafruit_SSD1306  
- SPI  
- Wire  
- HX711  
- SoftwareSerial  

## Setup & Execution
1. Clone or download the `ECE_IF-main` directory into your Arduino projects folder.  
2. Open the desired `.ino` sketch file (e.g., `motor_control.ino`) in the Arduino IDE.  
3. Connect your Arduino board and set the correct COM port.  
4. Compile and upload the code to the board.  
5. Power the hardware and ensure all modules are connected properly.  

## References
- Byeongjun Jang (2022), *Principles and Trends of UWB Positioning Technology*, KIEES.  
- STMicroelectronics, *L298N Motor Driver Datasheet*.  
- J. Morgan (2014), *Overview of HC-SR04 Ultrasonic Sensor*.  

## Contributors
- Yubin Wang  
- Seunghyun Bae  
- Kyuhyun Song  
- Seongyeon Moon  

Department of Electrical, Electronic and Computer Engineering  
University of Seoul

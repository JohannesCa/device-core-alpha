# device-core-alpha

## 🚪 Device Core Alpha - Arduino Project

This project uses [`arduino-cli`](https://arduino.github.io/arduino-cli/latest/) and a Makefile to streamline building and uploading code to an ESP8266 board (NodeMCU v2).

It is designed to control a lock device plugged on pin `D7` and a buzzer to indicate boot on pin `D7`.

---

### 📦 Prerequisites

- **Arduino CLI** (can be installed via the Makefile)
- **ESP8266 Board URL** for core installation
- **USB access** to your Arduino-compatible board (e.g., NodeMCU v2)
- Board connected to `/dev/ttyUSB0`

---

### 🧰 Setup

#### 1. Enable Access to Serial Port

Allow read/write access to the USB port:

```bash
make enable-port
```

> You might want to add your user to the `dialout` group for permanent access:
> `sudo usermod -aG dialout $USER && newgrp dialout`

---

#### 2. Install Arduino CLI

Install `arduino-cli` locally:

```bash
make download-cli
```

---

#### 3. Install ESP8266 Core

Install and update the board definitions using the ESP8266 core URL:

```bash
make install
```

---

### 🔗 Attach Board

Tell Arduino CLI which board is connected and where:

```bash
make attach
```

> This step links `/dev/ttyUSB0` to an `esp8266:nodemcuv2` board.

---

### 🏗️ Build the Project

Compile the `.ino` sketch:

```bash
make build
```

---

### 🚀 Upload to Board

Upload the compiled firmware to your ESP8266 board:

```bash
make upload
```


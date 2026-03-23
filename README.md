
# Morse Code LoRa Walkie-Talkie

![Morse Code Walkie Talkie](https://raw.githubusercontent.com/guidebee/esp32morse/refs/heads/master/images/1643360782152.jfif)

A battery-powered Morse code walkie-talkie built on the **TTGO LoRa32 v1** (ESP32). Type messages in Morse code using a physical key, transmit over LoRa radio, and receive messages on an OLED display — with multi-device support, AES encryption, and configurable channels.

[Watch Demo on YouTube](https://www.youtube.com/watch?v=TG7tIteto1E)

---

## Hardware

- **Board**: TTGO LoRa32 v1 (ESP32 + SX127x LoRa + SSD1306 OLED)
- **Framework**: PlatformIO / Arduino (C++)
- **Pins**:
  - `GPIO 35` — Battery level ADC
  - `GPIO 33` — Power switch
  - `GPIO 12` — Receiver LED
  - `GPIO 13` — Bluetooth LED

---

## Features

- **Morse code input** via physical key — short press = dit (`.`), long press = dah (`-`)
- **LoRa radio** for long-range communication (up to several kilometers)
- **OLED display** split into: scrolling top screen (received messages), bottom screen (composing), top bar (uptime/title), status bar (device/signal info + battery indicator)
- **Device discovery** — auto-tracks nearby nodes by chip ID and assigns short indexes
- **AES encryption** for secure message transmission
- **Multi-channel support** — configurable channel ID and LoRa sync word
- **Buzzer feedback** — audible dit/dah tones, message sent/received sounds, error beeps
- **BLE toggle** — optional Bluetooth support
- **Battery level indicator** in the status bar corner
- **Deep sleep support** (optional, disabled by default)

---

## Controls

| Button | Action |
|--------|--------|
| **Main** (short press) | Dit (`.`) |
| **Main** (long press) | Dah (`-`) |
| **Left** | Backspace (delete last character) |
| **Right** | Insert space |
| **OK** | Send message (if composing), or enter Options menu (press 3× when idle) |

Max message length is 36 characters — long messages are auto-sent when the limit is reached.

---

## Options Menu

Press **OK** three times while idle to enter the settings menu. Navigate with Left/Right, confirm with OK.

| Option | Description |
|--------|-------------|
| Device Name | Set a display name for this node |
| Play Sound | Toggle buzzer feedback on/off |
| Bluetooth | Enable/disable BLE |
| Channel Id | Set the channel number (numeric) |
| Sync Word | Set the LoRa sync word (0x00–0xFF) |
| Input Speed | Normal (150 ms threshold) or Fast (100 ms) |
| Exit | Return to main screen |

Settings are persisted to flash via the `Preferences` library.

---

## Build & Flash

### Requirements

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

### Libraries (auto-installed via `platformio.ini`)

- `adafruit/Adafruit BusIO`
- `thijse/ArduinoLog`
- `arduinogetstarted/ezButton`
- `sandeepmistry/LoRa`
- `adafruit/Adafruit GFX Library`
- `adafruit/Adafruit SSD1306`

### Flash

Select the environment matching your OS:

```bash
# macOS
pio run -e ttgo-lora32-v1-mac -t upload

# Windows (default COM3)
pio run -e ttgo-lora32-v1-windows -t upload

# Linux
pio run -e ttgo-lora32-v1-unix -t upload
```

Adjust the `upload_port` in `platformio.ini` if your serial port differs.

---

## Message Protocol

Each LoRa packet includes the sender's chip ID and a message type:

| Type | Description |
|------|-------------|
| `HELLO` | Broadcast on startup; announces device name and enables discovery |
| `TEXT` | A Morse-decoded text message |
| `TEXT_ACK` | Acknowledgment of a received text message |

Received messages are displayed as `<index>:<device_name> <text>` in the top screen.

---

## Photos

| | |
|---|---|
| ![](https://raw.githubusercontent.com/guidebee/esp32morse/refs/heads/master/images/1601632680949.jfif) | ![](https://raw.githubusercontent.com/guidebee/esp32morse/refs/heads/master/images/1602763708481.jfif) |
| ![](https://raw.githubusercontent.com/guidebee/esp32morse/refs/heads/master/images/1603798999329.jfif) | ![](https://raw.githubusercontent.com/guidebee/esp32morse/refs/heads/master/images/1605881449078.jfif) |

---

## License

MIT

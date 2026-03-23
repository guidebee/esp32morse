# Libraries

Private libraries compiled as static libraries by PlatformIO and linked into the firmware. Each library lives in its own subdirectory under `lib/`.

## button

Wraps [`ezButton`](https://github.com/ArduinoGetStarted/ezButton) with debouncing support.

- **`PushButton`** — thin subclass of `ezButton` for a single GPIO pin
- **`Keypad`** — manages four buttons (main, left, right, ok) and dispatches events through a `KeypadListener` interface

| Button | Pin |
|--------|-----|
| Main   | 36  |
| Left   | 34  |
| Right  | 39  |
| OK     | 23  |

## config

Defines `Configuration`, a plain struct holding all runtime settings, plus the `globalConfiguration` extern.

| Field | Type | Description |
|-------|------|-------------|
| `channelId` / `channelIdValue` | `string` / `int` | LoRa channel identifier |
| `chipId` | `string` | Unique device ID |
| `deviceName` | `string` | Human-readable name (default: `"Morse User"`) |
| `syncWord` | `byte` | LoRa sync word |
| `playSound` | `bool` | Enable buzzer |
| `inputSpeed` | `bool` | Morse input speed mode |
| `loraFrequency` | `string` | LoRa band frequency |
| `encryptionKey` | `char[16]` | AES-128 key |
| `bluetooth` | `bool` | Enable Bluetooth |

## display

OLED display driver built on `Adafruit_SSD1306` (128×64, I²C on SDA=4 / SCL=15 / RST=16).

- **`OledDisplay`** — subclass of `Adafruit_SSD1306`; handles hardware init
- **`Screen`** — buffered text display with a 50-line × 20-char ring buffer, viewport scrolling, optional cursor, and backspace support
- **`DynamicScreen`** — extends `Screen` with timed pattern-driven text animation (e.g. for Morse playback visualization); supports an `ExtraDisplayListener` hook for custom drawing

## led

Non-blocking LED driver for Morse and status signals.

- **`SignalLed`** — queues `LedSignal` (on/off + duration ms) segments and drives a GPIO pin asynchronously in `loop()`

| Method | Description |
|--------|-------------|
| `signal(on, period)` | Enqueue a single on/off pulse |
| `signalMorseText(text)` | Flash full text as Morse code |
| `signalMorse(raw)` | Flash a raw dit/dash string |
| `signalMessageReceived()` | Blink pattern for incoming message |
| `signalMessageSent()` | Blink pattern for sent message |
| `signalError()` | Blink pattern for errors |

## lora

LoRa radio layer built on the Arduino `LoRa` library (SPI: SCK=5, MISO=19, MOSI=27, SS=18, RST=14, DIO0=26, default band 915 MHz).

- **`LoraRadioClass`** — subclass of `LoRaClass`; encodes/decodes messages with a 48-byte header (channel ID, chip ID, counter, encryption flag, message type, length) and up to 80 bytes of payload
- AES-128 encrypt/decrypt via free functions `encrypt()` / `decrypt()`
- Message queue with non-blocking `loop()` dispatch
- Automatic hello/ACK exchange with randomized timing
- Dispatches received messages to a `LoraMessageListener`

Message types: `HELLO (0)`, `TEXT (1)`, `TEXT_ACK (2)`

## morse

Morse code encoder and decoder.

- **`MorseCodePatternMatch`** — real-time pattern matcher; call `process(bool isTone)` each sample tick. Tracks dot/dash/space timing, auto-adjusts WPM estimate, and fires `MorseCodeListener` callbacks (`onEmit`, `onCharStart`, `onCharEnd`). Supports the full ITU character set plus punctuation.
- **`KeyboardMorseCodeDecoder`** — subclass for keyboard/button input; `processKey(bool input)` maps a key press/release to a scaled sequence of samples (dot = 15 samples, dash = 45 samples)

| Method | Description |
|--------|-------------|
| `getMorseLetter(string)` | Dit/dash string → character |
| `getMorseString(char)` | Character → dit/dash string |
| `generateDitDashString(text)` | Text → full spaced dit/dash string |
| `changeDotLimit(float)` | Override the dot-length threshold |

## sound

Non-blocking buzzer driver using the ESP32 `tone()` API.

- **`BuzzerTone`** — queues `Tone` (frequency + duration ms) segments and plays them asynchronously in `loop()`

| Method | Description |
|--------|-------------|
| `playTone(freq, period)` | Enqueue an arbitrary tone |
| `playDi()` / `playDah()` | Morse dit / dah tones |
| `playMorse(raw)` | Play a raw dit/dash string |
| `playMorseText(text)` | Convert text to Morse and play |
| `playMessageReceived()` | Notification sound for incoming |
| `playMessageSent()` | Notification sound for sent |
| `playError()` | Error sound |
| `playNote(note, octave, period)` | Play a musical note |

## ui

Base class for application screens.

- **`BaseView`** — aggregates `OledDisplay`, `Keypad`, and a `DynamicScreen` top-bar (rows 0–1). Implements `KeypadListener` with empty defaults so subclasses override only what they need. Provides `setup()` / `loop()` and a `clearClient()` helper.

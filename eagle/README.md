# ESP32 Morse LoRa - PCB Design (Eagle)

PCB design for the LoRa Morse code walkie-talkie, designed in Autodesk EAGLE 9.6.2.

## Files

| File | Description |
|------|-------------|
| `morse_lora_v6.sch` | Latest schematic |
| `morse_lora_v6.brd` | Latest board layout |
| `morse_lora_v1–v5.*` | Prior revision history |

## Board Specs

- **Size**: ~55mm × 83mm (2.2" × 3.3")
- **Layers**: 2 (Top + Bottom)
- **Components**: 29 (through-hole / PTH)
- **Software**: EAGLE 9.6.2

## Bill of Materials (v6)

| Ref | Qty | Value | Description |
|-----|-----|-------|-------------|
| ESP32LORA | 1 | — | TTGO WiFi-LoRa-32 v1 module |
| C1–C4 | 4 | 0.1µF 50V | Decoupling capacitors |
| R1–R6 | 6 | 10kΩ 1/4W | Button pull-up/pull-down resistors |
| R7–R9 | 3 | 330Ω 1/10W | LED current limiting resistors |
| D-REC | 1 | Red 5mm LED | Receive indicator |
| D-BUT | 1 | Red 5mm LED | Button press feedback |
| D-BLU | 1 | Blue 5mm LED | Radio/BT status |
| RIGHT, LEFT, OK | 3 | 6mm switch | Navigation buttons (momentary PTH) |
| MAIN | 1 | 12mm switch | Main/power button (momentary PTH) |
| SLEEP | 1 | EG1218 | Sleep mode toggle switch |
| BUZZER | 1 | — | PTH electromechanical buzzer |

## Viewing the Files

Since EAGLE `.brd`/`.sch` files are XML-based, you can view them with:

- **[Autodesk EAGLE](https://www.autodesk.com/products/eagle/)** (free tier available) — full editing
- **[KiCad](https://www.kicad.org/)** with the EAGLE import plugin — open source
- **[EasyEDA](https://easyeda.com/)** — browser-based, supports EAGLE import
- **[Gerber viewers](https://www.pcbway.com/project/OnlineGerberViewer.html)** — after exporting Gerbers from EAGLE

## Design Revision History

| Version | Notes |
|---------|-------|
| v1 | Initial design — PTH switches, 5mm LEDs, discrete passives |
| v2 | Minor iteration on v1 |
| v3 | SMD redesign (0603 resistors, 1206 LEDs, SMD buzzer) — later reverted |
| v4 | Reverted to PTH; added SLEEP switch |
| v5 | Minor iteration on v4 |
| v6 | Added 330Ω LED current-limiting resistors (R7–R9); upgraded MAIN to 12mm switch |

The v3 SMD attempt was reverted in v4, settling on a PTH-first approach for easier hand-assembly and mechanical reliability of the buttons/switches.

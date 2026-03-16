# Vacuum Pump Controller

**ESP32-S3 firmware for a wireless-controlled pressure/vacuum pump system**

A dual-interface (BLE + 433 MHz RF) embedded controller for a pneumatic pump unit built around three reclaimed diaphragm pumps. Designed for extended CNC machining sessions where a suction-cup workpiece fixture requires reliable, remotely-operable air management.

---

## Device Photo
![Pump image](https://github.com/H1M4W4R1/RemoteVacuumPump/blob/master/images/pump.webp?raw=true)

---

## Table of Contents

- [Overview](#overview)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [Firmware Architecture](#firmware-architecture)
- [BLE Interface](#ble-interface)
- [RF Remote Control](#rf-remote-control)
- [Safety Features](#safety-features)
- [Building & Flashing](#building--flashing)
- [Project Background](#project-background)

---

## Overview

The controller manages two independently-controlled actuators:

| Actuator | GPIO | Description |
|----------|------|-------------|
| **Pump** | 9 | Three 555-type diaphragm pumps wired in parallel, switched via MOSFET |
| **Valve** | 7 | Normally-open solenoid valve; locks/unlocks the pneumatic circuit |

Both can be commanded wirelessly — either over **Bluetooth Low Energy** (using any generic GATT client) or via a **433 MHz two-button RF remote**. A session timer tracks total valve-lock time and an independent pump timer tracks cumulative run time, with configurable automatic shutoff for each.

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | ESP32-S3 Zero (or any ESP32-S3 with ≥ 3 free GPIOs) |
| Pump mechanism | 3× type-555 diaphragm pumps (tandem configuration) |
| Solenoid valve | Normally-open, 24 V coil |
| MOSFET modules | FR120N — one per actuator |
| RF receiver | SYN480R (433 MHz) |
| Power supply | 24 V / 5 A external PSU |
| Step-down converter | LM2596 (24 V → 5 V for MCU) |
| Pneumatic fittings | 8 mm quick-couplers, PE tubing 8/5 mm |
| Enclosure | Kradex Z39W plastic housing |
| Connectors | 3-pin WAGO lever connectors for internal wiring |

---

## Wiring

```
GPIO 1  ──── SYN480R DATA pin  (RF receiver input)
GPIO 7  ──── MOSFET gate  ──── Solenoid valve (24 V coil)
GPIO 9  ──── MOSFET gate  ──── Pump motors (24 V)

External emergency-stop button cuts 24 V rail for solenoid and motors
  ↳ Microcontroller stays powered (5 V from LM2596 remains active)
```

---

## Firmware Architecture

```
main.cpp                    ← init + delta-time event loop
├── sys_runtime             ← GPIO control, session/pump timers, safety shutoff
├── sys_bluetooth           ← BLE server, 7 GATT characteristics, auto-reconnect
└── sys_rf                  ← RCSwitch 433 MHz receiver, button decoding, cooldown
```

The main loop calculates `deltaTime` on every tick and passes it into each subsystem. This keeps all timers accurate regardless of loop duration variance.

---

## BLE Interface

**Device name:** `Vacuum Pump Prototype`
**Service UUID:** `ae615e5d-b4be-428e-8ff9-9348c929a36e`

| Characteristic | UUID (prefix `ae61…`) | Type | Properties | Purpose |
|----------------|----------------------|------|------------|---------|
| Pump Activity Status | `…5056` | `uint8` | R/W/Notify | `1` = start pump, `0` = stop pump |
| Valve Lock Status | `…5d1c` | `uint8` | R/W/Notify | `1` = lock valve, `0` = unlock valve |
| Valve Lock Allowed | `…5096` | `uint8` | R/W | Allow (`1`) / block (`0`) RF valve control |
| Expected Session Time | `…5c93` | `uint32` | R/W | Auto-end session after N seconds |
| Expected Pump Time | `…52b7` | `uint32` | R/W | Auto-stop pump after N seconds |
| Current Session Time | `…502e` | `uint32` | R/W/Notify | Live session timer (seconds) |
| Current Pump Time | `…53c8` | `uint32` | R/W/Notify | Live pump run timer (seconds) |

Any generic BLE GATT tool (e.g. **nRF Connect**, **LightBlue**) can connect and control the device. The server automatically restarts advertising after a client disconnects.

---

## RF Remote Control

The 433 MHz remote has two buttons. With default button codes:

| Button | Code | Action |
|--------|------|--------|
| A | `7300524` | Toggle pump (start if idle, stop if running) |
| B | `7300528` | Toggle valve lock (if RF valve control is enabled) |

RF commands are subject to a **1-second cooldown** to prevent accidental double-triggers. RF valve control can be disabled remotely via the `Valve Lock Allowed` BLE characteristic.

To pair a different remote, update `RF_BUTTON_A` / `RF_BUTTON_B` in `include/sys_rf.h` or perform pairing using SYN115 or similar transmitter.

---

## Safety Features

| Mechanism | Behaviour |
|-----------|-----------|
| **Pump timeout** | Pump stops automatically when `current_pump_time` reaches `expected_pumping_time` |
| **Session timeout** | Valve unlocks and pump stops when session duration limit is reached |
| **8-hour hard limit** | `MAX_SAFE_TIME` (28 800 s) triggers `session_end()` unconditionally — no override |
| **Hardware E-stop** | Physical button cuts 24 V to both actuators; MCU keeps running |
| **Normally-open valve** | Pressure equalises passively on power loss; no pressure trapped in circuit |

---

## Building & Flashing

### Prerequisites

- [PlatformIO](https://platformio.org/) CLI or IDE extension
- USB cable to the ESP32-S3

### Build & upload

```bash
pio run --target upload
```

### Monitor serial output

```bash
pio device monitor --baud 9600
```

The `platformio.ini` targets **esp32-s3-devkitc-1** with 4 MB QIO Flash and 2 MB QIO PSRAM. Adjust `board` and memory settings if using a different ESP32-S3 module.

---

## Project Background

This controller was built to solve a real workshop problem: a small airbrush compressor overheated within minutes when used as a CNC milling machine's cooling solution. Instead of stressing the incompatible hardware, a custom pump unit was assembled from three salvaged 555-type diaphragm pumps wired in parallel — providing both pressure output and a vacuum outlet for any needs.

The enclosure was machined on the same CNC that the device serves, with component mounting holes cut from CAD drawings directly into the Kradex Z39W housing. Internal wiring uses WAGO connectors throughout.


Full project write-up (Polish): [forbot.pl forum thread](https://forbot.pl/forum/topic/25713-pompa-cisnieniapodcisnienia-sterowana-radiowo/)

---

## License

This project is provided as-is for personal and educational use.

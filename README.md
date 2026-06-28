<div align="center">

# 🎓 Smart Exam Hall Monitoring & Management System

**An embedded system for automated, secure, and real-time exam hall management**

[![Platform](https://img.shields.io/badge/Platform-LPC2148%20ARM7-blue?style=for-the-badge&logo=arm)](https://www.nxp.com)
[![Language](https://img.shields.io/badge/Language-Embedded%20C-brightgreen?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/Embedded_C)
[![IDE](https://img.shields.io/badge/IDE-Keil%20%C2%B5Vision-orange?style=for-the-badge)](https://www.keil.com/)
[![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)]()

</div>

---

## 📌 Overview

The **Smart Exam Hall Monitoring and Management System** is a real-time embedded application designed to automate and secure exam hall operations. Built on the **NXP LPC2148 ARM7TDMI-S microcontroller**, it integrates an RTC, keypad, LCD, 7-segment display, temperature sensor, LEDs, and a buzzer to manage everything from exam scheduling to environment monitoring — all behind a password-protected interface.

> 💡 This system eliminates manual timekeeping in exam halls by automating countdowns, alerts, and access control.

---

## ✨ Features

| Feature | Description |
|---|---|
| ⏱️ **Exam Countdown** | Automatically starts when RTC matches the set exam time; displays remaining time on 7-segment |
| ⏸️ **Pause & Resume** | EINT2 interrupt pauses the countdown; paused duration is excluded from total elapsed time |
| 🔐 **Password Protection** | 4-digit PIN (default: `1234`) guards all settings; 3 attempts allowed before lockout |
| 🌡️ **Temperature Monitoring** | LM35 sensor reads room temperature via ADC and displays in °C on LCD |
| 📅 **RTC Management** | Internal RTC for live time/date display; fully configurable via keypad |
| 💡 **LED Alerts** | Three-stage visual alert as exam time nears its end |
| 🔊 **Buzzer** | Sounds for 3 seconds when exam duration is over |
| 🖥️ **20×4 LCD Display** | Shows live time, date, temperature, and remaining duration simultaneously |
| ⌨️ **4×4 Keypad** | Full navigation and data entry for all settings |
| 🔢 **7-Segment Display** | Dedicated 2-digit display for exam countdown |

---

## 🚦 LED Alert Logic

```
Remaining Time        LED State
─────────────────     ──────────────────────────
> 15 minutes      →    All LEDs OFF
≤ 15 minutes      →    🟡 LED3 ON
≤ 10 minutes      →    🟠 LED2 ON  (LED3 OFF)
≤ 5 minute       →    🔴 LED1 ON  (LED2 OFF)
  0 minutes       →    🔊 BUZZER sounds for 3 sec
```

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   LPC2148 ARM7 MCU                      │
│                                                         │
│  ┌──────────┐   ┌──────────┐   ┌─────────────────────┐ │
│  │  EINT0   │   │  EINT2   │   │   Timer0 (ISR)      │ │
│  │ Settings │   │  Pause/  │   │  7-Seg Multiplexer  │ │
│  │  Menu    │   │  Resume  │   │  (5ms tick)         │ │
│  └────┬─────┘   └────┬─────┘   └─────────────────────┘ │
│       │              │                                  │
│  ┌────▼──────────────▼───────────────────────────────┐  │
│  │              Main Loop                            │  │
│  │  • RTC time/date display                         │  │
│  │  • Exam start detection (uhour == HOUR)          │  │
│  │  • Elapsed time calculation (with pause offset) │  │
│  │  • LED / Buzzer control                         │  │
│  │  • Temperature reading (LM35 via ADC)           │  │
│  └───────────────────────────────────────────────────┘  │
│                                                         │
│  PERIPHERALS:  LCD │ Keypad │ 7-Seg │ LM35 │ LEDs/BUZ  │
└─────────────────────────────────────────────────────────┘
```

---

## 🔌 Hardware Components

| # | Component | Part / Spec | Role |
|---|---|---|---|
| 1 | Microcontroller | NXP LPC2148 (ARM7TDMI-S, 60 MHz) | Core processing |
| 2 | Display | 20×4 Character LCD (HD44780) | Time, date, temp, duration |
| 3 | Keypad | 4×4 Matrix Keypad | User input & navigation |
| 4 | 7-Segment | 2-Digit Common Anode (Multiplexed) | Countdown display |
| 5 | Temperature Sensor | LM35 (Analog, 10mV/°C) | Room temp monitoring |
| 6 | RTC | LPC2148 Internal RTC | Real-time clock |
| 7 | LEDs | 3× General Purpose LEDs | Time-warning indicators |
| 8 | Buzzer | Active Buzzer | Exam-end alert |
| 9 | Push Buttons | 2× (EINT0, EINT2) | Settings / Pause-Resume |

---

## 📍 Pin Configuration

### GPIO — Port 0

| Pin | Label | Function |
|---|---|---|
| P0.5 | BUZZER | Buzzer output |
| P0.8 – P0.15 | LCD_DATA | 8-bit LCD data bus |
| P0.16 | LCD_RS | LCD register select |
| P0.17 | LCD_EN | LCD enable |
| P0.19 | DSEL1 | 7-seg digit select 1 |
| P0.20 | DSEL2 | 7-seg digit select 2 |
| P0.27 | LED1 | Red alert (≤1 min) |
| P0.28 | LED2 | Amber alert (≤3 min) |
| P0.29 | LED3 | Yellow alert (≤5 min) |

### GPIO — Port 1

| Pin | Label | Function |
|---|---|---|
| P1.16 – P1.19 | ROW0–ROW3 | Keypad row outputs |
| P1.20 – P1.23 | COL0–COL3 | Keypad column inputs |
| P1.24 – P1.31 | ca7seg_2_mux | 7-segment segment data |

### Special Function Pins

| Pin | Function |
|---|---|
| EINT0 | Settings menu trigger (falling edge) |
| EINT2 | Pause / Resume exam (falling edge) |
| ADC CH3 (P0.28) | LM35 analog input |

---

## 📁 Project Structure

```
Smart-Exam-Hall-System/
│
├── 📄 main.c                  ← Main loop, ISRs, exam countdown logic
├── 📄 project.c               ← Peripheral drivers (LCD, Keypad, ADC, LM35,
│                                 Delay, 7-Segment)
├── 📄 project_functions.c     ← Menu system: password, RTC settings,
│                                 exam time, duration, password change
│
├── 📋 all_macro1.h            ← Type definitions, bit macros, all pin/peripheral
│                                 defines (LCD, KPM, ADC, RTC, SEG, LED)
├── 📋 project_declaration.h   ← Function prototypes for project_functions.c
└── 📋 declaration.h           ← Function prototypes for project.c drivers
```

---

## ⚙️ How It Works

### 1️⃣ Boot & Initialization
```
Power ON → Initialize GPIO, LCD, Keypad, 7-Seg, ADC, Timer0, EINT0, EINT2
         → Set default RTC (12:59:40, 12/06/2026)
         → Display "SYSTEM LOADING..." → Enter Main Loop
```

### 2️⃣ Main Loop (Continuous)
- **Line 1:** Live RTC time `HH:MM:SS`
- **Line 2:** Live RTC date `DD/MM/YYYY`
- **Line 3:** Temperature `XX.XX °C` + PAUSE indicator (if paused)
- **Line 4:** `Duration: XX` (remaining minutes)
- **7-Seg:** Remaining minutes (multiplexed via Timer0 ISR)

### 3️⃣ Settings Access (EINT0 Button)
```
Press EINT0 → Password prompt (3 attempts, 4-digit PIN)
            → Main Menu:
              1. Edit RTC Time/Date
              2. Edit Exam Start Time & Duration
              3. Change Password
              4. Exit
```
> ⚠️ EINT0 is **disabled** once an exam starts and **re-enabled** after it ends.

### 4️⃣ Exam Lifecycle
```
Set exam start time + duration via settings menu
         ↓
RTC matches exam start time
         ↓
Countdown begins → 7-seg counts down → LEDs trigger at thresholds
         ↓
        (optional) EINT2 to PAUSE → EINT2 again to RESUME
        (paused seconds are excluded from elapsed time)
         ↓
Duration reaches 0 → BUZZER sounds 3 sec → System resets for next exam
```

---

## ⏱️ Elapsed Time Calculation

The system uses a **total-minutes reference model** to accurately handle midnight rollovers and pauses:

```c
elapsed = (nowTotalMin - examStartTotalMin) - pause;
dur = (elapsed >= tempTime) ? 0 : tempTime - elapsed;
```

- `examStartTotalMin` — captured once at exam start (in total minutes from midnight)
- `pause` — accumulates paused minutes across multiple pause/resume cycles
- `flag2 % 2 == 0` → Resuming; `flag2 % 2 == 1` → Pausing

---

## 🔐 Password System

- Default PIN: **`1234`**
- 4-digit numeric input only (range: `1000–9999`)
- `*` shown on LCD for each digit entered
- `+` key acts as **backspace**
- `c` key cancels and returns to main display
- **3 failed attempts** → 3-second lockout → return to main loop
- PIN can be changed from the settings menu after authentication

---

## 🌡️ Temperature Monitoring

```
LM35 Output → ADC Channel 3 → 10-bit conversion (0–1023)
→ eAR = (3.3V / 1024) × ADC_value
→ Temperature (°C) = eAR × 100
→ Displayed on LCD Line 3 with 2 decimal places
```

---

## 🛠️ Development Environment

| Tool | Details |
|---|---|
| **IDE** | Keil µVision 4/5 (ARM-MDK) |
| **Compiler** | Keil ARM C Compiler |
| **Target MCU** | NXP LPC2148 — ARM7TDMI-S @ 60 MHz |
| **PCLK** | 15 MHz |
| **Flash Tool** | Flash Magic (UART ISP) |
| **Debugger** | JTAG / Keil simulator |
| **Language** | Embedded C (non-standard extensions: `__irq`) |

---

## 🚀 Getting Started

### Prerequisites
- Keil µVision IDE installed
- LPC2148 development board
- Flash Magic (for flashing via UART) or JTAG programmer

### Steps

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/Smart-Exam-Hall-System.git
cd Smart-Exam-Hall-System

# 2. Open in Keil µVision
#    File → Open Project → select the .uvproj file

# 3. Add source files to project
#    main.c, project.c, project_functions.c

# 4. Set target to LPC2148
#    Project → Options for Target → Device: LPC2148

# 5. Build
#    Project → Build Target  (Shortcut: F7)

# 6. Flash to board
#    Use Flash Magic with UART or JTAG debugger
```

---

## ⚠️ Known Constraints & Notes

- `PINSEL1 = 0x10000000` is set in both `main.c` and `Init_ADC()` — ensure no conflict with other PINSEL1 configurations
- Exam duration is validated between **5 and 99 minutes**
- Exam start hour must be ≥ current RTC hour; minutes validated accordingly
- The `f32Lcd()` function has a minor bug (`if(num<0.0)` checks uninitialized `num` instead of `fnum`) — fix: change `num` to `fnum` in that condition
- `EXTMODE` and `EXTPOLAR` registers should be configured before enabling interrupts in VIC for reliable edge detection

---



---

<div align="center">

**Built with ❤️ on ARM7 | LPC2148 | Embedded C**

</div>

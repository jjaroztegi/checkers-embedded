# Hardware Setup

This document details the required hardware, jumper settings, and wiring for a single player unit. The setup must be replicated for both Player 1 and Player 2.

## 1. Bill of Materials (Per Player)

A complete two-player game requires **two** of each of the following components (for a total of six elements):

- **1x MSP-EXP430FR5994 LaunchPad:** The main processor for game logic and UI.
- **1x BOOSTXL-EDUMKII BoosterPack:** Provides the LCD, joystick, and buttons for user interaction.
- **1x CC1310 LaunchPad:** Serves as the dedicated radio co-processor.
- **3x Jumper Wires:** To connect the MSP430 to the CC1310 (UART and GND).

## 2. Component Setup (Per Player)

Before wiring, each board must be configured correctly.

### 2.1. MSP430FR5994 LaunchPad

The MSP430 communicates with the CC1310 using its `EUSCI_A3` UART peripheral, which is mapped to pins **P6.0 (TX)** and **P6.1 (RX)**. On the LaunchPad, these pins are connected to the onboard debugger by default.

- **Action:** **Remove** the `RXD` and `TXD` jumpers from the **J101** header.
- **Reason:** This disconnects the UART pins from the onboard debugger, allowing them to be wired directly to the CC1310.

### 2.2. BOOSTXL-EDUMKII BoosterPack

The EDUMKII provides the user interface.

- **Action:** Stack the EDUMKII BoosterPack directly on top of the MSP430FR5994 LaunchPad, ensuring all pins are aligned.

### 2.3. CC1310 LaunchPad

The CC1310 uses its `Board_UART0` peripheral, which is mapped to pins **IOID_2 (RX)** and **IOID_3 (TX)**. These pins are also connected to their own onboard debugger by default.

- **Action:** **Remove** the `RXD` and `TXD` jumpers from the **P1** header (the 10-pin header between the CC1310 and the debugger).
- **Reason:** This disconnects the UART pins from the CC1310's debugger, allowing them to be wired directly to the MSP430.

## 3. System Wiring Diagram

With the jumpers removed and the EDUMKII stacked, connect the MSP430 and CC1310 LaunchPads using three jumper wires as shown below.

| MSP430FR5994 Pin | <-> | CC1310 LaunchPad Pin | Purpose                |
| :--------------- | :-: | :------------------- | :--------------------- |
| `GND`            | <-> | `GND`                | Common Ground          |
| `P6.0 (TXD)`     | <-> | `DIO2 (RXD)`         | MSP430 sends to CC1310 |
| `P6.1 (RXD)`     | <-> | `DIO3 (TXD)`         | CC1310 sends to MSP430 |

## 4. Final Assembly

The final assembly for a single player unit will consist of the stacked MSP430 + EDUMKII, connected via the three jumper wires to the standalone CC1310 LaunchPad. Both LaunchPads will need to be powered via their respective USB ports.

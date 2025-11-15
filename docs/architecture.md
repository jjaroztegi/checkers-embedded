# Project Architecture

This document details the system architecture for the wireless checkers project.

## 1. System Overview

The project implements a two-player checkers game using a distributed system architecture. Each player unit consists of two separate microcontrollers working in tandem: an **MSP430FR5994** for game logic and user interface, and a **CC1310** acting as a dedicated wireless co-processor.

This dual-MCU design decouples the real-time game logic from the complexities of managing the wireless communication stack.

### 1.0. Code Organization

To minimize code duplication and maintain consistency across both player units, shared code is organized into common directories:

- **`common_msp430/`**: Contains all shared MSP430 code used by both `player1-msp430` and `player2-msp430` projects:

  - **`_ti_driverlib/`**: Texas Instruments MSP430 driver library for peripheral management
  - **`_ti_grlib/`**: Graphics library for LCD rendering
  - **`comm/`**: Communication protocol implementation (UART handling, `protocol.c`)
  - **`drivers/`**: Hardware drivers (LCD, joystick, light sensor, etc.)
  - **`game/`**: Checkers game logic (`checkers.c`, board state management, move validation)
  - **`hal/`**: Hardware abstraction layer
  - **`input/`**: Input handling (joystick, buttons, debouncing)

- **`common_cc1310/`**: Contains shared CC1310 code used by both `player1-cc1310` and `player2-cc1310` projects:
  - **`easylink/`**: EasyLink wireless API implementation
  - **`smartrf_settings/`**: RF configuration settings

Each player-specific project (e.g., `player1-msp430`) contains only its `main.c` entry point and build configuration files, while referencing the shared modules from the common directories.

### 1.1. Component Responsibilities

#### MSP430FR5994 (Game & UI Processor)

The MSP430 serves as the "brain" of each player's unit, handling all user-facing tasks and game management.

- **Game Logic:** Manages the checkers board state, validates moves, and enforces game rules (implemented in `common_msp430/game/checkers.c`).
- **Main Control Loop:** Operates a state machine to manage the player's turn (`TURN_PLAYING`, `TURN_SENDING`, `TURN_WAITING`).
- **Display:** Renders the game board, pieces, and status messages to the EDUMKII's LCD screen using the `crystalfontz` driver from `common_msp430/drivers/`.
- **User Input:** Polls the EDUMKII's joystick and buttons, debounces them, and translates them into game actions (e.g., move cursor, select piece) using modules from `common_msp430/input/`.
- **Peripheral Management:**
  - Reads the `OPT3001` ambient light sensor via I2C.
  - Controls the LCD backlight brightness via PWM, automatically adjusting for ambient light.
  - Hardware drivers are located in `common_msp430/drivers/`.
- **Communication:** Communicates with its partner CC1310 processor over a UART (115200 baud) serial link using the protocol implementation in `common_msp430/comm/protocol.c`.

#### CC1310 (Radio Co-Processor)

The CC1310 functions as a simple, dedicated modem or "radio co-processor". It bridges the UART serial link from the MSP430 to the wireless RF link between players.

- **Wireless Stack:** Manages the EasyLink RF API for radio operations (implementation in `common_cc1310/easylink/`).
- **RF Configuration:** Uses settings from `common_cc1310/smartrf_settings/` for radio parameters.
- **Data Bridging:**
  1.  Listens for an ASCII move string (e.g., "A6B5") from the MSP430 on its `UART_READING` state.
  2.  Wraps this string into an 8-byte EasyLink packet and transmits it wirelessly (`RF_SENDING` state).
  3.  Listens for an incoming RF packet from the opponent (`RF_RECEIVING` state).
  4.  Extracts the ASCII move string from the received packet and forwards it to its MSP430 via UART (`UART_WRITING` state).

## 2. Communication Protocols

There are two distinct communication protocols operating in this system.

### 2.1. Internal Protocol: MSP430 <-> CC1310 (UART)

This is the serial link between the two microcontrollers within a single player unit.

- **Physical Layer:** UART. The MSP430 uses `EUSCI_A3` on pins P6.0 (TX) and P6.1 (RX). The CC1310 uses `Board_UART0`.
- **Configuration:** 115200 baud, 8-N-1.
- **Data Format:** Null-terminated ASCII strings. The `common_msp430/comm/protocol.c` module handles sending and receiving strings (e.g., "A6B5") terminated by `\r` and `\n` characters.

### 2.2. External Protocol: CC1310 <-> CC1310 (RF EasyLink)

This is the wireless link between the two separate player units.

- **Physical Layer:** RF via EasyLink API.
- **Configuration:**
  - **Frequency:** 862 MHz (862000000 Hz).
  - **RF Power:** 14 dBm.
- **Packet Structure:** The system uses an 8-byte EasyLink payload.
  - **Bytes 0-1:** A 16-bit sequence number (`seqNumber`).
  - **Bytes 2-7:** The ASCII move string, with a max length of 6 characters (e.g., "A6B5").

## 3. Data Flow & State Management

The game is kept in sync using a half-duplex, lock-step communication flow.

- **MSP430 State Machine:** The `main.c` on the MSP430 controls the player's turn.

  1.  **`TURN_WAITING`**: The unit is listening for an incoming move string on the UART from its CC1310. When a move is received, it is applied to the local game board, and the state transitions to `TURN_PLAYING`.
  2.  **`TURN_PLAYING`**: The unit polls the joystick and buttons for the local player's move. When a valid move is confirmed, the move data is stored, and the state transitions to `TURN_SENDING`.
  3.  **`TURN_SENDING`**: The unit encodes the move into an ASCII string (e.g., "C3D4") and sends it to its CC1310 via UART. It then immediately transitions to `TURN_WAITING`.

- **CC1310 State Machine:** The CC1310s run complementary state machines to enable the MSP430's flow.
  - **Player 1's CC1310** (`rfEasyLinkTx_nortos.c`) starts by waiting for a UART message from its MSP430. Its flow is: `UART_READING` -> `RF_SENDING` -> `RF_RECEIVING` -> `UART_WRITING`.
  - **Player 2's CC1310** (`rfEasyLinkRx_nortos.c`) starts by waiting for an RF message from Player 1. Its flow is: `RF_RECEIVING` -> `UART_WRITING` -> `UART_READING` -> `RF_SENDING`.

Player 1 (Red) begins in the `TURN_PLAYING` state, while Player 2 (Black) begins in the `TURN_WAITING` state, establishing the game's initial turn.

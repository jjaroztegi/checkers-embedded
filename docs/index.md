# Wireless Checkers Project

Welcome to the official documentation for the two-player wireless checkers game.

This project implements a full checkers game for two players, using two sets of Texas Instruments microcontrollers. Each player unit uses an **MSP430FR5994** LaunchPad for game logic and user interface, and a **CC1310** LaunchPad for wireless radio communication.

This site provides an in-depth look at the project's architecture, communication protocols, and setup instructions.

---

## Key Features

- **Dual-MCU Architecture:** A dedicated MSP430FR5994 handles game logic and the user interface, while a CC1310 co-processor manages the wireless communication stack.
- **Wireless 2-Player Gameplay:** Features a robust, half-duplex communication protocol running on the EasyLink RF API for transmitting moves between players.
- **Full Checkers Game Logic:** Implements complete move validation, including standard moves, captures (jumps), and king promotion.
- **Hardware-Driven UI:** Uses the BOOSTXL-EDUMKII's hardware, including the Crystalfontz LCD for the game board display, the joystick for cursor movement, and the buttons for piece selection/confirmation.
- **Adaptive Backlight:** The LCD backlight brightness automatically adjusts to ambient light conditions using the `OPT3001` light sensor for better visibility.

---

## Project Structure

The repository is organized into four main projects, two for each player unit.

```
/
├── player1-msp430/     # Player 1 UI, display, and game logic (MSP430FR5994)
├── player1-cc1310/     # Player 1 radio communication (CC1310)
├── player2-msp430/     # Player 2 UI, display, and game logic (MSP430FR5994)
└── player2-cc1310/     # Player 2 radio communication (CC1310)
```

---

## Documentation

For a deeper understanding of the project, please explore the following sections:

- **[Architecture](architecture.md):**
  A high-level overview of the system, detailing the responsibilities of each microcontroller and how they interact.

- **[Communication Protocol](communication-protocol.md):**
  A detailed look at the data flow, including the UART protocol between the MCUs and the RF (EasyLink) protocol between the two player units.

- **[Hardware Setup](hardware-setup.md):**
  The bill of materials, jumper settings, and wiring diagrams required to build the hardware for one player unit.

- **[Build Guide](build-guide.md):**
  Instructions for setting up the development environment, configuring Code Composer Studio, and flashing the firmware.

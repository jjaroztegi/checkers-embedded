# Checkers Game with Wireless Communication

A two-player checkers game system using MSP430 and CC1310 microcontrollers with radio communication.

## Project Structure

```
checkers/
├── player1-msp430/     # Player 1 UI, display, and game logic (MSP430FR5994)
├── player1-cc1310/     # Player 1 radio communication (CC1310)
├── player2-msp430/     # Player 2 UI, display, and game logic (MSP430FR5994)
└── player2-cc1310/     # Player 2 radio communication (CC1310)
```

## Architecture

Each player unit consists of two microcontrollers:

- **MSP430FR5994**: Handles LCD display, user input, game logic, move validation, and LED indicators
- **CC1310**: Manages wireless communication (both transmit and receive) via EasyLink

Both players can send and receive moves, enabling bidirectional gameplay.

## Development Environment

### Required Software

- **Code Composer Studio (CCS)**: v10.3
- **SimpleLink CC13x0 SDK**: v4.10.01.01
- **VS Code** (optional): For code editing with IntelliSense

### Environment Setup

The VS Code configuration files use environment variables for machine-specific paths. You need to update these in each project's `.vscode/c_cpp_properties.json`:

```json
"env": {
  "CCS_INSTALL_ROOT": "/Applications/TI/ccs1031/ccs",  // Update for your system
  "SIMPLELINK_CC13X0_SDK": "/path/to/simplelink_cc13x0_sdk_4_10_01_01"
}
```

#### Windows Example

```json
"CCS_INSTALL_ROOT": "C:/ti/ccs1031/ccs"
"SIMPLELINK_CC13X0_SDK": "C:/ti/simplelink_cc13x0_sdk_4_10_01_01"
```

## Building

### Using Code Composer Studio

1. Import projects: **File → Import → CCS Projects**
2. Select all four project folders
3. Build each project individually or all together

### Project Dependencies

**MSP430 Projects:**

- MSP430 DriverLib
- Graphics Library (grlib)

**CC1310 Projects:**

- SimpleLink CC13x0 SDK
- EasyLink API
- NoRTOS kernel

## Hardware Setup

### Player 1 Unit

- MSP430FR5994 LaunchPad
- EDUMKII board
- CC1310 LaunchPad

## License

University Project - 2025

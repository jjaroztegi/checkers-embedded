# Communication Protocol

This project utilizes two distinct communication protocols: an internal UART link between the MSP430 and CC1310 on each player unit, and an external RF link between the two CC1310 units.

---

## 1. Internal Protocol: MSP430 <-> CC1310 (UART)

This protocol governs the serial communication between the main game processor (MSP430) and its dedicated radio co-processor (CC1310).

- **Physical Layer:** UART
  - **MSP430:** `EUSCI_A3` (Pins P6.0 TX, P6.1 RX)
  - **CC1310:** `Board_UART0`
- **Configuration:** 115200 baud, 8-N-1 (No Parity, 1 Stop Bit)
- **Data Format:** The system uses the `protocol.c` helper functions (`send_string`, `receive_string`) to exchange data.
  - **Payload:** A simple ASCII string representing the move (e.g., "A6B5").
  - **Framing:** The string is terminated by `\r\n` (carriage return and newline) to signify the end of a message.

---

## 2. External Protocol: Player-to-Player (RF EasyLink)

This protocol governs the wireless communication between the two independent player units via their CC1310 co-processors.

- **Physical Layer:** RF (Radio Frequency) managed by the TI EasyLink API.
- **Configuration:**
  - **Frequency:** 862 MHz (862000000 Hz)
  - **RF Power:** 14 dBm
- **Packet Structure:** The system transmits an 8-byte EasyLink payload (`RFEASYLINKTXPAYLOAD_LENGTH`).
  - **Bytes 0-1:** A 16-bit sequence number (`seqNumber`), incremented for each new packet.
  - **Bytes 2-7:** The 6-byte ASCII move string (e.g., "A6B5"), which is null-padded if shorter.

---

## 3. End-to-End Data Flow & State Machine

The game's turn-based nature is managed by a lock-step state machine that spans all four microcontrollers. Player 1 (Red) starts in `TURN_PLAYING` and Player 2 (Black) starts in `TURN_WAITING`.

The following sequence describes Player 1 making a move:

1.  **P1-MSP430 (`TURN_PLAYING`):**

    - The user selects and confirms a valid move (e.g., "A6B5").
    - `handle_input()` sets the `turn_state` to `TURN_SENDING`.

2.  **P1-MSP430 (`TURN_SENDING`):**

    - The `main.c` loop encodes the move as an ASCII string: "A6B5".
    - It calls `send_string("A6B5")`, which transmits "A6B5\r\n" over UART to the CC1310.
    - The MSP430 immediately changes its state to `TURN_WAITING`.

3.  **P1-CC1310 (`UART_READING`):**

    - The `rfEasyLinkTx_nortos.c` task is blocked on `UART_read()`.
    - It receives the "A6B5" string, discards the `\r\n`, and transitions to `RF_SENDING`.

4.  **P1-CC1310 (`RF_SENDING`):**

    - The task assembles the 8-byte EasyLink packet (e.g., `[0x00, 0x01, 'A', '6', 'B', '5', 0x00, 0x00]`).
    - It calls `EasyLink_transmit()` to send the packet wirelessly.
    - It transitions to `RF_RECEIVING` to await Player 2's response.

5.  **P2-CC1310 (`RF_RECEIVING`):**

    - The `rfEasyLinkRx_nortos.c` task is blocked on `EasyLink_receive()`.
    - It receives the 8-byte packet from Player 1.
    - It extracts the payload string "A6B5" and transitions to `UART_WRITING`.

6.  **P2-CC1310 (`UART_WRITING`):**

    - The task calls `UART_write("A6B5")` to send the move string over UART to its MSP430.
    - It transitions to `UART_READING` to await its own MSP430's reply move.

7.  **P2-MSP430 (`TURN_WAITING`):**
    - The `main.c` loop is blocked on `receive_string()`.
    - It receives the "A6B5" string from its CC1310.
    - It calls `CHECKERS_apply_move_from_string()` to update its local game board.
    - The MSP430 transitions to `TURN_PLAYING`, allowing Player 2 to make their move.

The cycle then repeats in the reverse direction for Player 2's turn.

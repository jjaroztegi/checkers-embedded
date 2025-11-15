# Build and Flash Guide

This guide provides the necessary steps to set up the development environment, build all four project targets, and flash the firmware onto the hardware.

---

## 1. Required Software

Before proceeding, you must download and install the following software:

- **Code Composer Studio (CCS):** **v10.3**
- **SimpleLink CC13x0 SDK:** **v4.10.01.01**

It is important to install the SDK to a simple, root-level path (e.g., `C:/ti/simplelink_cc13x0_sdk_4_10_01_01`) as the projects will need to link to this location.

---

## 2. Environment Setup

### SDK Path Configuration

The CC1310 projects depend on the SimpleLink SDK. You must inform Code Composer Studio where to find these files.

1.  Launch Code Composer Studio.
2.  Go to **Window > Preferences**.
3.  Navigate to **Code Composer Studio > Build > Variables**.
4.  Click **Add...** to create a new path variable.
5.  Define the following variable:
    - **Variable name:** `SIMPLELINK_CC13X0_SDK`
    - **Value:** Set this to the full path where you installed the SDK (e.g., `C:/ti/simplelink_cc13x0_sdk_4_10_01_01`).

This step ensures that the `player1-cc1310` and `player2-cc1310` projects can find the required SDK drivers and kernel files to compile.

### Project Dependencies

All four projects rely on shared code libraries to reduce duplication and maintain consistency:

- **MSP430 Projects** (`player1-msp430` and `player2-msp430`):

  - Depend on shared libraries and application code in `common_msp430/`
  - Includes `_ti_driverlib`, `_ti_grlib`, and shared modules: `comm`, `drivers`, `game`, `hal`, and `input`

- **CC1310 Projects** (`player1-cc1310` and `player2-cc1310`):
  - Depend on shared code in `common_cc1310/`
  - Includes `easylink` and `smartrf_settings` modules

The project files are configured with relative paths to reference these common folders. When importing the projects, ensure the workspace structure maintains the repository layout with the common folders at the same level as the player project folders.

---

## 3. Importing Projects

1.  In CCS, go to **File > Import...**.
2.  Select **Code Composer Studio > CCS Projects** and click **Next**.
3.  Click **Browse...** next to "Select search-directory" and navigate to the root folder of this repository.
4.  All four projects should appear in the "Discovered projects" box:
    - `player1-cc1310`
    - `player1-msp430`
    - `player2-cc1310`
    - `player2-msp430`
5.  Ensure all four projects are checked, then click **Finish** to import them into your workspace.

---

## 4. Building the Projects

You must build all four projects individually. The build order does not matter.

1.  In the **Project Explorer** sidebar, right-click on a project (e.g., `player1-msp430`).
2.  Select **Build Project**.
3.  Repeat this process for the other three projects.

After a successful build, four executable (`.out`) files will be generated in their respective `Debug` folders:

- `player1-msp430.out`
- `player1-cc1310.out`
- `player2-msp430.out`
- `player2-cc1310.out`

---

## 5. Flashing the Firmware

This is the final step to load the compiled code onto the physical hardware.

1.  Assemble both Player 1 and Player 2 hardware units as described in the **Hardware Setup** guide.
2.  Connect all four LaunchPads (2x MSP430, 2x CC1310) to your computer via USB.
3.  For each of the four projects in your CCS workspace, follow these steps:

    1.  Right-click the project in the **Project Explorer** (e.g., `player1-msp430`).
    2.  Select **Debug As > Code Composer Debug Session**.
    3.  If CCS prompts you to select a device (as you have multiple boards connected), select the corresponding physical board for that project.
    4.  CCS will connect to the target, flash the firmware, and halt at the beginning of `main()`.
    5.  Press the **Resume** button (or F8) to run the program.
    6.  Click the **Terminate** button (red square) to end the debug session. The code will remain flashed on the device.

4.  Repeat this process, ensuring the correct firmware is flashed to the correct board:
    - **Player 1 MSP430:** Flash `player1-msp430.out`
    - **Player 1 CC1310:** Flash `player1-cc1310.out`
    - **Player 2 MSP430:** Flash `player2-msp430.out`
    - **Player 2 CC1310:** Flash `player2-cc1310.out`

Once all four boards are flashed, power-cycle them. The game will begin, with Player 1 (Red) able to make the first move.

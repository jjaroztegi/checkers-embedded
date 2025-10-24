/* Application header files */
#include "smartrf_settings/smartrf_settings.h"

/* Board Header files */
#include "Board.h"

/* Standard C Libraries */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* TI Drivers */
#include <ti/devices/DeviceFamily.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/rf/RF.h>

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

#define RFEASYLINKTXPAYLOAD_LENGTH 30

/* Pin driver handle */
static PIN_Handle pinHandle;
static PIN_State pinState;

/* UART driver handle */
static UART_Handle uartHandle;
/* LED configuration */
PIN_Config pinTable[] = {Board_PIN_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW |
                             PIN_PUSHPULL | PIN_DRVSTR_MAX,
                         Board_PIN_LED2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW |
                             PIN_PUSHPULL | PIN_DRVSTR_MAX,
                         PIN_TERMINATE};

static uint16_t seqNumber;

void* mainThread(void* arg0) {
  /* Open LED pins */
  pinHandle = PIN_open(&pinState, pinTable);
  if (pinHandle == NULL) {
    while (1);
  }
  /* Clear LED pins */
  PIN_setOutputValue(pinHandle, Board_PIN_LED1, 0);
  PIN_setOutputValue(pinHandle, Board_PIN_LED2, 0);
  // Initialize UART driver
  UART_init();
  // Initialize UART
  UART_Params uartParams;
  UART_Params_init(&uartParams);
  uartParams.baudRate = 115200;
  uartParams.readDataMode = UART_DATA_BINARY;
  uartParams.writeDataMode = UART_DATA_BINARY;
  uartParams.readReturnMode = UART_RETURN_FULL;
  uartParams.readTimeout = 1000;
  uartParams.readEcho = UART_ECHO_OFF;
  uartParams.readMode = UART_MODE_BLOCKING;
  uartParams.writeMode = UART_MODE_BLOCKING;
  uartHandle = UART_open(Board_UART0, &uartParams);
  if (uartHandle == NULL) {
    while (1);
  }

  // Initialize EasyLink
  EasyLink_Params easyLink_params;
  EasyLink_Params_init(&easyLink_params);
  if (EasyLink_init(&easyLink_params) != EasyLink_Status_Success) {
    while (1);
  }
  EasyLink_setFrequency(862000000);
  EasyLink_setRfPower(14);

  // 5 second startup delay - sync with MSP430
  PIN_setOutputValue(pinHandle, Board_PIN_LED1, 1);
  sleep(5);
  PIN_setOutputValue(pinHandle, Board_PIN_LED1, 0);
  sleep(1);

  char rxBuffer[32];
  char txBuffer[16];
  uint16_t fallback_counter = 0;  // For testing

  while (1) {
    // LISTENING MODE: Wait for RF packet
    EasyLink_RxPacket rxPacket = {{0}, 0, 0, 0, 0, {0}};
    rxPacket.rxTimeout = EasyLink_ms_To_RadioTime(5000);  // 5 second timeout

    uint16_t received_number = fallback_counter;  // Default fallback

    if (EasyLink_receive(&rxPacket) == EasyLink_Status_Success) {
      // Got RF packet
      PIN_setOutputValue(pinHandle, Board_PIN_LED1,
                         1);  // LED1 ON - RF received
      char* rf_payload = (char*)&rxPacket.payload[2];
      received_number = (uint16_t)atoi(rf_payload);
      PIN_setOutputValue(pinHandle, Board_PIN_LED1, 0);  // LED1 OFF
    }

    // 200 milliseconds delay for MSP to start listening
    usleep(200000);

    // Send received number to MSP via UART
    sprintf(txBuffer, "%u\r\n", received_number);
    UART_write(uartHandle, txBuffer, strlen(txBuffer));

    // Toggle LED2 to show we sent to MSP
    PIN_setOutputValue(pinHandle, Board_PIN_LED2,
                       !PIN_getOutputValue(Board_PIN_LED2));

    // Now wait for MSP to send back the response
    memset(rxBuffer, 0, sizeof(rxBuffer));
    int bytesRead = UART_read(uartHandle, rxBuffer, sizeof(rxBuffer) - 1);

    if (bytesRead > 0) {
      uint16_t response_number = (uint16_t)atoi(rxBuffer);
      fallback_counter = response_number;

      // Send response back via RF
      EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};
      txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
      txPacket.payload[1] = (uint8_t)(seqNumber++);
      sprintf((char*)&txPacket.payload[2], "%u", response_number);
      txPacket.len = RFEASYLINKTXPAYLOAD_LENGTH;
      txPacket.dstAddr[0] = 0xaa;

      EasyLink_transmit(&txPacket);
    }
  }
}

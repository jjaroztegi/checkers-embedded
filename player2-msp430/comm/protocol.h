#ifndef COMM_PROTOCOL_H_
#define COMM_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

void send_string(const char* str);
bool receive_string(char* buffer, int max_len, uint32_t timeout_cycles);
void send_number(uint16_t number);
bool receive_number(uint16_t* number, uint32_t timeout_cycles);

#endif /* COMM_PROTOCOL_H_ */

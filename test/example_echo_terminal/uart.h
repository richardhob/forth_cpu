
#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#define START     (1u)
#define DATA      (8u)
#define STOP      (2u)
#define COOLDOWN  (1u)
#define OSR      (16u)

void receive(uint8_t data);
void transmit(uint8_t * data);

#endif  // UART_H_

// EOF


#include "Vecho_terminal.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#include "uart.h"

extern Vecho_terminal * tb;
extern void tick(void);

void receive(uint8_t data)
{
    tb->i_rx = 1; // RX is active LOW

    tick();

    tb->i_rx = 0;
    for (uint32_t i = 0; i < (START*OSR); i++) tick();

    uint8_t value;
    for (uint32_t i = 0; i < DATA; i++)
    {
        // We can't use ~ here ... Verilator gets upset when we do that?
        value = !(data & 0x01);
        tb->i_rx = value;

        data = data >> 1;
        for (uint32_t j = 0; j < OSR; j++) tick();
    }

    // At this point the RX Ready line should be good. We'll set the i_rx to the
    // default state to prevent dummy messages.
    //
    // The STOP and COOLDOWN parts don't really care what the state of the RX
    // line is.
    tb->i_rx = 1;
}

void transmit(uint8_t * data)
{
    // TX Should start as soon as it receives the Ready signal from the RX side.
    for (uint32_t i = 0; i < (START * OSR); i++) tick();

    for (uint32_t i = 0; i < DATA; i++)
    {
        for (uint32_t j = 0; j < OSR; j++)
        {
            if (j == (OSR/2)) *data = (*data & (~(1 << i))) | ((~tb->o_tx) << i);
            tick();
        }
    }

    for (uint32_t i = 0; i < (STOP * OSR); i++) tick();
    for (uint32_t i = 0; i < (COOLDOWN * OSR); i++) tick();
}

// EOF

# UART 

UART is an ACTIVE LOW two wire communication protocol. Each side generates its
own clock, and samples the signal and stuff. More information on the protocol
specifics can be found on Wikipedia.

The short short version: start bit, data bits (LSB to MSB), and stop bits. For
our efforts:

- one start bit
- eight data bits
- two stop bits

For my efforts here, it was easiest to implement the RX and TX as separate
modules (`uart_rx` and `uart_tx`). I used state machines for both, since this
made everything much easier to adjust.

I also decided to use 'normal' logic signal in the underlying blocks, since this
made it easier for me to debug and test things. Which means that when using this
block, you have to invert the polarity of the RX and TX lines.

Reset is active high as well, so invert that probably into the block. Also,
the clock line is expected to be divided appropriately.

## TX

See:

- `uart_tx.v`

Parameters:

- START : Number of UART START Bits (Typically 1)
- DATA : Number of UART DATA Bits (Typically 8)
- STOP : Number of UART STOP Bits (Typically 2)
- COOLDOWN : Number of bits to wait before the next transmission starts (Tyically 1)
- OSR : Number of samples to take for each bit.

Clock Input:

- `i_divided_clk` -> Divided main clock. It is expected that the provided clock
  is the buad rate * OSR. 

Normal Inputs:

- `i_rst`
- `i_en`

UART Signals:

- `i_data` : DATA to transmit (`[DATA-1:0]`)
- `i_ready` : Start the Transmitter
- `o_next` : Transmission is Done
- `o_tx` : UART TX Line

Debug Signals:

- `d_state` : UART TX State 
- `d_data` : Internal DATA register (cached from `i_data`)

### Reset Procedure
### Inputs
### Behavior


## RX

See:

- `uart_rx.v`

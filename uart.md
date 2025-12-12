# UART 

UART is an ACTIVE LOW two wire communication protocol. Each side generates its
own clock, and samples the signal and stuff. More information on the protocol
specifics can be found on Wikipedia.

The short short version: start bit, data bits (LSB to MSB), and stop bits. For
our efforts:

- one start bit
- eight data bits
- two stop bits

## Implementation

For my efforts here, it was easiest to implement the RX and TX as separate
modules (`uart_rx` and `uart_tx`). I used state machines for both, since this
made everything much easier to adjust.

I also decided to use 'normal' logic signal in the underlying blocks, since this
made it easier for me to debug and test things. Which means that when using this
block, you have to invert the polarity of the RX and TX lines.

Reset is active high as well, so invert that probably into the block. Also,
the clock line is expected to be divided appropriately.

## Example 1: RX to TX

Connect the RX directly to the TX, so all messages are echo'd back.

## Example 2: RX to Tokenizer to TX

Connect the RX to the Tokenizer (looks for '\n'). When a complete line is
detected, the TX is triggered and the message is echo'd back.

## Example 3: RX to Tokenizer to Parser to TX
## Example 4: RX to Tokenizer to Parser to CPU to TX

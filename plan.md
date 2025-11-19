# Plan

Using Verilog and the OSS CAD Suite, let's build a working Forth CPU.

## Hardware

There are a few parts of the hardware design we need to figure out:

- Input / Output 
- String Parsing
- Computations

I think we'll try to use UART for input / output. The Zynq has access to a UART
to USB dude, so we can use that (potentially).

------

**__NOTE__**

Is it worth putting the peripherals on a Wishbone bus? And connecting them to
the CPU using that? Maybe learn all about memory mapper peripherals and such?

Maybe I'll look through the ZipCPU Wishbone bits and determine if I wanna do
that. The big advantage to using Wishbone is that I can (potentially) steal
other peoples peripherals maybe?

------

### UART 

Infomation about UART can be found on Wikipedia:

    https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter

Requires:

- Internal Clock 
- Connection to TX RX lines
- Shift Register

UART is a full duplex communication system, in which data can be transmitted
simultaneously on TX and RX.

What makes UART interesting is that each side generates its own clock.

### Input Lexing

From RX buffer:

- take until space
- load token in CPU
- If token was invalid, dump buffer 

### CPU

Do work with provided tokens. 

- If a token is invalid
    - notify the Input lexing
    - Write an error message with information to Output

### Output Printing


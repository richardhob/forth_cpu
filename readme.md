# Forth CPU

Let's write a (simple?) Forth CPU, which uses a few stacks to keep track of
things instead of explicit registers.

## Key Features

- 8 bits 
- Von Neuman Architecture 
    - Single Data / Instruction Bus

## Plan

Using Verilog and the OSS CAD Suite, let's build a working Forth CPU.

### Hardware

There are a few parts of the hardware design we need to figure out:

- Input / Output 
- String Parsing
- Computations

I think we'll try to use UART for input / output. The Zynq has access to a UART
to USB dude, so we can use that (potentially).

#### UART 

Infomation about UART can be found on Wikipedia:

    https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter

Requires:

- Internal Clock 
- Connection to TX RX lines
- Shift Register

UART is a full duplex communication system, in which data can be transmitted
simultaneously on TX and RX.

What makes UART interesting is that each side generates its own clock.

#### Input Lexing

From RX buffer:

- take until space
- load token in CPU
- If token was invalid, dump buffer 

#### CPU

Do work with provided tokens. 

- If a token is invalid
    - notify the Input lexing
    - Write an error message with information to Output

#### Output Printing

### Opcodes



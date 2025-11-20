# UART 

Let's first set up a UART, which will echo what was typed on the RX line back
onto the TX line.

For this project, we'll need to create:

1. Timer / Counter
2. Clock divider
3. Shift Register
4. FIFO Stack
5. Diff

We'll write this in Verilog for now, and test it with Verilator and Yosys (sby?)
hopefully.

## Timer Counter 

One shot counter / timer with a separate `start` signal and a parameter
threshold. Pretty simple. 

To use as a counter, route the signal you wish to count into the `i_clk` line,
and enable / start the block (MUST be posedge signal).

To use as a timer, route the signal to start and enable the block (use a clock
that is convenient too).

The provided `THRESHOLD` to this block is actually "+1" in practice? so
definitely keep this in mind.

## Clock Divider

Divide the provided `i_clk` into an `o_clk` using a simple parameter
(`DIVIDER`). This block should be used to divide down the FPGA clock into a
clock that can be used to sample the UART signal. 

This module doesn't do anything fancy - just specify the divider as a parameter,
and the `o_clk` should be the value. The specified divider should be over 0 -
passing through a clock is not really supported.

## Shift Register

On every clock, sample the input wire and shift it into the output data.

## Diff

Determine if the input value has changed from the last clock. If it has changed,
output a pulse.

## FIFO Stack

# Acutal UART Parts

Now that we have the base components designed, we can build "bigger" things. The
UART can be broken up into three stages:

1. First Bit Detector
2. Data Bits Collector
3. Stop Bit Counter
4. FIFO

## First Bit Detector

Find the first UART bit on the RX line. To make sure this isn't a 'pulse' and is
in fact a real bit, I included a 'pulse_timer' which will reset the block if the
pulse is not long enough. 

Parameters:

-OSR = Over Sample Ratio - how many `i_clk` pulses we get for each bit 

Inputs:
- `i_en` = Enable the block
- `i_rst` = Reset the block
- `i_clk` = UART Clock (OSR * BaudRate)
- `i_rx` = UART RX Line

Output:
- `o_found` = Start bit found

```
              first_bit
              ===========
              |<counter>|
              |<T=OSR/2>|
       i_rx-->|start    |
i_en & i_rx-->|en      o|---->o_found
actually_reset|rst      | 
       i_clk->|>clk     |
              -----------

              pulse_timer     
              ===========     
              |<counter>|     
             |<T=OSR/2+2>|  
       i_rx-->|start    |     
       i_en-->|en      o|---->pulse_timer_expired
actually_reset|rst      |
       i_clk->|>clk     |
              -----------
```

Where:

- `actually_reset = i_rst | pulse_timer_expired | !i_rx`

This block is designed to output a pulse every time there is a valid "high" bit
in the `i_rx` input. A valid high bit is when a pulse is detected which is OSR
counts long (based on the provided `i_clk`).

From a system perspective, this block is to be used as follows:

```
          uart_first_bit
          ================
   i_en-->|en            |
   i_rst->|rst           |
   i_rx-->|rx            |
          |         found|---->start_bit_found
   d_clk->|>clk          |
          ----------------
```

A little extra circuitry should be used to make sure that more `found` pulses
don't happen:

```
    reg running;
    initial running = 0;

    assign uart_fist_bit_enable = !running

    always @(posedge i_rst or posedge d_clk)
    begin
        if (i_rst) running <= 0;
        if (start_bit_found && !running) running <= 1;
        else if (stop_bits_found && running) running <= 0;
    end
```

Or something like that. Basically we should disable the first bit detector when
the first bit is detected.

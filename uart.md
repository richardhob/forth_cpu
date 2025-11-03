# UART 

Let's first set up a UART, which will echo what was typed on the RX line back
onto the TX line.

For this project, we'll need to create:

1. Timer / Counter
2. Clock divider
3. FIFO Stack

We'll write this in Verilog for now, and test it with Verilator and Yosys (sby?)
hopefully.

## Timer / Counter

A Timer will take the input signal, and, after a period of time,
assert the output signal:

```
      Timer
      =============
EN  ->|           |->LINE
RST ->|           |
CLK ->|           |
      -------------

Where:

EN   = Enable
RST  = Reset (Active Low)
CLK  = Clock
LINE = Period has happened 
```

A counter on the other hand, will count the events:

```
      Counter
      =============
EN  ->|           |->LINE
RST ->|           |
IN  ->|           |
      -------------

Where:

EN   = Enable
RST  = Reset (Active Low)
IN   = Counter Input
LINE = Count Threshold has been reached
```

Similar right? There are applications that involve both ... but I won't explore
those for now. 

The timer will be useful for delaying things. Say I need to wait for something
to happen, having a little timer dude would be helpful.

Counter is similar - if I know there must be 10 pulses, then having a counter
will be nice.

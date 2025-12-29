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

- `i_rst` : Active High
- `i_en`

UART Signals:

- `i_data` : DATA to transmit (`[DATA-1:0]`)
- `i_ready` : Start the Transmitter
- `o_next` : Ready for new data
- `o_tx` : UART TX Line

Debug Signals:

- `d_state` : UART TX State 
- `d_data` : Internal DATA register (cached from `i_data`)

### STATES

``` verilog
localparam STATE_RESET    = 0;
localparam STATE_IDLE     = 1;
localparam STATE_START    = 2;
localparam STATE_DATA     = 3;
localparam STATE_STOP     = 4;
localparam STATE_COOLDOWN = 5;
```

### Initialization

1. Set `i_rst` high and then low
2. Set `i_en` high

### Using the TX Block

Set the `i_data` lines to the desired value, then set `i_ready` high. The
`i_data` lines are cached into `d_data` and the state machine is started. The
UART signal will be transmitted on `o_tx`.

When transmission is complete, the next state returns to IDLE and the next
data sample can be processed.

### TX Block Details

Pretty simple state machine!

#### STATE IDLE

Wait for `i_ready` to be asserted. In this state, `o_next` is set high to
indicate the UART TX Block is ready for the next piece of data.

When `i_ready` is high:

1. `i_data` is saved to `d_data`
2. `o_next` is set to 0
3. `start_bits` is set to 0
4. `d_state` is set to START

`start_bits` is used in `STATE_START` to determine when to transition to the
next state.

#### STATE START

Count `i_divided_clk` pulses until `START_THRESHOLD - 1`. `START_THRESHOLD` is
calculated based on the input parameters `START` and `OSR`:

$$ START THRESHOLD = START * OSR $$

While `start_bits` is less than `START_THRESHOLD - 1`, `o_tx` is set high.

When `start_bits` is equal to `START_THRESHOLD - 1`:

1. `o_tx` is set to `d_data[0]` - the first data bit
2. `data_bits` is set to 0
3. `d_state` is set to DATA

`data_bits` is used in the `STATE_DATA` to determine which bit to transmit on
the `o_tx` as well as when to transition to the next state.

#### STATE DATA

Count `i_divided_clk` pulses until `DATA_THRESHOLD - 1`. `DATA_THRESHOLD` is
calculated based on the input parameters `DATA` and `OSR`, similar to
`START_THRESHOLD`:

$$ DATA THRESHOLD = DATA * OSR $$

While `data_bits` is less than `DATA_THRESHOLD - 1`, `o_tx` is set to the value
in `d_data` shifted by `OSR_BITS`:

``` verilog
o_tx <= d_data[data_bits + 1 >> OSR_BITS];
```

Where:

- $OSR BITS = log_{2} {OSR}$

When `data_bits` is equal to `DATA_THRESHOLD - 1`:

1. `o_tx` is set to 1 for the stop bits
2. `stop_bits` is set to 0
3. `d_state` is set to STOP

`stop_bits` is used in the `STATE_STOP` to determine when to transition to the
next state.

#### STATE STOP

Count `i_divided_clk` pulses until `STOP_THRESHOLD - 1`. `STOP_THRESHOLD` is
calculated based on the input parameters `STOP` and `OSR`, similar to
`START_THRESHOLD` and `DATA_THRESHOLD`.

$$ STOP THRESHOLD = STOP * OSR $$

While `stop_bits` is less than `STOP_THRESHOLD - 1`, `o_tx` is set to 1.

When `stop_bits` is equal to `STOP_THRESHOLD - 1`:

1. `o_tx` is set to 0
2. `cooldown_bits` is set to 0
3. `d_state` is set to COOLDOWN

`cooldown_bits` is the same story as the other bit counters in other states
basically.

#### STATE COOLDOWN

Count `i_divided_clk` pulses until `COOLDOWN_THRESHOLD - 1`. Same stuff:

$$ COOLDOWN THRESHOLD = COOLDOWN * OSR $$

While `cooldown_bits` is less than `COOLDOWN_THRESHOLD - 1`, `o_tx` is set to 0.

When `cooldown_bits` is equal to `COOLDOWN_THRESHOLD - 1`, `d_state` is set to
`STATE_IDLE`.

## RX

See:

- `uart_rx.v`

Parameters:

- START : Number of UART START Bits (Typically 1)
- DATA : Number of UART DATA Bits (Typically 8)
- STOP : Number of UART STOP Bits (Typically 2)
- OSR : Number of samples to take for each bit.

Clock Input:

- `i_divided_clk` -> Divided main clock. It is expected that the provided clock
  is the buad rate * OSR. 

Normal Inputs:

- `i_rst` : Active High
- `i_en`

UART Signals:

- `i_rx` : Input UART RX signal
- `o_data` : Received UART Data, updated only when data is ready
- `o_ready` : UART Data is ready for parsing

Debug Signals:

- `d_state` : UART rX State 
- `d_data` : Internal DATA register, updated as data is parsed

### STATES

```
localparam STATE_RESET              =  0;
localparam STATE_IDLE               =  1;
localparam STATE_START_DEBOUNCE     =  2;
localparam STATE_START_VALID        =  3;
localparam STATE_START              =  4;
localparam STATE_DATA_D0            =  5;
localparam STATE_DATA_D1            =  6;
localparam STATE_DATA_D2            =  7;
localparam STATE_DATA_D3            =  8;
localparam STATE_DATA_D4            =  9;
localparam STATE_DATA_D5            = 10;
localparam STATE_DATA_D6            = 11;
localparam STATE_DATA_D7            = 12;
localparam STATE_DATA_END           = 13;
localparam STATE_STOP               = 14;
```

### Initialization

1. Set `i_rst` high and then low
2. Set `i_en` high

### Using the RX Block

...

### RX Block Details

Simple state machine! Mostly the same as `uart_tx`. The two most interesting
bits of the block are the input debouncer and the data sampling.

To make sure input is not just a pulse, included is a DEBOUNCE threshold state.
This is set to be `1/4` of the `START_THRESHOLD` (which is `START * OSR` as in
`uart_tx`). In this state, the value of `i_rx` is ignored.

After debouncing, in `STATE_START_VALID`, the `i_rx` line must be 1 until
`START_THRESHOLD_OK` (which is `1/2` of the `START_THRESHOLD`). 

Finally the RX parsing can start.

When we get to Data parsing - data is sampled as close to the middle of the bit
as possible. Assuming that OSR is 16:

- D1 is sampled at 8 divided clocks
- D2 is sampled at 24 (8+16) divided clock
- D3 is sampled at ... (continues until D7)

States are mostly the same as in the TX otherwise!

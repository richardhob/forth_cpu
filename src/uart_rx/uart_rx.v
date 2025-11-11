// UART RX bits
// 
// Clocking bits:
//
// ```
// i_clk-->[divider]-->divided_clk
// ```
//
// First bit:
//
// ```
//               counter0
//               ===========
//               | T=OSR/2 |
//        i_rx-->| start   |
//        i_rx-->| en     o|----+===>|c0.o -> start_bit_found
//      reset1-->| rst     |    |    |!c0.o & c1.o -> no_s.b.rst
// divided_clk-->|>clk     |    |
//               -----------    |    |no_s.b.rst | sample_data_bit -> reset1
//                              |
//               counter1       |
//               ===========    |
//               | T=OSR   |    |
//        i_rx-->| start   |    |
//          HI-->| en     o|----+
//      reset1-->| rst     |
// divided_clk-->|>clk     |
//               -----------
// ```
//
// Data bits:
//
// ```
//                   counter3
//                   ===========
//                   | T=OSR   |
// start_bit_found-->| start   |
// !end_of_data_bits>| en     o|---->sample_data_bit
// sample_data_bit-->| rst     |
//     divided_clk-->|>clk     |
//                   -----------
//
//                   counter4
//                   ===========
//                   | T=8     |
// start_bit_found-->| start   |
//              HI-->| en     o|---->end_of_data_bits
// end_of_data_bits->| rst     |
// sample_data_bit-->|>clk     |
//                   -----------
//
//                   counter5
//                   ===========
//                   | T=2     |
// end_of_data_bits->| start   |
//              HI-->| en     o|---->end_of_stop_bits
// end_of_stop_bits->| rst     |
//     divided_clk-->|>clk     |
//                   -----------
// ```
//
// Shift Register:
//
// ```
//                   shift      
//                   ===========
// sample_data_bit-->| en      |
//              ??-->| rst    o|---->[8:0] result
//            i_rx-->| in      |
//           i_clk-->|>clk     |
//                   -----------
// ```
//
// Fifo ...
//
//

module uart_rx(i_clk, i_rst, i_en, i_rx, o_data, o_new_data);

parameter START_BITS  = 1; // UART Start Bits
parameter DATA_BITS   = 8; // UART Data Bits
parameter STOP_BITS   = 2; // UART Stop Bits
parameter PARITY_BITS = 0; // UART Parity Bits

parameter CLOCK_RATE = 120000000; // Hz
parameter BAUDRATE   = 115200;    // Baud rate (bits / second)
parameter OSR        = 16;        // Over Sample Ratio

localparam TOTAL_BITS = DATA_BITS + STOP_BITS + START_BITS + PARITY_BITS;
localparam DIVIDER_RATIO = CLOCK_RATE / (BAUDRATE * TOTAL_BITS);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_rx;

output reg [DATA_BITS-1:0] o_data;
initial o_data = 0;

output reg o_new_data;
initial o_new_data = 0;

// 

wire divided_clk;

reg start_bit_found;
initial start_bit_found = 0;

wire start_bit_valid;
wire start_bit_timeout;


// UART Setup
//
// 1. Clock Divider [X]
// 2. First Bit Counter [X]
// 3. First Bit Timer [ ]
// 4. Package Timer [ ]
// ?. If First Bit is long enough:
//    a. Save N bit packet (o_data)
//    b. set o_new_data

// Divide the clock down to a multiple (ish) of the desired baud rate
clock_divider #(
    .DIVIDER=DIVIDER_RATIO
) uart_clk (
    .i_rst(i_rst),
    .i_clk(i_clk),
    .o_clk(divided_clk)
);

// Is the first bit wider than half a bit (AKA OSR / 2)?
//
// TODO: Add "one shot" feature to counter
counter #(
    .THRESHOLD=(OSR/2), // More than half a bit
    .CYCLES=1
) start_bit_counter (
    .i_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(start_bit_found),
    .o_line(start_bit_valid)
);

// When do we sample the waveforms?
timer #(
    .THRESHOLD=OSR // A whole bit
    .CYCLES=DATA_BITS // Sample 8 Bits
) sample_timer (
    .i_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(start_bit_valid),
    .o_line(start_bit_timeout)
);

// If the first bit is NOT wider than half a bit, reset it.
timer #(
    .THRESHOLD=OSR // A whole bit
) start_bit_timer (
    .i_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(start_bit_found),
    .o_line(start_bit_timeout)
);

// Packet timer
timer #(
    .THRESHOLD=OSR * TOTAL_BITS // A whole packet
) start_bit_timer (
    .i_clk(divided_clk),
    .i_rst(i_rst | start_bit_timeout),
    .i_en(start_bit_found),
    .o_line(packet_timer)
):

always @(negedge i_rx)
    if (i_en) start_bit_found <= 1;

endmodule

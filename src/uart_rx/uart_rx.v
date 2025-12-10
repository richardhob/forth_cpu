// UART RX bits
// 
// Clock divider -> clock_divider.v
//
// ```
// i_clk-->[divider]-->divided_clk
// ```
//
// First bit -> uart_first_bit.v:
//
// ```
//           uart_first_bit
//           ================
//    i_en-->|en            |
//    i_rst->|rst           |
//    i_rx-->|rx            |
//           |         found|---->start_bit_found
//    d_clk->|>clk          |
//           ----------------
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
// Fifo:
//
// ```
//                   fifo
//                   ===========
//             HI--->| en      |
//            ???--->| rst     |
//   [...] result--->| i_data  |
// end_of_data_bits->| i_set   |
//          o_set--->| o_set   |
//     divided_clk-->|>clk     |
//                   |    i_get|--->|To other places...
//                   |    o_get|--->|
//                   -----------
// ```
//
// Samples in Fifo:
//
// ```
//                   counter5
//                   ===========
//                   | T=FIFO.D|
// end_of_data_bits->| start   |
//              HI-->| en     o|---->fifo_full
//              ??-->| rst     |
//     divided_clk-->|>clk     |
//                   -----------
// ```

module uart_rx(i_clk, i_rst, i_en, i_rx, o_data, o_ready);

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

output reg o_ready;
initial o_ready = 0;

wire divided_clk;

// Divide the clock down to a multiple (ish) of the desired baud rate
clock_divider #(
    .DIVIDER(DIVIDER_RATIO)
) uart_clk (
    .i_rst(i_rst),
    .i_clk(i_clk),
    .o_clk(divided_clk)
);

reg running;
initial running = 0;

always @(posedge i_rst or posedge i_clk)
begin
    if (i_rst) running <= 0;
    else if (!running && start_bit_found) running <= 1;
    else if ( running && stop_bits_found) running <= 0;

wire start_bit_found;

uart_first_bit #(
    .OSR=OSR
) first_bit (
    .i_rst(i_rst),
    .i_clk(divided_clk),
    .i_en(!running),
    .i_rx(i_rx),
    .o_found(start_bit_found)
);

uart_data_bits #(
    .OSR=OSR,
    .DATA_BITS=DATA_BITS
) data_bits (
    .i_rst(i_rst),
    .i_clk(divided_clk),
    .i_en(running),
    .i_rx(i_rx),
    .i_start(start_bit_found),
    .o_data(o_data),
    .o_done(o_ready)
);

uart_stop_bits #(
    .OSR=OSR,
    .STOP_BITS=STOP_BITS
) stop_bits (
    .i_rst(i_rst),
    .i_clk(divided_clk),
    .i_en(running),
    .i_rx(i_rx),
    .i_start(o_ready),
    .o_done(stop_bits_found)
);

endmodule

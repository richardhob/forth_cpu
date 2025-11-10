
module uart_rx(i_rst, i_en, i_rx, i_clk, o_data, o_new_data);

parameter WIDTH = 8;       // UART Data Bits
parameter STOP_BITS = 2;   // UART Stop Bits
parameter START_BITS = 1;  // UART Start Bits
parameter PARITY_BITS = 0; // UART Parity Bits

parameter CLOCK_RATE = 120000000; // Hz
parameter BAUDRATE = 115200;      // bps
parameter OSR = 16;               // Over Sample Ratio

localparam TOTAL_BITS = WIDTH + STOP_BITS + START_BITS + PARITY_BITS;
localparam DIVIDER_RATIO = CLOCK_RATE / (BAUDRATE * TOTAL_BITS);

input wire rx_i;
input wire clk_i;

wire divided_clk;

reg start_bit_found;
initial start_bit_found = 0;

wire start_bit_valid;
wire start_bit_timeout;

output reg [WIDTH-1:0] o_data;
initial o_data = 0;

output reg o_new_data;
initial o_new_data = 0;

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
    .CYCLES=WIDTH // Sample 8 Bits
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

// First UART Bit
//
// Find the first UART bit on the RX line. To make sure this isn't a 'pulse' and
// is in fact a real bit, I included a 'pulse_timer' which will reset the block
// if the pulse is not long enough. 
//
// Parameters:
// -OSR = Over Sample Ratio - how many "i_clk" pulses we get for each bit 
//
// Inputs:
// -i_en = Enable the block
// -i_rst = Reset the block
// -i_clk = UART Clock (OSR * BaudRate)
// -i_rx = UART RX Line
//
// Output:
// -o_found = Start bit found
//
// ```
//               first_bit
//               ===========
//               |<counter>|
//               |<T=OSR/2>|
//        i_rx-->|start    |
// i_en & i_rx-->|en      o|---->o_found
// actually_reset|rst      | 
//        i_clk->|>clk     |
//               -----------
//
//               pulse_timer     
//               ===========     
//               |<counter>|     
//              |<T=OSR/2+2>|  
//        i_rx-->|start    |     
//        i_en-->|en      o|---->pulse_timer_expired
// actually_reset|rst      |
//        i_clk->|>clk     |
//               -----------
// ```
//
// where:
// - actually_reset = i_rst | pulse_timer_expired | !i_rx
//
// Simple!

module uart_first_bit(i_rst, i_clk, i_en, i_rx, o_found);

// i_clk here should be the divided clock

parameter  OSR      = 16;      // oversample ratio
localparam HALF_OSR = OSR / 2; // Half OSR
localparam TIMER    = OSR > 4 ? HALF_OSR + 2 : HALF_OSR + 1;

input wire i_rst;
input wire i_clk;
input wire i_en;
input wire i_rx;

output reg o_found;
initial o_found = 0;

wire first_bit_found;
wire pulse_timer_expired;

// Reset if i_rx is low
wire actually_reset;
assign actually_reset = i_rst | pulse_timer_expired | !i_rx;

assign o_found = first_bit_found;

wire first_bit_enable;
assign first_bit_enable = i_rx & i_en;

counter #(.THRESHOLD(HALF_OSR)) first_bit (
    .i_clk(i_clk),
    .i_rst(actually_reset),
    .i_en(first_bit_enable),
    .i_start(i_rx),
    .o_line(first_bit_found)
);

counter #(.THRESHOLD(TIMER)) pulse_timer (
    .i_clk(i_clk),
    .i_rst(actually_reset),
    .i_en(i_en),
    .i_start(i_rx),
    .o_line(pulse_timer_expired)
);

endmodule

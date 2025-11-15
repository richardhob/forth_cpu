
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

wire actually_reset;
assign actually_reset = i_rst | pulse_timer_expired;

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

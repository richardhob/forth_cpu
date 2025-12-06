
module uart_stop_bits(i_clk, i_rst, i_en, i_rx, i_start, o_ready);

// This ends up being longer the 2 full bits ... which I think will be OK.
parameter OSR       = 16;
parameter STOP_BITS =  2;

input wire i_clk;
input wire i_rst;
input wire i_en;
input wire i_rx;
input wire i_start;

output wire o_ready;

assign o_ready = _o_ready;
wire _o_ready;

uart_data_bits #(
    .OSR(OSR),
    .DATA_BITS(STOP_BITS)
) stop_bits (
    .i_rx(i_rx),
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_start(i_start),
    .o_data(),
    .o_ready(_o_ready)
);

endmodule

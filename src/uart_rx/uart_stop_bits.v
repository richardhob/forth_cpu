
module uart_stop_bits(i_clk, i_rst, i_en, i_rx, i_start, o_ready);

parameter OSR       = 16;
parameter STOP_BITS =  8;

input wire i_clk;
input wire i_rst;
input wire i_en;
input wire i_rx;
input wire i_start;

output reg o_ready;

assign o_ready <= _o_ready;

wire _o_ready;

uart_data_bits #(
    .OSR(OSR),
    .DATA_BITS(STOP_BITS)
) stop_bits (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_start(i_start),
    .o_data(),
    .o_ready(_o_ready)
);

endmodule


module uart_rx_to_tx (i_clk, i_rst, i_en, i_rx, o_tx);

parameter OSR = 16;

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_rx;
output reg o_tx;

wire [7:0] rx_data;
wire rx_data_ready;

reg _i_rx;
assign _i_rx = ~i_rx;

// Set up RX
uart_rx #(
    .OSR(OSR)
) rx (
    .i_divided_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_rx(_i_rx),
    .o_data(rx_data),
    .o_ready(rx_data_ready),
    .d_state(),
    .d_data()
);

wire tx_next;
wire tx_ready;
wire [7:0] tx_data;
wire _o_tx;

assign o_tx = ~_o_tx;

// Set up TX
uart_tx #(
    .OSR(OSR)
) tx (
    .i_divided_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_data(rx_data),
    .i_ready(rx_data_ready),
    .o_next(),
    .o_tx(_o_tx),
    .d_state(),
    .d_data()
);

endmodule

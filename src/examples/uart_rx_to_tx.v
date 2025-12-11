
module uart_rx_to_tx (i_clk, i_rst, i_rx, o_tx);

input wire i_clk;
input wire i_rst;
input wire i_rx;

output reg o_tx;

wire divided_clk;

parameter CLOCK    = 125000000;
parameter BAUDRATE = 115200;
parameter OSR      = 16;
localparam DIVIDER = (CLOCK / (OSR * BAUDRATE));

// Divide the clock
clock_divider #(
    .DIVIDER(DIVIDER)
) divider (
    .i_rst(i_rst), 
    .i_clk(i_clk),
    .o_clk(divided_clk)
);

wire [7:0] rx_data;
wire rx_data_ready;

reg _i_rx;
assign _i_rx = ~i_rx;

// Set up RX
uart_rx #(
    .OSR(OSR)
) rx (
    .i_divided_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(1'b1),
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
    .i_divided_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(1'b1),
    .i_data(rx_data),
    .i_ready(rx_data_ready),
    .o_next(),
    .o_tx(_o_tx),
    .d_state(),
    .d_data()
);

endmodule


module echo_terminal(i_clk, i_rst, i_en, i_rx, o_tx);

// Example echo terminal, which runs the UART and TERMINAL block at the same
// speed (Which is allowed I think? terminal can be a bit faster clocked if
// desired).

parameter OSR = 16;

input wire i_clk; // UART Clock with OSR factored in
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

terminal #(
    .LENGTH(256),
    .LINES(2)
) term (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),

    .i_rx_data(rx_data),
    .i_rx_ready(rx_data_ready),

    .i_tx_next(tx_next),
    .o_tx_data(tx_data),
    .o_tx_ready(tx_ready),

    // Tokenizer Outputs
    .o_line(),
    .o_len(),
    .o_ready(),

    // Debug Outputs
    .d_line(),
    .d_index(),
    .d_state(),
    .d_next_state()
);

endmodule


module cpu_terminal(i_clk, i_rst, i_en, i_rx, o_tx);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_rx;
output wire o_tx;

parameter OSR             = 16;
parameter ICLOCK          = 25000000;
parameter BAUDRATE        = 9600;

parameter LINE_LENGTH     = 256; // Characters

localparam WIDTH = 8; // Number of bits in the UART packet and related
localparam DIVIDER = (ICLOCK * OSR) / BAUDRATE;
localparam LINE_LENGTH_BITS = $clog2(LINE_LENGTH);

// Clock Divider
clock_divider #(
    .DIVIDER(DIVIDER)
) uart_clk (
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
    .i_divided_clk(divided_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_data(rx_data),
    .i_ready(rx_data_ready),
    .o_next(),
    .o_tx(_o_tx),
    .d_state(),
    .d_data()
);

#local

wire [WIDTH-1:0] terminal_line [LINE_LENGTH];
wire [LINE_LENGTH_BITS:0] terminal_len;
wire terminal_ready;

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

    // to Tokenizer 
    .o_line(terminal_line),
    .o_len(terminal_len),
    .o_ready(terminal_ready),

    // Debug Outputs
    .d_line(),
    .d_index(),
    .d_state(),
    .d_next_state()
);

tokenizerL #(
    .LENGTH(LINE_LENGTH)
) tokenizer (
    .i_clk(
);

parser ();

cpu ();

endmodule

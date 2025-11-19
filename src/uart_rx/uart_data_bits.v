
module uart_data_bits(i_clk, i_rst, i_en, i_rx, i_start, o_data, o_ready);

parameter  OSR       = 16;
parameter  DATA_BITS =  8;
localparam MSG_TIMER = (OSR * DATA_BITS) + 1;

input wire i_clk;
input wire i_rst;
input wire i_en;
input wire i_rx;
input wire i_start;

output reg [DATA_BITS-1:0] o_data;
initial o_data = 0;

output reg o_ready;
initial o_ready = 0;

wire sample_ready;

reg sample_timer_reset;
assign sample_timer_reset = i_rst | sample_ready;

counter #(
    .THRESHOLD(OSR)
) sample_timer (
    .i_clk(i_clk),
    .i_rst(sample_timer_reset),
    .i_en(i_en),
    .i_start(i_start),
    .o_line(sample_ready)
);

counter #(
    .THRESHOLD(DATA_BITS)
) sample_counter (
    .i_clk(sample_ready),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_start(i_start),
    .o_line(o_ready)
);

counter #(
    .THRESHOLD(MSG_TIMER)
) message_timer (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_start(i_start),
    .o_line(message_done)
);

shift #(
    .WIDTH(DATA_BITS)
) data (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_en(i_en),
    .i_data(i_rx),
    .o_data(o_data)
);

endmodule

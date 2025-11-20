// Capture and save the UART Data bits 
//
//

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

wire counter_done;
wire timer_done;

reg running;
initial running = 0;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst) running <= 0;
    else if (i_en)
    begin
        if (counter_done || timer_done) running <= 0;
        else if (i_start) running <= 1;
    end
end

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst) o_ready <= 0;
    else if (i_en)
    begin
        if (counter_done || timer_done) o_ready <= 1;
    end
end

wire [DATA_BITS-1:0] sample_done;

genvar i;

generate 
    for (i = 0; i < DATA_BITS; i = i + 1) 
    begin
        if (i == 0)
        begin
            counter #(
                .THRESHOLD(OSR-1)
            ) sample_timer (
                .i_clk(i_clk),
                .i_rst(!running),
                .i_en(running),
                .i_start(running),
                .o_line(sample_done[i])
            );
        end
        else
        begin
            counter #(
                .THRESHOLD(OSR-1)
            ) sample_timer (
                .i_clk(i_clk),
                .i_rst(!running),
                .i_en(running),
                .i_start(sample_done[i-1]),
                .o_line(sample_done[i])
            );
        end
    end
endgenerate

wire sample_ready;

diff #(
    .DATA_WIDTH(DATA_BITS)
) ready (
    .i_clk(i_clk),
    .i_rst(!running),
    .i_en(running),
    .i_data(sample_done),
    .o_changed(sample_ready)
);

counter #(
    .THRESHOLD(DATA_BITS)
) sample_counter (
    .i_clk(sample_ready),
    .i_rst(!running),
    .i_en(running),
    .i_start(running),
    .o_line(counter_done)
);

counter #(
    .THRESHOLD(MSG_TIMER)
) message_timer (
    .i_clk(i_clk),
    .i_rst(!running),
    .i_en(running),
    .i_start(running),
    .o_line(timer_done)
);

shift #(
    .WIDTH(DATA_BITS)
) data (
    .i_clk(sample_ready),
    .i_rst(i_rst),
    .i_en(running),
    .i_data(i_rx),
    .o_data(o_data)
);

endmodule

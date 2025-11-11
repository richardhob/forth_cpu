// One shot counter / timer (depending on how you hook it up)
//
// Takes one clock to start the timer now (since adding the start bit)

module counter(i_clk, i_rst, i_en, i_start, o_line);

parameter  THRESHOLD = 255; // Clocks
localparam NBITS = $clog2(THRESHOLD) + 1;

input wire i_clk;
input wire i_rst;
input wire i_en;
input wire i_start;

output reg o_line;
initial o_line = 0;

reg started;
initial started = 0;

always @(posedge i_clk or posedge i_rst)
begin 
    if (i_rst) started <= 0;
    else if (i_en && i_start) started <= 1;

reg [NBITS-1:0] _count;
initial _count = 0;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst)
    begin
        _count <= 0;
        o_line <= 0;
    end
    else if (i_en)
    begin
        if (started) 
        begin
            if (_count < THRESHOLD) 
            begin
                o_line <= 0;
                _count = _count + 1;
            end
            else
            begin
                o_line <= 1;
            end
        end
    end
end

`ifdef FORMAL

reg f_past_valid;
initial f_past_valid = 1'b0;

reg f_past_reset;
initial f_past_reset = 1'b0;

always @(posedge i_clk)
begin
    f_past_valid <= 1'b1;
end

always @(posedge i_rst)
begin
    f_past_reset <= 1'b1;
end

always @(posedge i_clk)
    if (f_past_valid && !i_rst && i_en && $past(i_en) && started && (_count < THRESHOLD)) 
        assert($changed(_count));

always @(posedge i_clk)
    if (f_past_valid && !i_rst && i_en && !f_past_reset && started && (_count >= THRESHOLD)) 
        assert(o_line == 1);

always @(posedge i_clk)
    if (o_line == 1 && !f_past_reset && !i_rst)
        assert(o_line == 1);

always @(posedge i_clk)
    if (_count < THRESHOLD)
        assert(o_line == 0);

`endif

endmodule

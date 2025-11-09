
module counter(i_clk, i_rst, i_en, o_line);

parameter  THRESHOLD = 255; // Clocks
localparam NBITS = $clog2(THRESHOLD) + 1;

input wire i_clk;
input wire i_rst;
input wire i_en;

output reg o_line;
initial o_line = 0;

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
        if (_count < THRESHOLD) _count = _count + 1;
        else
        begin
            o_line <= 1;
            _count <= 0;
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

always @(posedge i_reset)
begin
    f_past_reset <= 1'b1;
end

always @(posedge i_clk)
    if (f_past_valid && !i_rst && i_en && !f_past_reset) assert($changed(_count));

`endif

endmodule

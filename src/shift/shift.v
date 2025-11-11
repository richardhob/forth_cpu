
module shift(i_clk, i_rst, i_en, i_data, o_data);

parameter WIDTH = 8; // bits

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_data;

output reg [WIDTH-1:0] o_data;
initial o_data = 0;

reg index;
initial index = 0;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst) 
    begin
        o_data <= 0;
        index <= 0;
    end
    else if (i_en && index < WIDTH) 
    begin
        o_data[index] <= i_data;
        index <= index + 1;
    end
end

`ifdef FORMAL

reg f_past_valid;
initial f_past_valid = 1'b0;

always @(posedge i_clk)
begin
    f_past_valid <= 1'b1;
end

reg f_past_reset;
initial f_past_reset = 1'b0;

always @(posedge i_rst)
begin
    f_past_reset <= 1'b1;
end

always @(posedge i_clk)
    if (f_past_valid && !f_past_reset && i_en && $past(i_en) && $changed(i_data) && index < WIDTH)
        assert($changed(o_data));

always @(posedge i_clk)
    if (index >= WIDTH) 
    begin
        assert(!$changed(o_data));
        assert(!$changed(index));
    end

`endif

endmodule

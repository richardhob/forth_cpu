
module clock_divider(i_rst, i_clk, o_clk);

parameter DIVIDER = 2; // Divider amount

input wire i_rst;
input wire i_clk;
output reg o_clk;
initial o_clk = 0;

reg [31:0] counter;
initial counter = 0;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst) 
    begin
        counter <= 0;
        o_clk <= 0;
    end 
    else 
    begin
        if (counter < DIVIDER) counter = counter + 1;
        else
        begin
            o_clk <= ~o_clk;
            counter <= 0;
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
    if (f_past_valid && !i_rst && !f_past_reset) assert(counter <= DIVIDER);

`endif

endmodule

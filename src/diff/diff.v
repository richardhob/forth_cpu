
module diff(i_clk, i_rst, i_en, i_data, o_changed);

parameter DATA_WIDTH = 8;

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [DATA_WIDTH-1:0] i_data;

output reg o_changed;
initial o_changed = 0;

reg [DATA_WIDTH-1:0] temp;
initial temp = 0;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst) 
    begin
        temp <= 0;
        o_changed <= 0;
    end
    else if (i_en)
    begin
        if (temp != i_data)
        begin
            o_changed <= 1;
            temp <= i_data;
        end
        else o_changed <= 0;
    end
end

endmodule

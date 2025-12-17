
// String to Integer Dictionary
//
// -Store values
// -Look up values

module dict_si(i_clk, i_rst, i_en, i_ready, i_op, i_key, i_value, o_value, o_done);

parameter ENTRIES = 10;

parameter KEY_WIDTH = 8;
parameter KEY_LENGTH = 8;

parameter VALUE_WIDTH = 32;
parameter VALUE_LENGTH = 1;

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_ready;
input wire i_op; // 0 -> set, 1 -> get

input wire [KEY_WIDTH-1:0] i_key [KEY_LENGTH-1:0];
input wire [VALUE_WIDTH-1:0] i_value [VALUE_LENGTH-1:0];

output reg o_done;
output reg [VALUE_WIDTH-1:0] o_value [VALUE_LENGTH-1:0];

reg [KEY_WIDTH-1:0] keys [ENTRIES-1:0][KEY_LENGTH-1:0];
reg [VALUE_WIDTH-1:0] values [ENTRIES-1:0][VALUE_LENGTH-1:0];
reg index;

localparam OP_SET = 0;
localparam OP_GET = 1;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        index <= 0;
        o_done <= 0;
        o_value <= 0;
    end
    else if (1 == i_en)
    begin
        if (i_op == OP_SET)
        begin
            // Is the key in keys?
            // If yes then set the value 
        end
        else if (i_op == OP_GET)
        begin
            // TODO
        end
    end
end

endmodule

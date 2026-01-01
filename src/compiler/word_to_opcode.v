
// Convert the provided word into an OPCODE

module word_to_opcode(i_clk, i_en, i_word, i_len, o_data, o_opcode, o_err);

localparam DATA_WIDTH = 8; // Character width (also UART width)
parameter  WIDTH      = 32; // Maximum word width
parameter  DATA       = 32; // DATA width
parameter  OPCODE     = 16; // OPCODE width

localparam WIDTH_BITS = $clog2(WIDTH) + 1;

input wire i_clk;
input wire i_en;
input wire [DATA_WIDTH-1:0] i_word [WIDTH-1:0];
input wire [WIDTH_BITS - 1:0] i_len;

output reg [OPCODE-1:0] o_opcode; 
output reg [DATA-1:0] o_data; 
output reg o_err;

localparam OPCODE_IDLE = 0;

localparam OPCODE_PUSH = 1;

localparam OPCODE_POP = 2;
localparam OPCODE_ADD = 3;
localparam OPCODE_SUBTRACT= 4;
localparam OPCODE_MULTIPLY = 5;
localparam OPCODE_DIVIDE = 6;
localparam OPCODE_DUP = 7;
localparam OPCODE_ROT = 8;
localparam OPCODE_SWAP = 9;

always @(posedge i_clk)
begin
    if (1 == i_en)
    begin
        if (i_len == 1)
        begin
            o_err = 0;
            o_opcode = OPCODE_IDLE;
            o_data = 0;
            case (i_word[0])
                ".": o_opcode = OPCODE_POP;
                "+": o_opcode = OPCODE_ADD;
                "-": o_opcode = OPCODE_SUBTRACT;
                "*": o_opcode = OPCODE_MULTIPLY;
                "/": o_opcode = OPCODE_DIVIDE;
                default: o_err = 1;
            endcase
        end
        else if (i_len == 3)
        begin
            if (i_word[0] == "D" && i_word[1] == "U" && i_word[2] == "P")
            begin
                o_opcode <= OPCODE_DUP;
                o_data <= 0;
                o_err <= 0;
            end
            else if (i_word[0] == "R" && i_word[1] == "O" && i_word[2] == "T")
           begin
                o_opcode <= OPCODE_ROT;
                o_data <= 0;
                o_err <= 0;
           end
           else o_err <= 1;
        end
        else if (i_len == 4)
        begin
            if (i_word[0] == "S" && i_word[1] == "W" && i_word[2] == "A" && i_word[3] == "P")
            begin
                o_opcode <= OPCODE_SWAP;
                o_data <= 0;
                o_err <= 0;
            end
            else o_err <= 1;
        end
        else o_err <= 1;
    end
end

endmodule

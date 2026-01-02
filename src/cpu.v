
// Connects to:
//
// 1. Dictionary (TODO)
// 2. Parser (i_rx -> ... -> inputs)
// 3. Print Module (??? -> ??? -> o_tx)
//
module cpu(i_clk, i_rst, i_en,

    // Dictionary ... TODO
    // From Parser
    i_opcode, i_data, i_ready, 

    // To Printer
    o_data, o_err,

    // Debug
    d_stack, d_index
);

localparam DATA_WIDTH = 8; // Character width (also UART width)

parameter  WIDTH      = 32; // Maximum word width
parameter  DATA       = 32; // Data Width 
parameter  OPCODE     = 16; // OPCODE Width 

parameter  STACK      = 16; // STACK Depth
localparam STACK_BITS = $clog2(STACK); // STACK Depth

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [OPCODE-1:0] i_opcode;
input wire [DATA-1:0] i_data;
input wire i_ready; // OPCODE and DATA are valid

output reg [DATA-1:0] o_data;
output reg [2:0] o_err;

output reg [DATA-1:0] d_stack [STACK-1:0];
output reg [STACK_BITS-1:0] d_index;

reg [DATA-1:0] temp;

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

localparam ERROR_OK = 0;
localparam ERROR_INVALID_OPCODE = 1;
localparam ERROR_STACK_TOO_SMALL = 2;
localparam ERROR_STACK_TOO_BIG = 3;
localparam ERROR_DIVIDE_BY_ZERO = 4;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        o_data <= 0;
        o_err <= 0;

        d_index <= 0;
        for (int i = 0; i < STACK; i++)
        begin
            d_stack[i] <= 0;
        end
    end
    else if (1 == i_en)
    begin
        if (1 == i_ready)
        begin
            o_err <= ERROR_OK;
            o_data <= 0;
            case (i_opcode)
                OPCODE_IDLE: o_err <= ERROR_OK;
                OPCODE_PUSH: 
                begin
                    d_stack[d_index] = i_data;
                    d_index = d_index + 1;
                end
                OPCODE_POP:
                begin
                    if (d_index == 0) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        o_data = d_stack[d_index];
                        d_index = d_index - 1;
                    end
                end
                OPCODE_ADD:
                begin
                    if (d_index < 2) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        d_stack[d_index - 2] = d_stack[d_index - 2] + d_stack[d_index - 1];
                        d_index = d_index - 1;
                    end
                end
                OPCODE_SUBTRACT:
                begin
                    if (d_index < 2) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        d_stack[d_index - 2] = d_stack[d_index - 2] - d_stack[d_index - 1];
                        d_index = d_index - 1;
                    end
                end
                OPCODE_MULTIPLY:
                begin
                    if (d_index < 2) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        d_stack[d_index - 2] = d_stack[d_index - 2] * d_stack[d_index - 1];
                        d_index = d_index - 1;
                    end
                end
                OPCODE_DIVIDE:
                begin
                    if (d_index == 0) o_err <= ERROR_STACK_TOO_SMALL;
                    else if (d_stack[d_index-1] == 0) o_err <= ERROR_DIVIDE_BY_ZERO;
                    else
                    begin
                        d_stack[d_index - 2] = d_stack[d_index - 2] / d_stack[d_index - 1];
                        d_index = d_index - 1;
                    end
                end
                OPCODE_DUP:
                begin
                    if (d_index == 0) o_err <= ERROR_STACK_TOO_SMALL;
                    // verilator lint_off WIDTHEXPAND
                    else if (d_index >= STACK) o_err <= ERROR_STACK_TOO_BIG;
                    // verilator lint_on WIDTHEXPAND
                    else
                    begin
                        d_stack[d_index] = d_stack[d_index - 1];
                        d_index = d_index + 1;
                    end
                end
                OPCODE_ROT:
                begin
                    if (d_index < 3) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        temp = d_stack[d_index-1];
                        d_stack[d_index-1] = d_stack[d_index-2];
                        d_stack[d_index-2] = d_stack[d_index-3];
                        d_stack[d_index-3] = temp;
                        temp = 0;
                    end
                end
                OPCODE_SWAP:
                begin
                    if (d_index < 2) o_err <= ERROR_STACK_TOO_SMALL;
                    else
                    begin
                        temp = d_stack[d_index-2];
                        d_stack[d_index-2] = d_stack[d_index-1];
                        d_stack[d_index-1] = temp;
                        temp = 0;
                    end
                end
                default: o_err <= 1;
            endcase
        end
    end
end

endmodule

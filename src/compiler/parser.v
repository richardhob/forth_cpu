

// Collect the tokenized characters into words and determine how to interpret
// them.

module parser(i_clk, i_rst, i_en,
    // To tokenizer
    i_data, i_ready, i_eol, i_wc, o_next, 

    // To CPU
    o_opcode, o_data, o_ready, o_err,

    d_state, d_word, d_index,
);

localparam DATA_WIDTH = 8; // Character width (also UART width)
parameter  WIDTH      = 32; // Maximum word width

parameter  DATA       = 32; // Data Width 
parameter  OPCODE     = 16; // OPCODE Width 

localparam WIDTH_BITS = $clog2(WIDTH) + 1;
localparam DATA_BITS = $clog2(DATA) + 1;
localparam OPCODE_BITS = $clog2(OPCODE) + 1;

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [DATA_WIDTH-1:0] i_data;
input wire i_ready;
input wire i_eol;
input wire i_wc;

output reg o_next; // Get the next character

output reg [OPCODE-1:0] o_opcode; // Next Opcode to process
output reg [DATA-1:0] o_data; // Data to process
output reg o_ready; // New Opcode is available
output reg o_err; // Error parsing word

output reg [2:0] d_state;
output reg [DATA_WIDTH-1:0] d_word [WIDTH-1:0]; // Register 
output reg [WIDTH_BITS - 1:0] d_index;

reg update; // Update the parser bits

localparam STATE_IDLE = 0;

wire [DATA-1:0] int_data;
wire int_err;

word_to_int #(
    .WIDTH(WIDTH),
    .DATA(DATA)
) int_parser (
    .i_clk(i_clk),
    .i_word(d_word),
    .i_len(d_index),
    .o_data(int_data),
    .o_err(int_err),
);

wire [DATA-1:0] hex_data;
wire hex_err;

word_to_hex #(
    .WIDTH(WIDTH),
    .DATA(DATA)
) hex_parser (
    .i_clk(i_clk),
    .i_word(d_word),
    .i_len(d_index),
    .o_data(hex_data),
    .o_err(hex_err),
);

wire [DATA-1:0] opcode_data;
wire [OPCODE-1:0] opcode_opcode;
wire opcode_err;

word_to_opcode #(
    .WIDTH(WIDTH),
    .DATA(DATA),
    .OPCODE(OPCODE)
) opcode_parser (
    .i_clk(i_clk),
    .i_word(d_word),
    .i_len(d_index),
    .o_data(opcode_data),
    .o_opcode(opcode_opcode),
    .o_err(opcode_err)
);

localparam OPCODE_IDLE = 0;
localparam OPCODE_PUSH = 1;
localparam OPCODE_POP = 2;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        o_next <= 0;
        o_opcode <= 0;
        o_ready <= 0;

        d_state <= STATE_IDLE;
        d_index <= 0;
        for (int i = 0; i < WIDTH; i++) d_word[i] <= 8'b0;
    end
    else if (1 == i_en)
    begin
        if (STATE_IDLE == d_state)
        begin
            if (1 == i_ready)
            begin
                o_next <= 0;
                if (i_wc or i_eol) 
                begin
                    // Process
                    if (0 == int_err)
                    begin
                        o_data <= int_data;
                        o_opcode <= OPCODE_PUSH; 
                        o_err <= 0;
                    end
                    else if (0 == hex_err)
                    begin
                        o_data <= hex_data;
                        o_opcode <= OPCODE_PUSH;
                        o_err <= 0;
                    end
                    else if (0 == opcode_err)
                    begin
                        o_data <= opcode_data;
                        o_opcode <= opcode_opcode;
                        o_err <= 0;
                    end
                end
                else
                begin
                    d_word[d_index] = i_data;
                end
            end
            else 
            begin 
                o_next <= 1;
            end
        end
    end
end

endmodule

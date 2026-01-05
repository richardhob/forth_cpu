
module tokenizer(i_clk, i_rst, i_en, 

    // From Terminal
    i_line, i_len, i_ready, 

    // To Parser
    i_next, o_data, o_data_ready, o_eol, o_wc,
    
    // Debug
    d_line, d_len, d_index, d_state
    );

localparam WIDTH      = 8; // LOCKED as 8 for now .... since we only have 8 UART bits to work with
parameter  EOL        = "\n";
parameter  WC         = " ";
parameter  LENGTH     = 16; // Line Length (16 Characters)

localparam LENGTH_BITS      = $clog2(LENGTH);

input wire i_clk;
input wire i_rst;
input wire i_en;

// From Terminal
input wire [WIDTH-1:0] i_line [LENGTH-1:0];
input wire [LENGTH_BITS-1:0] i_len;
input wire i_ready;

// To Parser
input wire i_next;
output reg [WIDTH-1:0] o_data;
output reg o_data_ready;
output reg o_eol;
output reg o_wc;

// Debug
output reg [WIDTH-1:0] d_line [LENGTH-1:0];
output reg [LENGTH_BITS-1:0] d_index;
output reg [LENGTH_BITS-1:0] d_len;
output reg d_state;

localparam STATE_IDLE = 0;
localparam STATE_TOKENIZE = 1;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        d_state <= STATE_IDLE;

        o_data <= 0;
        o_data_ready <= 0;
        o_wc <= 0;
        o_eol <= 0;

        d_index <= 0;
        for (int i = 0; i < LENGTH; i++) d_line[i] <= 0;
    end
    else if (1 == i_en)
    begin
        if (STATE_IDLE == d_state)
        begin
            if (1 == i_ready)
            begin
                d_line = i_line;
                d_len = i_len;
                d_index = 0;
                d_state = STATE_TOKENIZE;
                o_data_ready = 1;
            end
        end

        if (STATE_TOKENIZE == d_state)
        begin
            if (d_index < d_len)
            begin
                if (i_next == 1)
                begin
                    o_data = d_line[d_index];
                    o_wc = (d_line[d_index] == " ") ? 1 : 0;
                    o_eol = (d_line[d_index] == "\n") ? 1 : 0;
                    d_index = d_index + 1;
                end
            end
            else
            begin
                d_state <= STATE_IDLE;
                d_len <= 0;
                d_index <= 0;
                o_data_ready <= 0;
                o_wc <= 0;
                o_eol <= 0;
                for (int i = 0; i < LENGTH; i++) d_line[i] <= 0;
            end
        end
    end
end

endmodule

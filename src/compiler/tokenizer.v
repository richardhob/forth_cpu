
module tokenizer(i_clk, i_rst, i_en, 
    i_data, i_ready, 

    i_next, 
    o_eol, o_wc, o_data_ready, o_data);

localparam DATA_WIDTH = 8; // LOCKED as 8 for now .... since we only have 8 UART bits to work with
parameter  WIDTH      = 32;
parameter  LINES      = 2;
parameter  EOL        = "\n";
parameter  WC         = " ";

localparam DATA_WIDTH_BITS  = $clog2(DATA_WIDTH);
localparam WIDTH_BITS       = $clog2(WIDTH);
localparam LINES_BITS       = $clog2(LINES);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [DATA_WIDTH-1:0] i_data;
input wire i_ready;
input wire i_next;

output reg o_data_ready;

// We can add signals for other things as well
// - Integer (0x.., 0b.., 0o..,)
// - OpCode  (+, -, .)
// - Dict    (Dictionary Word / Subroutine?)
output reg o_eol;
output reg o_wc;
output reg [DATA_WIDTH-1:0] o_data;

reg [DATA_WIDTH - 1:0]       lines [LINES - 1:0][WIDTH - 1:0];

reg [WIDTH_BITS - 1:0]       width_index;
reg [LINES_BITS - 1:0]       line_index;

reg [WIDTH_BITS - 1:0]       sent_width_index;
reg [LINES_BITS - 1:0]       sent_line_index;

reg prev_i_ready;
reg prev_i_next;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        width_index = 0;
        line_index = 0;

        sent_width_index = 0;
        sent_line_index = 0;

        o_data_ready = 0;
        o_wc = 0;
        o_eol = 0;

        prev_i_ready = 0;
        prev_i_next = 0;
    end
    else if (1 == i_en)
    begin
        // Save Data
        if (1 == i_ready && 0 == prev_i_ready)
        begin
            lines[line_index][width_index] = i_data;
            if (i_data == EOL)
            begin
                line_index = line_index + 1;
                width_index = 0;
            end
            else width_index <= width_index + 1;
        end

        // Send portion
        if (1 == i_next && prev_i_next == 0)
        begin
            if (line_index != sent_line_index) // Only send finished lines
            begin
                o_data = lines[sent_line_index][sent_width_index];
                o_data_ready = 1;

                case (o_data)
                    EOL: 
                    begin
                        o_eol = 1;
                        o_wc = 0;
                        sent_width_index = 0;
                        sent_line_index = sent_line_index + 1;
                    end

                    WC: 
                    begin
                        o_eol = 0;
                        o_wc = 1;
                        sent_width_index = sent_width_index + 1;
                    end

                    default:
                    begin
                        o_eol = 0;
                        o_wc = 0;
                        sent_width_index = sent_width_index + 1;
                    end
                endcase

                o_data_ready = 1;
            end
        end

        prev_i_ready = i_ready;
        prev_i_next = i_next;
    end
end

endmodule

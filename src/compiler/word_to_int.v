
// Convert the provided word into an integer

module word_to_int(i_clk, i_en, i_word, i_len, o_data, o_err);

localparam DATA_WIDTH = 8; // Character width (also UART width)
parameter  WIDTH      = 32; // Maximum word width
parameter  DATA       = 32; // Data width

localparam WIDTH_BITS = $clog2(WIDTH) + 1;
localparam DATA_BITS = $clog2(DATA) + 1;

input wire i_clk;
input wire i_en;

input wire [DATA_WIDTH-1:0] i_word [WIDTH-1:0];
input wire [WIDTH_BITS - 1:0] i_len;

output reg [DATA-1:0] o_data; 
output reg o_err;

reg [DATA-1:0] temp;

always @(posedge i_clk)
begin
    if (1 == i_en)
    begin
        if (i_len == 0) o_err <= 1;
        else
        begin
            temp = 0;
            for (int i = 0; i < i_len; i++)
            begin
                o_err = 0;
                case (i_word[i])
                    "0": temp = (temp * 10);
                    "1": temp = (temp * 10) + 1;
                    "2": temp = (temp * 10) + 2;
                    "3": temp = (temp * 10) + 3;
                    "4": temp = (temp * 10) + 4;
                    "5": temp = (temp * 10) + 5;
                    "6": temp = (temp * 10) + 6;
                    "7": temp = (temp * 10) + 7;
                    "8": temp = (temp * 10) + 8;
                    "9": temp = (temp * 10) + 9;
                    default: o_err = 1;
                endcase
            end
            o_data <= temp;
        end
    end
end

endmodule


// Convert the provided word into an integer (HEX)

module word_to_hex(i_clk, i_en, i_word, i_len, o_data, o_err);

localparam DATA_WIDTH = 8; // Character width (also UART width)
parameter  WIDTH      = 32; // Maximum word width (WAY Longer than the data can be)
parameter  DATA       = 32; // Data width

localparam PREFIX0 = "0";
localparam PREFIX1 = "x";
localparam WIDTH_BITS = $clog2(WIDTH);

input wire i_clk;
input wire i_en;

input wire [DATA_WIDTH-1:0] i_word [WIDTH-1:0];
input wire [WIDTH_BITS-1:0] i_len;

output reg [DATA-1:0] o_data; 
output reg o_err;

reg [DATA-1:0] temp;

always @(posedge i_clk)
begin
    if (1 == i_en)
    begin
        if (i_word[0] == PREFIX0 && i_word[1] == PREFIX1)
        begin
            temp = 0;
            for (int i = 2; i < i_len; i++)
            begin
                o_err = 0;
                case (i_word[i])
                    "0": temp = (temp << 4);
                    "1": temp = (temp << 4) + 1;
                    "2": temp = (temp << 4) + 2;
                    "3": temp = (temp << 4) + 3;
                    "4": temp = (temp << 4) + 4;
                    "5": temp = (temp << 4) + 5;
                    "6": temp = (temp << 4) + 6;
                    "7": temp = (temp << 4) + 7;
                    "8": temp = (temp << 4) + 8;
                    "9": temp = (temp << 4) + 9;
                    // Upper Case
                    "A": temp = (temp << 4) + 10;
                    "B": temp = (temp << 4) + 11;
                    "C": temp = (temp << 4) + 12;
                    "D": temp = (temp << 4) + 13;
                    "E": temp = (temp << 4) + 14;
                    "F": temp = (temp << 4) + 15;
                    // Lower Case
                    "a": temp = (temp << 4) + 10;
                    "b": temp = (temp << 4) + 11;
                    "c": temp = (temp << 4) + 12;
                    "d": temp = (temp << 4) + 13;
                    "e": temp = (temp << 4) + 14;
                    "f": temp = (temp << 4) + 15;
                    default: o_err = 1;
                endcase
            end
            o_data <= temp;
        end
        else
        begin
            temp <= 0;
            o_data <= 0;
            o_err <= 1;
        end
    end
end

endmodule

module find_index(i_update, i_key, i_keys, o_index, d_found);

parameter ENTRIES = 2;
parameter KEY_WIDTH = 8;
parameter KEY_LENGTH = 1;

localparam INDEX_BITS = $clog2(ENTRIES) + 1;

input wire i_update;
input wire [KEY_WIDTH-1:0] i_key [KEY_LENGTH-1:0];
input wire [KEY_WIDTH-1:0] i_keys [ENTRIES-1:0][KEY_LENGTH-1:0];

output reg d_found;
initial d_found = 0;

output reg [INDEX_BITS-1:0] o_index;
initial o_index = 0;

always @(posedge i_update)
begin
    d_found = 0;
    o_index = 0;

    for(int i = 0; i < ENTRIES; i++) 
    begin
        if ((0 == d_found) && (i_key == i_keys[i]))
        begin
            /* verilator lint_off WIDTHTRUNC */
            o_index = i;
            /* verilator lint_on WIDTHTRUNC */
            d_found = 1;
        end
    end
end

endmodule

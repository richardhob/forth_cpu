module find_index(i_update, i_key, i_keys, o_index, o_found);

parameter ENTRIES = 2;
parameter KEY_WIDTH = 8;
parameter KEY_LENGTH = 1;

localparam INDEX_BITS = $clog2(ENTRIES);

input wire i_update;
input wire [KEY_WIDTH-1:0] i_key [KEY_LENGTH-1:0];
input wire [KEY_WIDTH-1:0] i_keys [ENTRIES-1:0][KEY_LENGTH-1:0];

output reg o_found;
initial o_found = 0;

output reg [INDEX_BITS-1:0] o_index;
initial o_index = 0;

always @(posedge i_update)
begin
    o_found = 0;
    o_index = 0;

    for(int i = 0; i < ENTRIES; i++) 
    begin
        if ((0 == o_found) && (i_key == i_keys[i]))
        begin
            /* verilator lint_off WIDTHTRUNC */
            o_index = i;
            /* verilator lint_on WIDTHTRUNC */

            o_found = 1;
        end
    end
end

endmodule

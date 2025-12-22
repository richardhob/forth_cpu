
// String to Integer Dictionary
//
// -Store values
// -Look up values
//
// SET and GET take two clocks
//
// SET_FAST and GET_FAST take one, but you HAVE to know the index already of the
// KEY... which isn't a problem necessarily 
//
// There is also "ENCODE" which retreives the index

module dict(i_clk, i_rst, i_en, i_ready, i_op, i_key, i_index, i_value, o_value, o_index, o_done, d_state);

parameter ENTRIES = 10;

parameter KEY_WIDTH = 8;
parameter KEY_LENGTH = 8;

parameter VALUE_WIDTH = 32;
parameter VALUE_LENGTH = 1;

localparam ENTRIES_BITS = $clog2(ENTRIES);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire i_ready;
input wire [2:0] i_op; // set, get, set_fast, get_fast, encode, delete, quick_delete

input wire [KEY_WIDTH-1:0] i_key [KEY_LENGTH-1:0];
input wire [ENTRIES_BITS - 1:0] i_index;
input wire [VALUE_WIDTH-1:0] i_value [VALUE_LENGTH-1:0];

output reg o_done;
output reg [VALUE_WIDTH-1:0] o_value [VALUE_LENGTH-1:0];
output reg [ENTRIES_BITS-1:0] o_index;

output reg [2:0] d_state;
initial d_state = STATE_IDLE;

reg [KEY_WIDTH-1:0] keys [ENTRIES-1:0][KEY_LENGTH-1:0];
reg [VALUE_WIDTH-1:0] values [ENTRIES-1:0][VALUE_LENGTH-1:0];

wire [ENTRIES_BITS-1:0] index;
reg update_index;
initial update_index = 0;

find_index #(
    .ENTRIES(ENTRIES),
    .KEY_WIDTH(KEY_WIDTH),
    .KEY_LENGTH(KEY_LENGTH)
) find_key (
    .i_update(update_index),
    .i_key(i_key),
    .i_keys(keys),
    .o_index(index),
    .d_found()
);

localparam OP_SET = 0;
localparam OP_GET = 1;
localparam OP_ENCODE = 3;
localparam OP_SET_FAST = 3;
localparam OP_GET_FAST = 4;
localparam OP_DELETE = 5;
localparam OP_DELETE_FAST = 6;

localparam STATE_IDLE = 0;
localparam STATE_SET = 1;
localparam STATE_GET = 2;
localparam STATE_ENCODE = 3;
localparam STATE_DELETE = 4;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        update_index <= 0;

        o_done <= 0;
        o_index <= 0;

        d_state <= STATE_IDLE;

        // Delete KEYS and VALUES on RESET
        for (int i = 0; i < ENTRIES; i++)
        begin
            for (int j = 0; j < KEY_LENGTH; j++)
            begin
                keys[i][j] <= 0;
            end

            for (int k = 0; k < VALUE_LENGTH; k++)
            begin
                values[i][k] <= 0;
            end
        end
        for (int l = 0; l < VALUE_LENGTH; l++)
        begin
            o_value[l] <= 0;
        end
    end
    else if (1 == i_en)
    begin
        if (1 == i_ready)
        begin
            if (STATE_IDLE == d_state)
            begin
                case (i_op)
                    OP_SET:
                    begin
                        update_index <= 1;
                        d_state <= STATE_SET;
                    end

                    OP_GET:
                    begin
                        update_index <= 1;
                        d_state <= STATE_GET;
                    end

                    OP_DELETE:
                    begin
                        update_index <= 1;
                        d_state <= STATE_DELETE;
                    end

                    OP_ENCODE:
                    begin
                        update_index <= 1;
                        d_state <= STATE_ENCODE;
                    end

                    OP_SET_FAST:
                    begin
                        values[i_index] <= i_value;
                        o_value <= 0;
                        o_index <= i_index;
                        o_done <= 1;
                    end

                    OP_GET_FAST:
                    begin
                        o_value <= values[i_index];
                        o_index <= i_index;
                        o_done <= 1;
                    end

                    OP_DELETE_FAST:
                    begin
                        o_value <= values[i_index];
                        o_index <= i_index;

                        for (int i = 0; i < KEY_LENGTH; i++) keys[i_index] <= 0;
                        for (int i = 0; i < VALUE_LENGTH; i++) values[i_index] <= 0;

                        o_done <= 1;
                    end

                    default:
                    begin
                        update_index <= 1;
                        d_state <= STATE_IDLE;
                    end
                endcase
            end
        end
        if(STATE_SET == d_state)
        begin
            keys[index] <= i_key;
            values[index] <= i_value;
            o_value <= 0;
            o_index <= index;
            o_done <= 1;
            d_state <= STATE_IDLE;
        end
        else if(STATE_GET == d_state)
        begin
            o_value <= values[index];
            o_index <= index;
            o_done <= 1;
            d_state <= STATE_IDLE;
        end
        else if (STATE_ENCODE == d_state)
        begin
            o_index <= index;
            o_value <= 0;
            o_done <= 1;
            d_state <= STATE_IDLE;
        end
        else if (STATE_DELETE == d_state)
        begin
            o_index <= index;
            o_value <= values[index];

            for (int i = 0; i < KEY_LENGTH; i++) keys[index] <= 0;
            for (int i = 0; i < VALUE_LENGTH; i++) values[index] <= 0;

            o_done <= 1;
        end
        else // STATE_IDLE == d_state
            o_done <= 0;
    end
end

endmodule

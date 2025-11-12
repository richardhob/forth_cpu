// Refactor: Split i_cmd into i_save and i_get lines?
//           Split o_done into o_save_done and o_get_done

module fifo(i_clk, i_rst, i_en, 
    i_set, i_get, i_data,
    o_data, o_set, o_get);

parameter WIDTH = 8; // Bits wide
parameter DEPTH = 256; // Number of Cells

input wire i_clk;
input wire i_rst;
input wire i_en;
input wire i_set;
input wire i_get;
input wire [WIDTH-1:0] i_data;

output reg o_set;
initial o_set = 0;

output reg o_get;
initial o_get = 0;

output reg [WIDTH-1:0] o_data;
initial o_data = 0;

reg [WIDTH-1:0] _data [DEPTH-1:0];

reg [$clog2(DEPTH)-1:0] _i_pointer;
initial _i_pointer = 0;

reg [$clog2(DEPTH)-1:0] _o_pointer;
initial _o_pointer = 0;

localparam CMD_NONE = 0;
localparam CMD_SET  = 1;
localparam CMD_GET  = 2;

always @(posedge i_clk or posedge i_rst)
begin
    if (i_rst)
    begin
        _i_pointer <= 0;
        _o_pointer <= 0;
        o_data <= 0;
        o_set <= 0;
        o_get <= 0;
    end
    else if (i_en)
    begin
        if (i_cmd & CMD_SET)
        begin
            _data[_i_pointer] <= i_data;
            _i_pointer <= _i_pointer + 1;
            o_set <= 1;
        end
        else o_set <= 0;

        if (i_cmd & CMD_GET)
        begin
            o_data <= _data[_o_pointer];
            _o_pointer <= _o_pointer + 1;
            o_get <= 1;
        end
        else o_get <= 0;
    end
end

`ifdef FORMAL
reg f_past_valid;
initial f_past_valid = 1'b0;

reg f_past_reset;
initial f_past_reset = 1'b0;

always @(posedge i_clk)
begin
    f_past_valid <= 1'b1;
end

always @(posedge i_rst)
begin
    f_past_reset <= 1'b1;
end

always @(posedge i_clk)
    if (f_past_valid
        && i_en
        && ($past(i_cmd) & CMD_SET)
        && (i_cmd & CMD_SET))
    begin
        assert($stable(_o_pointer));
    end

always @(posedge i_clk)
    if (f_past_valid
        && i_en
        && ($past(i_cmd) & CMD_GET)
        && (i_cmd & CMD_GET))
    begin
        assert($stable(_i_pointer));
    end

`endif

endmodule

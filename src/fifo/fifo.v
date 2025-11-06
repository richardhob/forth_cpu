
module fifo(i_clk, i_reset, i_enable, i_cmd, i_data, o_data, o_done);

parameter WIDTH = 8; // Bits wide
parameter DEPTH = 256; // Number of Cells

input wire i_clk;
input wire i_reset;
input wire i_enable;
input wire [1:0] i_cmd;
input wire [WIDTH-1:0] i_data;

output reg [WIDTH-1:0] o_data;

output reg o_done;
initial o_done = 1;

reg [WIDTH-1:0] _data [DEPTH-1:0];

reg [$clog2(DEPTH)-1:0] _i_pointer;
initial _i_pointer = 0;

reg [$clog2(DEPTH)-1:0] _o_pointer;
initial _o_pointer = 0;

localparam CMD_NONE = 0;
localparam CMD_PUSH = 1;
localparam CMD_POP  = 2;

always @(posedge i_clk or posedge i_reset)
begin
    if (i_reset == 1'b1) o_done <= 1'b0;
    else if (i_enable == 1'b1)
    begin
        case (i_cmd)
            CMD_NONE: o_done <= 1'b0;
            CMD_PUSH: o_done <= 1'b1;
            CMD_POP:  o_done <= 1'b1;
            default:  o_done <= 1'b0;
        endcase
    end
end

always @(posedge i_clk or posedge i_reset)
begin
    if (i_reset == 1'b1)
    begin
        _i_pointer <= 0;
        _o_pointer <= 0;
        o_data <= 0;
    end
    else if (i_enable == 1'b1)
    begin
        if (i_cmd == CMD_PUSH)
        begin
            _data[_i_pointer] <= i_data;
            _i_pointer <= _i_pointer + 1;
        end
        else if (i_cmd == CMD_POP)
        begin
            o_data <= _data[_o_pointer];
            _o_pointer <= _o_pointer + 1;
        end
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

always @(posedge i_reset)
begin
    f_past_reset <= 1'b1;
end

always @(posedge i_clk)
    if (f_past_valid
        && i_enable
        && !i_reset
        && !f_past_reset
        && $past(i_cmd) == CMD_NONE)
    begin
        assert($stable(_o_pointer));
        assert($stable(_i_pointer));
        assert(o_data == 0);
    end

always @(posedge i_clk)
    if (f_past_valid
        && !i_reset
        && !f_past_reset
        && i_enable
        && $past(i_cmd) == CMD_PUSH)
    begin
        assert($stable(_o_pointer));
        assert($changed(_i_pointer));
        assert(_data[$past(_i_pointer)] == $past(i_data));
        assert(o_data == 1);
    end

always @(posedge i_clk)
    if (f_past_valid
        && !i_reset
        && !f_past_reset
        && i_enable
        && $past(i_cmd) == CMD_POP)
    begin
        assert($stable(_i_pointer));
        assert($changed(_o_pointer));
        assert(_data[$past(_o_pointer)] == $past(o_data));
        assert(o_data == 1);
    end

`endif

endmodule

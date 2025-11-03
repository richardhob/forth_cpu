
module timer(
    input enable,
    input reset,
    input clock,
    output line);

parameter BITS = 32;
parameter PERIOD = 256;

wire enable;
wire reset;
wire clock;

reg line;

reg [BITS-1:0] counter;

always @(posedge clock or reset)
begin
    if (reset == 0) 
    begin 
        counter = 0;
        line = 0;
    end
    else 
        if (enable == 1)
        begin
            if (counter < PERIOD) counter = counter + 1;
            else 
            begin 
                counter = 0;
                line = 1;
            end
        end
    end
end

endmodule

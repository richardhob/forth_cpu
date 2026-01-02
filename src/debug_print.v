
module debug_print(i_clk, i_rst, i_en,
    // Parser Inputs

    // CPU Inputs
    i_opcode, i_data, i_ready, 

    // CPU Outputs as inputs
    i_cpu_data, i_cpu_err,

    // UART TX Outputs
    o_tx);

reg [OPCODE-1:0] opcodes [256];
reg [DATA-1:0] data [256];
reg [DATA-1:0] cpu_data [256];
reg [ERROR-1:0] cpu_err [256];
reg index;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        index = 0;
    end
    if (1 == i_en)
    begin
        // Save CPU Inputs
        if (1 == i_ready)
        begin
            opcodes[index] = i_opcode;
            data[index] = i_data;
            cpu_data[index] = i_cpu_data;
            cpu_err[index] = i_cpu_err;
            index = index + 1;
        end

        if (1 == i_wc)
        begin
        end

        // if we're in an EOL, check for errors
        if (1 == i_eol)
        begin
            d_state <= STATE_CHECK_CPU_ERR;
        end
    end
end

endmodule


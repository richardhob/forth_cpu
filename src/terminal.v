
module terminal(i_clk, i_rst, i_en,
    // UART RX
    i_rx_data, 
    i_rx_ready, 

    // UART TX
    i_tx_next, 
    o_tx_data, 
    o_tx_ready,

    // Tokenizer
    o_line,
    o_len,
    o_ready,

    // Debug
    d_line,
    d_index,
    d_state,
    d_next_state
);

localparam WIDTH = 8; // Character Width (8 bits, locked to what UART uses)
parameter LENGTH = 16; // Line Length (16 Characters)
parameter LINES = 2; // Number of lines (more than 1 recommended)

localparam LINE_BITS = LINES > 1 ? $clog2(LINES) : 1;
localparam LENGTH_BITS = $clog2(LENGTH);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [WIDTH-1:0] i_rx_data;
input wire i_rx_ready;

input wire i_tx_next;
output reg [WIDTH-1:0] o_tx_data;
output reg o_tx_ready;

output reg [WIDTH-1:0] o_line [LENGTH-1:0];
output reg [LENGTH_BITS-1:0] o_len;
output reg o_ready;

output reg [LINE_BITS - 1:0] d_line;
output reg [LENGTH_BITS-1:0] d_index;
output reg [2:0] d_state;
output reg [2:0] d_next_state;

reg [WIDTH-1:0] lines [LINES-1:0][LENGTH-1:0];
reg [7:0] blank_character;
reg tx_sent;

localparam STATE_IDLE = 0;
localparam STATE_SEND_LAST = 1;
localparam STATE_SEND_BACKSPACE_SPACE_BACKSPACE = 2;
localparam STATE_SEND_SPACE_BACKSPACE = 3;
localparam STATE_SEND_BACKSPACE = 4;
localparam STATE_SEND_NEWLINE = 5;
localparam STATE_WAIT_FOR_TX = 6;

always @(posedge i_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        o_tx_data <= 0;
        o_tx_ready <= 0;

        o_len <= 0;
        o_ready <= 0;

        d_line <= 0;
        d_index <= 0;
        d_state <= STATE_IDLE;
        d_next_state <= STATE_IDLE;

        blank_character <= 8'h0;
        tx_sent <= 0;

        for (int i = 0; i < LINES; i++)
        begin
            o_line[i] <= blank_character;
            for (int j = 0; j < LENGTH; j++)
            begin
                lines[i][j] <= blank_character;
            end
        end
    end
    else if (1 == i_en)
    begin
        if (STATE_IDLE == d_state)
        begin
            if (i_rx_ready == 1)
            begin
                if (i_rx_data == 8'h0A // NEWLINE
                    || i_rx_data == 8'h0C) // FORMFEED
                begin
                    // Send New Line
                    // TODO: Wait for CPU Error Code or Data
                    lines[d_line][d_index] = i_rx_data;
                    o_line = lines[d_line];
                    o_len = d_index + 1;

                    d_line = d_line + 1;
                    d_index = 0;

                    d_state = STATE_SEND_NEWLINE;
                end
                else if (i_rx_data == 8'h7F || i_rx_data == 8'h08) // BACKSPACE
                begin
                    // Send: Backspace / Space / Backspace
                    if (d_index > 0) 
                    begin
                        d_index = d_index - 1;
                        d_state = STATE_SEND_BACKSPACE_SPACE_BACKSPACE;
                    end
                end
                else // Save data to line buffer
                begin
                    lines[d_line][d_index] = i_rx_data;
                    d_index = d_index + 1;
                    d_state = STATE_SEND_LAST;
                end
            end
        end

        if (STATE_SEND_NEWLINE == d_state)
        begin
            o_tx_data = 8'h0A; // New Line
            d_state = STATE_WAIT_FOR_TX;
            d_next_state = STATE_IDLE;
        end

        if (STATE_SEND_LAST == d_state)
        begin
            o_tx_data = lines[d_line][d_index - 1];
            d_state = STATE_WAIT_FOR_TX;
            d_next_state = STATE_IDLE;
        end

        if (STATE_SEND_BACKSPACE_SPACE_BACKSPACE == d_state)
        begin
            o_tx_data = 8'h08; // Backspace
            d_state = STATE_WAIT_FOR_TX;
            d_next_state = STATE_SEND_SPACE_BACKSPACE;
        end

        if (STATE_SEND_SPACE_BACKSPACE == d_state)
        begin
            o_tx_data = " "; // Space
            d_state = STATE_WAIT_FOR_TX;
            d_next_state = STATE_SEND_BACKSPACE;
        end

        if (STATE_SEND_BACKSPACE == d_state)
        begin
            o_tx_data = 8'h08; // Backspace
            d_state = STATE_WAIT_FOR_TX;
            d_next_state = STATE_IDLE;
        end

        if (STATE_WAIT_FOR_TX == d_state)
        begin
            o_tx_ready = 1;

            // TX Block is ready
            if (1 == i_tx_next && 0 == tx_sent) tx_sent = 1;

            // Send in process, Continue with state machine
            if (0 == i_tx_next && 1 == tx_sent) 
            begin 
                o_tx_ready = 0;
                tx_sent = 0;
                d_state = d_next_state;
                d_next_state = STATE_IDLE;
            end
        end
    end
end

endmodule

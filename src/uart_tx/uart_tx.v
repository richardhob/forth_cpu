
/* UART TX Transmitter
 *
 * - CLK -> Clock Divider -> Divided Clock
 * - Data 
 * - New Data line (Ready for next data)
 * - Data Ready line
 * - UART TX Out
 *
 *
 * When DATA Ready is high, then:
 *
 * 1. get data
 * 2. Write bits
 * 3. 
 */
module uart_tx(i_clk, i_rst, i_en, i_data, i_ready, o_next, o_tx)

parameter START_BITS = 1;
parameter DATA_WIDTH = 8;
parameter STOP_BITS = 2;
parameter COOLDOWN_BITS = 3;

parameter CLOCK_RATE = 120000000; // Hz
parameter BAUDRATE   = 115200;    // Baud rate (bits / second)
parameter OSR        = 16;        // Over Sample Ratio

localparam TOTAL_BITS = DATA_BITS + STOP_BITS + START_BITS + PARITY_BITS;
localparam DIVIDER_RATIO = CLOCK_RATE / (BAUDRATE * TOTAL_BITS);

input wire i_clk;
input wire i_rst;
input wire i_en;

input wire [DATA_BITS-1:0] i_data;
input wire i_ready;

output reg o_next;
initial o_next = 0;

output reg o_tx;
initial o_tx = 0;

wire divided_clk;

clock_divider #(
    .DIVIDER=DIVIDER_RATIO
) uart_clk (
    .i_rst(i_rst),
    .i_clk(i_clk),
    .o_clk(divided_clk)
);

localparam STATE_RESET = 0;
localparam STATE_IDLE = 1;
localparam STATE_START = 2;
localparam STATE_DATA = 3;
localparam STATE_STOP = 4;
localparam STATE_COOLDOWN = 5;

reg state;
initial state = STATE_RESET;

reg [DATA_BITS-1:0] data;
initial data = 0;

reg [START_BITS-1:0] start_bits;
initial start_bits = 0;

reg [DATA_BITS-1:0] data_bits;
initial data_bits = 0;

reg [STOP_BITS-1:0] stop_bits;
initial stop_bits = 0;

reg [COOLDOWN_BITS-1:0] cooldown_bits;
initial cooldown_bits = 0;

always @(posedge divided_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        data <= 0;
        state <= STATE_IDLE;

        start_bits <= 0;
        data_bits <= 0;
        stop_bits <= 0;
        cooldown_bits <= 0;

        o_tx <= 0;
    end
    else if (1 == i_en)
    begin
        case (state)
            STATE_IDLE:
            if (i_ready == 1)
            begin
                data <= i_data;
                state <= STATE_START;
                o_next <= 1;
            end

            STATE_START:
            begin
                if (start_bits < START_BITS)
                begin
                    start_bits <= start_bits + 1;
                    o_tx <= 1;
                    o_next <= 0;
                end
                else
                begin
                    state <= STATE_DATA;
                end
            end

            STATE_DATA:
            begin
                if (data_bits < DATA_BITS)
                begin
                    o_tx <= data & 1;
                    data <= data >> 1;
                    data_bits <= data_bits + 1;
                end
                else
                begin
                end
            end

            STATE_STOP:
            begin
            end

            STATE_COOLDOWN:
            begin
            end

        endcase
    end
end

// --- Is a transmission happening?

// Parts of the transmission:
//
// 1. Start Bits
// 2. Data Bits
// 3. Stop Bits
// 4. Cooldown (IE wait for an amount of time between packets)
//
// When ready:
//
// 1. Sample input bits (cache)
// 2. Signal that we're ready for new data (o_next)
// 3. Form / Transmit packet 
// 4. COOLDOWN
//
// 5. Next packet
// 



endmodule


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
module uart_tx(i_divided_clk, i_rst, i_en, i_data, i_ready, o_next, o_tx, d_state, d_data);

parameter START      = 1;
parameter DATA       = 8;
parameter STOP       = 2;
parameter COOLDOWN   = 1;

parameter CLOCK_RATE = 120000000; // Hz
parameter BAUDRATE   = 115200;    // Baud rate (bits / second)
parameter OSR        = 16;        // Over Sample Ratio

localparam START_THRESHOLD    = START * OSR;
localparam START_BITS         = $clog2(START_THRESHOLD) + 1;

localparam DATA_THRESHOLD     = DATA * OSR;
localparam DATA_BITS          = $clog2(DATA_THRESHOLD) + 1;

localparam STOP_THRESHOLD     = STOP * OSR;
localparam STOP_BITS          = $clog2(STOP_THRESHOLD) + 1;

localparam COOLDOWN_THRESHOLD = COOLDOWN * OSR;
localparam COOLDOWN_BITS      = $clog2(COOLDOWN_THRESHOLD) + 1;

localparam OSR_BITS = $clog2(OSR);
localparam TOTAL_BITS = DATA + STOP + START;
localparam DIVIDER_RATIO = CLOCK_RATE / (BAUDRATE * TOTAL_BITS);

input wire i_divided_clk;
input wire i_rst;
input wire i_en;

input wire [DATA_BITS-1:0] i_data;
input wire i_ready;

output reg o_next;
initial o_next = 0;

output reg o_tx;
initial o_tx = 0;

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

localparam STATE_RESET    = 0;
localparam STATE_IDLE     = 1;
localparam STATE_START    = 2;
localparam STATE_DATA     = 3;
localparam STATE_STOP     = 4;
localparam STATE_COOLDOWN = 5;

output reg [31:0] d_state;
initial d_state = STATE_RESET;

output reg [DATA_BITS-1:0] d_data;
initial d_data = 0;

reg [START_BITS-1:0] start_bits;
initial start_bits = 0;

reg [DATA_BITS-1:0] data_bits;
initial data_bits = 0;

reg [STOP_BITS-1:0] stop_bits;
initial stop_bits = 0;

reg [COOLDOWN_BITS-1:0] cooldown_bits;
initial cooldown_bits = 0;

always @(posedge i_divided_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        d_data <= 0;
        d_state <= STATE_IDLE;

        stop_bits <= 0;
        cooldown_bits <= 0;

        o_tx <= 0;
        o_next <= 0;
    end
    else if (1 == i_en)
    begin
        case (d_state)
            STATE_IDLE:
            begin
                if (i_ready == 0) // NOT READY
                begin
                    o_next <= 1;
                end
                else // READY
                begin
                    d_data <= i_data;
                    d_state <= STATE_START;
                    o_next <= 0;
                    start_bits <= 0;
                end
            end

            STATE_START:
            begin
                if (start_bits < (START_THRESHOLD - 1))
                begin
                    start_bits <= start_bits + 1;
                    o_tx <= 1;
                end
                else
                begin
                    d_state <= STATE_DATA;
                    data_bits <= 0;
                    o_tx <= d_data[0];
                end
            end

            STATE_DATA:
            begin
                if (data_bits < (DATA_THRESHOLD - 1))
                begin
                    o_tx <= d_data[data_bits + 1 >> 4];
                    data_bits <= data_bits + 1;
                end
                else
                begin
                    d_state <= STATE_STOP;
                    stop_bits <= 0;
                    o_tx <= 1;
                end
            end

            STATE_STOP:
            begin
                if (stop_bits < (STOP_THRESHOLD - 1))
                begin
                    stop_bits <= stop_bits + 1;
                end
                else
                begin
                    o_tx <= 0;
                    d_state <= STATE_COOLDOWN;
                    cooldown_bits <= 0;
                end
            end

            STATE_COOLDOWN:
            begin
                if (cooldown_bits < (COOLDOWN_THRESHOLD - 1))
                begin
                    cooldown_bits <= cooldown_bits + 1;
                end
                else 
                begin
                    d_state <= STATE_IDLE;
                end
            end

        endcase
    end
end

endmodule


// UART RX With State Machine
//
// Might (should) be a bit easier to understand / develop
//
module uart_rx(i_divided_clk, i_rst, i_en, i_rx, o_data, o_ready, d_state, d_data);

parameter START  =  1; // UART Start Bits
parameter DATA   =  8; // UART Data Bits
parameter STOP   =  2; // UART Stop Bits
parameter OSR    = 16; // Over Sample Ratio

input wire i_divided_clk;
input wire i_rst;
input wire i_en;
input wire i_rx;

output reg [DATA-1:0] o_data;
initial o_data = 0;

output reg o_ready;
initial o_ready = 0;

localparam START_THRESHOLD          = START * OSR;
localparam START_THRESHOLD_DEBOUNCE = START_THRESHOLD / 4;
localparam START_THRESHOLD_OK       = START_THRESHOLD / 2;
localparam START_BITS               = $clog2(START_THRESHOLD) + 1;

localparam DATA_THRESHOLD           = DATA * OSR;
localparam DATA_BITS                = $clog2(DATA_THRESHOLD) + 1;
localparam D0_THRESHOLD             = (OSR / 2) - 1;
localparam D1_THRESHOLD             = 1*OSR + (OSR / 2) - 1;
localparam D2_THRESHOLD             = 2*OSR + (OSR / 2) - 1;
localparam D3_THRESHOLD             = 3*OSR + (OSR / 2) - 1;
localparam D4_THRESHOLD             = 4*OSR + (OSR / 2) - 1;
localparam D5_THRESHOLD             = 5*OSR + (OSR / 2) - 1;
localparam D6_THRESHOLD             = 6*OSR + (OSR / 2) - 1;
localparam D7_THRESHOLD             = 7*OSR + (OSR / 2) - 1;

localparam STOP_THRESHOLD           = STOP * OSR;
localparam STOP_BITS                = $clog2(STOP_THRESHOLD) + 1;

localparam STATE_RESET              =  0;
localparam STATE_IDLE               =  1;
localparam STATE_START_DEBOUNCE     =  2;
localparam STATE_START_VALID        =  3;
localparam STATE_START              =  4;
localparam STATE_DATA_D0            =  5;
localparam STATE_DATA_D1            =  6;
localparam STATE_DATA_D2            =  7;
localparam STATE_DATA_D3            =  8;
localparam STATE_DATA_D4            =  9;
localparam STATE_DATA_D5            = 10;
localparam STATE_DATA_D6            = 11;
localparam STATE_DATA_D7            = 12;
localparam STATE_DATA_END           = 13;
localparam STATE_STOP               = 14;

output reg [31:0] d_state;
initial d_state = STATE_RESET;

output reg [DATA_BITS-1:0] d_data;
initial d_data = 0;

reg [START_BITS-1:0] start_counter;
initial start_counter = 0;

reg [DATA_BITS-1:0]  data_counter;
initial data_counter = 0;

reg [STOP_BITS-1:0]  stop_counter;
initial stop_counter = 0;

always @(posedge i_divided_clk or posedge i_rst)
begin
    if (1 == i_rst)
    begin
        d_state <= STATE_IDLE;
        start_counter <= 0;
        data_counter <= 0;
        stop_counter <= 0;
        d_data <= 0;
    end
    else if (1 == i_en)
    begin
        case(d_state)
            STATE_IDLE:
            begin
                if (1 == i_rx)
                begin
                    d_state <= STATE_START_DEBOUNCE;
                    start_counter <= 0;
                end
            end

            STATE_START_DEBOUNCE:
            begin
                start_counter <= start_counter + 1;
                if (start_counter >= (START_THRESHOLD_DEBOUNCE - 1)) d_state <= STATE_START_VALID;
            end

            STATE_START_VALID:
            begin
                start_counter <= start_counter + 1;
                if (start_counter < START_THRESHOLD_OK)
                begin
                    if (i_rx == 0) d_state <= STATE_IDLE;
                end
                else d_state <= STATE_START;
            end

            STATE_START:
            begin
                start_counter += 1;
                if (start_counter >= (START_THRESHOLD - 1))
                begin
                    d_state <= STATE_DATA_D0;
                    data_counter <= 0;

                    o_ready <= 0;
                    d_data <= 0;
                end
            end

            STATE_DATA_D0:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D0_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D1;
                    d_data[0] <= i_rx;
                end
            end

            STATE_DATA_D1:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D1_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D2;
                    d_data[1] <= i_rx;
                end
            end

            STATE_DATA_D2:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D2_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D3;
                    d_data[2] <= i_rx;
                end
            end

            STATE_DATA_D3:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D3_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D4;
                    d_data[3] <= i_rx;
                end
            end

            STATE_DATA_D4:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D4_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D5;
                    d_data[4] <= i_rx;
                end
            end

            STATE_DATA_D5:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D5_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D6;
                    d_data[5] <= i_rx;
                end
            end

            STATE_DATA_D6:
            begin
                data_counter <= data_counter + 1;
                if (data_counter >= D6_THRESHOLD)
                begin
                    d_state <= STATE_DATA_D7;
                    d_data[6] <= i_rx;
                end
            end

            STATE_DATA_D7:
            begin
                if (data_counter < D7_THRESHOLD) data_counter <= data_counter + 1;
                else
                begin
                    d_data[7] <= i_rx;
                    d_state <= STATE_DATA_END;
                end
            end

            STATE_DATA_END:
            begin
                if (data_counter < (DATA_THRESHOLD - 1)) data_counter <= data_counter + 1;
                else
                begin
                    stop_counter <= 0;
                    o_data <= d_data;
                    o_ready <= 1;

                    d_state <= STATE_STOP;
                end
            end

            STATE_STOP:
            begin
                if (stop_counter < (STOP_THRESHOLD - 1)) stop_counter <= stop_counter + 1;
                else d_state <= STATE_IDLE;
            end

            default:
            begin
                d_state <= STATE_IDLE;
                start_counter <= 0;
                data_counter <= 0;
                stop_counter <= 0;
                d_data <= 0;
            end
        endcase
    end
end

endmodule

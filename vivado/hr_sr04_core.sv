module hc_sr04_core
   (
    input  logic clk,
    input  logic reset,
    // slot interface
    input  logic cs,
    input  logic read,
    input  logic write,
    input  logic [4:0] addr,
    input  logic [31:0] wr_data,
    output logic [31:0] rd_data,
    // external signals
    output logic trigger,
    input  logic echo
   );

   // idle: Do nothing
   // trig: Send the 10us start pulse
   // wait_echo: Pulse sent so we wiat for the sensor to respond
   // measure: Sensor is high (echoing), we are counting time so then we can do distance
   // calc in the software in the .cpp file
   // done: Measurement finished, updating registers
   
   typedef enum {idle, trig, wait_echo, measure, done} state_type;
   state_type state_reg, state_next;
   logic [31:0] tick_reg, tick_next;
   logic [31:0] dist_reg, dist_next;
   logic ready_reg, ready_next;

   // 1000 ticks at 10 nanoseconds is 10 microseconds which is the required trigger length
   // Need a timeout to prevent hanging if sensor breaks and made that 30 milliseconds
   localparam TRIG_TICKS = 1000;
   localparam MAX_WAIT = 3000000;


   // Flip Flop 
   always_ff @(posedge clk, posedge reset)
      if (reset) begin
         state_reg <= idle;
         tick_reg <= 0;
         dist_reg <= 0;
         ready_reg <= 0;
      end
      else begin
         state_reg <= state_next;
         tick_reg <= tick_next;
         dist_reg <= dist_next;
         ready_reg <= ready_next;
      end

   // FSMD next-state logic
   always_comb begin
      //Default
      state_next = state_reg;
      tick_next = tick_reg;
      dist_next = dist_reg;
      ready_next = ready_reg;
      trigger = 1'b0;

      // Start condition so if the CPU writes to address 0x00 force a trig state
      if (write && cs && (addr[1:0] == 2'b00)) begin
         state_next = trig;
         tick_next = 0;
         ready_next = 0;
      end

      else begin
         case (state_reg)
            idle: begin
               // Do nothing lol
            end

            trig: begin
               trigger = 1'b1;
               // Count up
               if (tick_reg == TRIG_TICKS) begin
                  state_next = wait_echo;
                  tick_next  = 0;
               end
               else
                  tick_next = tick_reg + 1;
            end

            wait_echo: begin
               // Wait for the echo line to go high
               if (echo) begin
                  state_next = measure;
                  tick_next  = 0;
               end
               // If sensor never responds we send the error code
               else if (tick_reg == MAX_WAIT) begin
                  state_next = done;
                  dist_next  = 32'hFFFFFFFF;
               end
               else
                  tick_next = tick_reg + 1;
            end

            measure: begin
               if (~echo) begin
                  state_next = done;
                  dist_next  = tick_reg;
               end
               // If sensor is too far or stuck high give it the error code
               else if (tick_reg == MAX_WAIT) begin 
                  state_next = done;
                  dist_next  = 32'hFFFFFFFF; 
               end
               else
                  tick_next = tick_reg + 1;
            end

            done: begin
               ready_next = 1'b1;
               state_next = idle;
            end
            default: state_next = idle;
         endcase
      end
   end

   // addr[0] = 0 Read Distance
   // addr[1] = 1 Read Status 
   assign rd_data = (addr[0] == 0) ? dist_reg : {31'b0, ready_reg};

endmodule

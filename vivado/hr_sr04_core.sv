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

   typedef enum {idle, trig, wait_echo, measure, done} state_type;
   state_type state_reg, state_next;
   logic [31:0] tick_reg, tick_next;
   logic [31:0] dist_reg, dist_next;
   logic ready_reg, ready_next;

   localparam TRIG_TICKS = 1000;
   localparam MAX_WAIT = 3000000;


   always_ff @(posedge clk, posedge reset)
      if (reset) begin
         state_reg <= idle;
         tick_reg  <= 0;
         dist_reg  <= 0;
         ready_reg <= 0;
      end
      else begin
         state_reg <= state_next;
         tick_reg  <= tick_next;
         dist_reg  <= dist_next;
         ready_reg <= ready_next;
      end

   // FSMD next-state logic
   always_comb begin
      //Default
      state_next = state_reg;
      tick_next  = tick_reg;
      dist_next  = dist_reg;
      ready_next = ready_reg;
      trigger    = 1'b0;


      if (write && cs && (addr[1:0]==2'b00)) begin
         state_next = trig;
         tick_next  = 0;
         ready_next = 0;
      end

      else begin
         case (state_reg)
            idle: begin
            end

            trig: begin
               trigger = 1'b1;
               if (tick_reg == TRIG_TICKS) begin
                  state_next = wait_echo;
                  tick_next  = 0;
               end
               else
                  tick_next = tick_reg + 1;
            end

            wait_echo: begin
               if (echo) begin
                  state_next = measure;
                  tick_next  = 0;
               end
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

   // 0x00: Read Distance (ticks)
   // 0x01: Read Status (Bit 0 = Ready)
   assign rd_data = (addr[0] == 0) ? dist_reg : {31'b0, ready_reg};

endmodule

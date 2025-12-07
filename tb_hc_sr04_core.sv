`timescale 1ns / 1ps

module tb_hc_sr04_core();
    // Signal Declarations
    logic clk, reset;
    logic cs, read, write;
    logic [4:0] addr;
    logic [31:0] wr_data;
    logic [31:0] rd_data;
    logic trigger;
    logic echo;

    // Instantiate the Core
    hc_sr04_core uut (
        .clk(clk),
        .reset(reset),
        .cs(cs),
        .read(read),
        .write(write),
        .addr(addr),
        .wr_data(wr_data),
        .rd_data(rd_data),
        .trigger(trigger),
        .echo(echo)
    );

    // Clock Generator (100 MHz)
    always #5 clk = ~clk;

    // Test Procedure
    initial begin
        // 1. Initialize
        clk = 0; reset = 1;
        cs = 0; write = 0; addr = 0; wr_data = 0;
        echo = 0;
        
        // 2. Reset Pulse
        #100;
        reset = 0;
        #100;

        // 3. Simulate CPU Writing "Start" (Write to Addr 0)
        $display("Starting Measurement...");
        cs = 1; write = 1; addr = 0; wr_data = 0;
        #10; // 1 clock cycle write
        cs = 0; write = 0;

        // 4. Wait for Trigger to go High
        wait(trigger == 1);
        $display("Trigger Detected!");

        // 5. Wait for Trigger to go Low (End of 10us pulse)
        wait(trigger == 0);
        $display("Trigger Pulse Ended.");

        // 6. Simulate "Physics": Sound travels, bounces, comes back
        #5000; // Wait 5 microseconds before echo starts

        // 7. Simulate Echo Pulse (Let's say 200us duration)
        $display("Echo Received...");
        echo = 1;
        #200000; // 200us pulse width
        echo = 0;
        $display("Echo Ended.");

        // 8. Wait for Core to Process
        #1000;

        // 9. Simulate CPU Reading Result (Read Addr 0)
        cs = 1; read = 1; addr = 0;
        #10;
        $display("Read Distance Ticks: %d", rd_data);
        
        // 10. Check "Ready" Bit (Read Addr 1)
        addr = 1;
        #10;
        $display("Status Register (Bit 0 should be 1): %b", rd_data[0]);

        $stop;
    end
endmodule

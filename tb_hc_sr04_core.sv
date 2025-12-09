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

    //Instantiate
    hc_sr04_core dut (
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

    initial begin
        // Initial
        clk = 0; reset = 1;
        cs = 0; write = 0; addr = 0; wr_data = 0;
        echo = 0;
        
        // Reset
        #100;
        reset = 0;
        #100;

        $display("Starting Measurement...");
        cs = 1; write = 1; addr = 0; wr_data = 0;
        #10;
        cs = 0; write = 0;

        // Wait for the trigger to end up going high and then report the detected
        wait(trigger == 1);
        $display("Trigger Detected!");

        // Wait for trigger to go low and that will happen at the end of the pulse
        wait(trigger == 0);
        $display("Trigger Pulse Ended.");

        // Simulate travel
        #5000; // Wait 5 microseconds before echo starts

        // Simulate echo
        $display("Echo Received...");
        echo = 1;
        #200000; // 200us pulse width
        echo = 0;
        $display("Echo Ended.");

        // Wait for core process
        #1000;

        // Result
        cs = 1; read = 1; addr = 0;
        #10;
        $display("Read Distance Ticks: %d", rd_data);
        addr = 1;
        #10;
        $display("Status Register (Bit 0 should be 1): %b", rd_data[0]);

        $stop;
    end
endmodule

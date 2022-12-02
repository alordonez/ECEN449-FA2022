`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/16/2022 10:50:38 AM
// Design Name: 
// Module Name: 4bit_counter
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

// 4-BIT COUNTER
module counter(
    output [3:0] LEDS,
    
    input CLOCK,
    input reset,
    input [1:0] BUTTONS
    );
    
    reg [3:0] count;
    wire div;
    
    clock_divider new_clock(div, CLOCK, reset); // use clock divider below
    
    always@(posedge div) begin
        if (reset == 1'b1)
            begin
                count <= 4'b0;
            end
        else 
            begin
                if (BUTTONS[0]) // button 0 - count up
                    count <= count + 1;
                if (BUTTONS[1]) // button 1 - count down
                    count <= count - 1;
            end
    end
    
    assign LEDS[3:0] = count[3:0];
endmodule

// CLOCK DIVIDER
module clock_divider(
    output reg div,
    
    input CLOCK,
    input reset
    );
    
    parameter n = 31; // accounts for 32 bits
    reg [n:0] counter;
    
    always@(posedge CLOCK) begin
        if (counter == 25000000)
            begin
                div <= 1;
                counter <= 0;
            end
        else // if not at limit, keep clock at 0
            begin
                div <= 0;
                counter <= counter + 1;
            end
    end
endmodule

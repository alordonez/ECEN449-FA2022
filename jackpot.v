`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/16/2022 11:13:43 AM
// Design Name: 
// Module Name: jackpot
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


module jackpot(
    input CLOCK,
    input reset,
    input [3:0] SWITCHES,
    output reg [3:0] LEDS
    );
    
    reg win;
    reg [3:0] prevSwitches;
    
    clock_divider new_clock(div, CLOCK, reset);
    
    always@(posedge div)
       begin
            prevSwitches <= SWITCHES;
            
            if (reset) begin
                win <= 0;
                LEDS <= 1;
            end
            
            // check winning condition
            else if ( win | (SWITCHES == LEDS && SWITCHES != 0 && prevSwitches == 0) ) begin
                win <= 1;
            end
            
            if (win) LEDS <= 15;        
            else if (LEDS == 0) LEDS <= 1;
            
            else LEDS <= LEDS * 2;
       end

endmodule

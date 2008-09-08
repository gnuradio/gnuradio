

module clock_bootstrap_rom(input [15:0] addr, output [47:0] data);

   reg [47:0] rom [0:15];
   
   //initial
   //  $readmemh("bootrom.mem", rom);

   assign     data = rom[addr];

   initial 
     begin
	//  First 16 bits are address, last 32 are data
	//  First 4 bits of address select which slave
	rom[0] = 48'h0000_0C00_0F03;  //  Both clk sel choose ext ref (0), both are enabled (1), turn off SERDES, ADCs, turn on leds
	rom[1] = 48'h1014_0000_0000;  //  SPI: Set Divider to div by 2
	rom[2] = 48'h1018_0000_0001;  //  SPI: Choose AD9510
	rom[3] = 48'h1010_0000_3418;  //  SPI: Auto-slave select, interrupt when done, TX_NEG, 24-bit word
	rom[4] = 48'h1000_0000_0010;  //  SPI: AD9510 A:0 D:10  Set up AD9510 SPI
	rom[5] = 48'h1010_0000_3518;  //  SPI: SEND IT Auto-slave select, interrupt when done, TX_NEG, 24-bit word
	rom[6] = 48'hffff_ffff_ffff;  // terminate
	rom[7] = 48'hffff_ffff_ffff;  // terminate
	rom[8] = 48'hffff_ffff_ffff;  // terminate
	rom[9] = 48'hffff_ffff_ffff;  // terminate
	rom[10] = 48'hffff_ffff_ffff;  // terminate
	rom[11] = 48'hffff_ffff_ffff;  // terminate
	rom[12] = 48'hffff_ffff_ffff;  // terminate
	rom[13] = 48'hffff_ffff_ffff;  // terminate
	rom[14] = 48'hffff_ffff_ffff;  // terminate
	rom[15] = 48'hffff_ffff_ffff;  // terminate
     end // initial begin
   
endmodule // clock_bootstrap_rom

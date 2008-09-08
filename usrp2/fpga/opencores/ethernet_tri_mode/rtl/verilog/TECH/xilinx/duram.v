module duram(
data_a,
data_b,
wren_a,
wren_b,
address_a,
address_b,
clock_a,
clock_b,
q_a,
q_b);

parameter DATA_WIDTH    = 36; 
parameter ADDR_WIDTH    = 9;  
parameter BLK_RAM_TYPE  = "AUTO";
parameter ADDR_DEPTH    = 2**ADDR_WIDTH;



input   [DATA_WIDTH -1:0]   data_a;
input                       wren_a;
input   [ADDR_WIDTH -1:0]   address_a;
input                       clock_a;
output  [DATA_WIDTH -1:0]   q_a;
input   [DATA_WIDTH -1:0]   data_b;
input                       wren_b;
input   [ADDR_WIDTH -1:0]   address_b;
input                       clock_b;
output  [DATA_WIDTH -1:0]   q_b;
 
wire    [35:0]  do_b;
wire    [35:0]  din_a; 

assign  din_a   =data_a;
assign  q_b     =do_b;


RAMB16_S36_S36 U_RAMB16_S36_S36 (
.DOA         (                          ),
.DOB         (do_b[31:0]                ),
.DOPA        (                          ),
.DOPB        (do_b[35:32]               ),
.ADDRA       (address_a                 ),
.ADDRB       (address_b                 ),
.CLKA        (clock_a                   ),
.CLKB        (clock_b                   ),
.DIA         (din_a[31:0]               ),
.DIB         (                          ),
.DIPA        (din_a[35:32]              ),
.DIPB        (                          ),
.ENA         (1'b1                      ),
.ENB         (1'b1                      ),
.SSRA        (1'b0                      ),
.SSRB        (1'b0                      ),
.WEA         (wren_a                    ),
.WEB         (1'b0                      ));
 
endmodule 



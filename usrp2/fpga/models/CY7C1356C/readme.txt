***************************
Cypress Semiconductor
MPD Applications
Verilog  model for NoBL SRAM CY7C1356
Created: August 04, 2004
Rev: 1.0
***************************

This is the verilog model for the CY7C1356 along with the testbench and test vectors.

Contact support@cypress.com if you have any questions.

This directory has 4 files. including this readme.

1)cy7c1356c.v -> Verilog model for CY7C1356c

2)cy1356.inp -> Test Vector File used for testing the verilog model  

3)testbench.v -> Test bench used for testing the verilog model


COMPILING METHOD :
------------------

        	verilog  +define+<speed_bin>	<Main File>	<Test bench File>

	Ex:
		verilog  +define+sb133  	CY7C1356c.v 	testbench.v

VERIFIED WITH:
--------------

VERILOG-XL 2.2
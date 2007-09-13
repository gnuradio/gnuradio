--Copyright (C) 1991-2006 Altera Corporation
--Your use of Altera Corporation's design tools, logic functions 
--and other software and tools, and its AMPP partner logic 
--functions, and any output files any of the foregoing 
--(including device programming or simulation files), and any 
--associated documentation or information are expressly subject 
--to the terms and conditions of the Altera Program License 
--Subscription Agreement, Altera MegaCore Function License 
--Agreement, or other applicable license agreement, including, 
--without limitation, that your use is for the sole purpose of 
--programming logic devices manufactured by Altera and sold by 
--Altera or its authorized distributors.  Please refer to the 
--applicable agreement for further details.


FUNCTION fifo_4kx16_dc 
(
	aclr,
	data[15..0],
	rdclk,
	rdreq,
	wrclk,
	wrreq
)

RETURNS (
	q[15..0],
	rdempty,
	rdusedw[11..0],
	wrfull,
	wrusedw[11..0]
);

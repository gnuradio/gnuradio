all: single dual

single:	
	iverilog -Wimplicit -Wportbind -c cmdfile ../top/single_u2_sim/single_u2_sim.v -o single_u2_sim

dual:	
	iverilog -Wimplicit -Wportbind -c cmdfile ../top/dual_u2_sim/dual_u2_sim.v -o dual_u2_sim

clean:
	rm -f single_u2_sim dual_u2_sim *.vcd *.lxt

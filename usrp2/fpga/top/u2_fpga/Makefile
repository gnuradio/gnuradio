FILENAME=u2_fpga_top
PARTNUM=xc3s1500-5fg456

all: project command xst ngd ncd ncd2 bit 

xst: 
	xst -ifn ${FILENAME}.cmd -ofn xst.log

ngd: 
	ngdbuild -nt timestamp -p ${PARTNUM} ${FILENAME}

ncd: 
	rm -rf ${FILENAME}.ncd
	map -detail -cm speed -k 8 -retiming on -equivalent_register_removal on -timing -ol high -pr b -p ${PARTNUM} ${FILENAME}.ngd -o ${FILENAME}.ncd ${FILENAME}.pcf

# Place and route ncd file into new ncd file
ncd2:	
	par -ol high -xe n -w ${FILENAME}.ncd ${FILENAME} ${FILENAME}.pcf

bit:	
	bitgen -w ${FILENAME}.ncd -b ${FILENAME}.bit

clean:
	@rm -rf ${FILENAME}.ngc *.lst *.bit *.lso *.xst *.stx *.syr \
	*.ngr *.cmd_log _ngc _xmsgs xst *.html *.srp \
	*.blc *.bld *.ise_ISE_Backup *~ \
	*.pad *.ngm *.ngd *.par *.pcf *.unroutes     \
	*.xpi *.bgn *.drc *.bin *.mrp *.csv *.txt    \
	*.rbt *.ncd ${FILENAME} *_cg templates/ tmp/ \
        output.dat coregen.log *.ngo *.log ${FILENAME}.map \
	${FILENAME}_summary.xml ${FILENAME}_usage.xml ${FILENAME}.twr

command:
	rm -rf ${FILENAME}.cmd
	@echo "identification"       >> ${FILENAME}.cmd
	@echo "status"               >> ${FILENAME}.cmd
	@echo "time short"           >> ${FILENAME}.cmd
	@echo "memory on"            >> ${FILENAME}.cmd
	@echo "run "	             >> ${FILENAME}.cmd
	@echo "-top ${FILENAME}"     >> ${FILENAME}.cmd
	@echo "-ifn ${FILENAME}.prj" >> ${FILENAME}.cmd
	@echo "-ifmt Verilog "       >> ${FILENAME}.cmd
	@echo "-ofn ${FILENAME} "    >> ${FILENAME}.cmd
	@echo "-p ${PARTNUM}"        >> ${FILENAME}.cmd
	@echo "-bufg 6"              >> ${FILENAME}.cmd
	@echo "-vlgincdir { ../../opencores/i2c/rtl/verilog ../../eth/rtl/verilog/ ../../opencores/spi/rtl/verilog}"  >> ${FILENAME}.cmd

project:
	rm -f ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/TECH/duram.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/sign_extend.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cordic_stage.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cic_int_shifter.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cic_dec_shifter.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../opencores/i2c/rtl/verilog/i2c_master_bit_ctrl.v" '   	>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_regfile.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_fetch.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_decode.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_control.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_aslu.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/miim/eth_shiftreg.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/miim/eth_outputcontrol.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/miim/eth_clockgen.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/TECH/eth_clk_switch.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/TECH/eth_clk_div2.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/Reg_int.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/RMON/RMON_dpram.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/RMON/RMON_ctrl.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/RMON/RMON_addr_gen.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/flow_ctrl.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/Ramdon_gen.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/MAC_tx_ctrl.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/MAC_tx_addr_add.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/MAC_tx_FF.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx/CRC_gen.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx/MAC_rx_ctrl.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx/MAC_rx_add_chk.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx/MAC_rx_FF.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx/CRC_chk.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx/Broadcast_filter.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../control_lib/ram_2port.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cordic.v" '   					>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cic_interp.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/cic_decim.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../opencores/spi/rtl/verilog/spi_shift.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/spi/rtl/verilog/spi_clgen.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/i2c/rtl/verilog/i2c_master_byte_ctrl.v" '   	>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_core.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/eth_miim.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/RMON.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/Phy_int.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_tx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_rx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/Clk_ctrl.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/strobe_gen.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/ss_rcvr.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/shortfifo.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/setting_reg.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/mux8.v" '   					>> ${FILENAME}.prj
	@echo '`include "../../control_lib/mux4.v" '   					>> ${FILENAME}.prj
	@echo '`include "../../control_lib/longfifo.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/decoder_3_8.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/buffer_int.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/CRC16_D16.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/tx_control.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/rx_control.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/dsp_core_tx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../sdr_lib/dsp_core_rx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../opencores/spi/rtl/verilog/spi_top.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/simple_pic/rtl/simple_pic.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/i2c/rtl/verilog/i2c_master_top.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../opencores/aemb/rtl/verilog/aeMB_core_BE.v" '   		>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/MAC_top.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/mac_txfifo_int.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../eth/mac_rxfifo_int.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/wb_readback_mux.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../control_lib/wb_1master.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/timer.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/system_control.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../control_lib/settings_bus.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/serdes_tx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/serdes_rx.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/ram_wb_harvard.v" '   			>> ${FILENAME}.prj
	@echo '`include "../../control_lib/ram_loader.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/nsgpio.v" '   				>> ${FILENAME}.prj
	@echo '`include "../../control_lib/buffer_pool.v" '   				>> ${FILENAME}.prj
	@echo '`include "../u2_basic/u2_basic.v" '   					>> ${FILENAME}.prj
	@echo '`include "u2_fpga_top.v" '   						>> ${FILENAME}.prj
	@echo '`include "../../eth/rtl/verilog/elastic_buffer.v" '   			>> ${FILENAME}.prj

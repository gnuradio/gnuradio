##############################################################################
#
# Makefile for the spi_boot project.
#
# The dependencies for all VHDL source files are stored here.
#
# Copyright (c) 2005, Arnim Laeuger (arniml@opencores.org)
#
# All rights reserved
#
##############################################################################


PROJECT_DIR = ../..
RTL_DIR     = $(PROJECT_DIR)/rtl/vhdl
BENCH_DIR   = $(PROJECT_DIR)/bench/vhdl



ANALYZE=ghdl -a --std=87 --workdir=work
ELABORATE=ghdl -e --std=87 --workdir=work

.PHONY: all
all: work elaborate

work:
	mkdir work

work/spi_boot_pack-p.o: $(RTL_DIR)/spi_boot_pack-p.vhd
	$(ANALYZE) $(RTL_DIR)/spi_boot_pack-p.vhd

work/spi_counter.o: $(RTL_DIR)/spi_counter.vhd \
                    work/spi_boot_pack-p.o
	$(ANALYZE) $(RTL_DIR)/spi_counter.vhd
work/spi_counter-c.o: $(RTL_DIR)/spi_counter-c.vhd \
                      work/spi_counter.o
	$(ANALYZE) $(RTL_DIR)/spi_counter-c.vhd

work/spi_boot.o: $(RTL_DIR)/spi_boot.vhd \
                 work/spi_boot_pack-p.o
	$(ANALYZE) $(RTL_DIR)/spi_boot.vhd
work/spi_boot-c.o: $(RTL_DIR)/spi_boot-c.vhd \
                   work/spi_boot.o           \
                   work/spi_counter-c.o
	$(ANALYZE) $(RTL_DIR)/spi_boot-c.vhd

work/chip-e.o: $(RTL_DIR)/chip-e.vhd
	$(ANALYZE) $(RTL_DIR)/chip-e.vhd

work/chip-full-a.o: $(RTL_DIR)/chip-full-a.vhd \
                    work/chip-e.o
	$(ANALYZE) $(RTL_DIR)/chip-full-a.vhd
work/chip-full-c.o: $(RTL_DIR)/chip-full-c.vhd \
                    work/chip-full-a.o         \
                    work/spi_boot-c.o
	$(ANALYZE) $(RTL_DIR)/chip-full-c.vhd

work/chip-mmc-a.o: $(RTL_DIR)/chip-mmc-a.vhd \
                   work/chip-e.o
	$(ANALYZE) $(RTL_DIR)/chip-mmc-a.vhd
work/chip-mmc-c.o: $(RTL_DIR)/chip-mmc-c.vhd \
                   work/chip-mmc-a.o         \
                   work/spi_boot-c.o
	$(ANALYZE) $(RTL_DIR)/chip-mmc-c.vhd

work/chip-sd-a.o: $(RTL_DIR)/chip-sd-a.vhd \
                  work/chip-e.o
	$(ANALYZE) $(RTL_DIR)/chip-sd-a.vhd
work/chip-sd-c.o: $(RTL_DIR)/chip-sd-c.vhd \
                  work/chip-sd-a.o           \
                  work/spi_boot-c.o
	$(ANALYZE) $(RTL_DIR)/chip-sd-c.vhd

work/chip-minimal-a.o: $(RTL_DIR)/chip-minimal-a.vhd \
                       work/chip-e.o
	$(ANALYZE) $(RTL_DIR)/chip-minimal-a.vhd
work/chip-minimal-c.o: $(RTL_DIR)/chip-minimal-c.vhd \
                       work/chip-minimal-a.o           \
                       work/spi_boot-c.o
	$(ANALYZE) $(RTL_DIR)/chip-minimal-c.vhd

work/ram_loader.o: $(RTL_DIR)/sample/ram_loader.vhd
	$(ANALYZE) $(RTL_DIR)/sample/ram_loader.vhd
work/ram_loader-c.o: $(RTL_DIR)/sample/ram_loader-c.vhd \
                     work/ram_loader.o
	$(ANALYZE) $(RTL_DIR)/sample/ram_loader-c.vhd

work/tb_pack-p.o: $(BENCH_DIR)/tb_pack-p.vhd
	$(ANALYZE) $(BENCH_DIR)/tb_pack-p.vhd

work/card.o: $(BENCH_DIR)/card.vhd \
             work/tb_pack-p.o
	$(ANALYZE) $(BENCH_DIR)/card.vhd
work/card-c.o: $(BENCH_DIR)/card-c.vhd \
               work/card.o
	$(ANALYZE) $(BENCH_DIR)/card-c.vhd

work/tb_elem.o: $(BENCH_DIR)/tb_elem.vhd \
                work/spi_boot_pack-p.o   \
                work/tb_pack-p.o
	$(ANALYZE) $(BENCH_DIR)/tb_elem.vhd
work/tb_elem-full-c.o: $(BENCH_DIR)/tb_elem-full-c.vhd \
                       work/tb_elem.o                  \
                       work/chip-full-c.o              \
                       work/card-c.o
	$(ANALYZE) $(BENCH_DIR)/tb_elem-full-c.vhd
work/tb_elem-mmc-c.o: $(BENCH_DIR)/tb_elem-mmc-c.vhd \
                      work/tb_elem.o                 \
                      work/chip-mmc-c.o              \
                      work/card-c.o
	$(ANALYZE) $(BENCH_DIR)/tb_elem-mmc-c.vhd
work/tb_elem-sd-c.o: $(BENCH_DIR)/tb_elem-sd-c.vhd \
                     work/tb_elem.o                \
                     work/chip-sd-c.o              \
                     work/card-c.o
	$(ANALYZE) $(BENCH_DIR)/tb_elem-sd-c.vhd
work/tb_elem-minimal-c.o: $(BENCH_DIR)/tb_elem-minimal-c.vhd \
                          work/tb_elem.o                     \
                          work/chip-minimal-c.o              \
                          work/card-c.o
	$(ANALYZE) $(BENCH_DIR)/tb_elem-minimal-c.vhd

work/tb.o: $(BENCH_DIR)/tb.vhd
	$(ANALYZE) $(BENCH_DIR)/tb.vhd
work/tb-c.o: $(BENCH_DIR)/tb-c.vhd    \
             work/tb.o                \
             work/tb_elem-full-c.o    \
             work/tb_elem-mmc-c.o     \
             work/tb_elem-sd-c.o      \
             work/tb_elem-minimal-c.o
	$(ANALYZE) $(BENCH_DIR)/tb-c.vhd

work/tb_rl.o: $(BENCH_DIR)/tb_rl.vhd
	$(ANALYZE) $(BENCH_DIR)/tb_rl.vhd
work/tb_rl-c.o: $(BENCH_DIR)/tb_rl-c.vhd \
                work/tb_rl.o             \
                work/chip-full-c.o       \
                work/card-c.o            \
                work/ram_loader-c.o
	$(ANALYZE) $(BENCH_DIR)/tb_rl-c.vhd


.PHONY: elaborate
elaborate: tb_behav_c0 tb_rl_behav_c0

tb_behav_c0: work/tb-c.o
	$(ELABORATE) tb_behav_c0; \
	strip tb_behav_c0

tb_rl_behav_c0: work/tb_rl-c.o
	$(ELABORATE) tb_rl_behav_c0; \
	strip tb_rl_behav_c0

.PHONY: analyze
analyze: work/tb-c.o work/tb_rl-c.o

.PHONY: clean
clean:
	rm -rf work tb_behav_c0 tb_rl_behav_c0 *~

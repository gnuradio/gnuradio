-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Testbench
--
-- $Id: tb.vhd,v 1.1 2005/02/08 21:09:20 arniml Exp $
--
-- Copyright (c) 2005, Arnim Laeuger (arniml@opencores.org)
--
-- All rights reserved, see COPYING.
--
-- Redistribution and use in source and synthezised forms, with or without
-- modification, are permitted provided that the following conditions are met:
--
-- Redistributions of source code must retain the above copyright notice,
-- this list of conditions and the following disclaimer.
--
-- Redistributions in synthesized form must reproduce the above copyright
-- notice, this list of conditions and the following disclaimer in the
-- documentation and/or other materials provided with the distribution.
--
-- Neither the name of the author nor the names of other contributors may
-- be used to endorse or promote products derived from this software without
-- specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
-- PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
-- POSSIBILITY OF SUCH DAMAGE.
--
-- Please report bugs to the author, but before you do so, please
-- make sure that this is not a derivative work and that
-- you have the latest version of this file.
--
-- The latest version of this file can be found at:
--      http://www.opencores.org/projects.cgi/web/spi_boot/overview
--
-------------------------------------------------------------------------------

entity tb is

end tb;


library ieee;
use ieee.std_logic_1164.all;

architecture behav of tb is

  component tb_elem
    generic (
      chip_type_g   : string := "none";
      has_sd_card_g : integer := 1
    );
    port (
      clk_i   : in  std_logic;
      reset_i : in  std_logic;
      eos_o   : out boolean
    );
  end component;

  constant period_c      : time := 100 ns;
  constant reset_level_c : integer := 0;

  signal clk_s   : std_logic;
  signal reset_s : std_logic;

  signal eos_full_s,
         eos_mmc_s,
         eos_sd_s,
         eos_minimal_s : boolean;

begin


  -----------------------------------------------------------------------------
  -- Testbench element including full featured chip
  -----------------------------------------------------------------------------
  tb_elem_full_b : tb_elem
    generic map (
      chip_type_g   => "Full Chip",
      has_sd_card_g => 1
    )
    port map (
      clk_i   => clk_s,
      reset_i => reset_s,
      eos_o   => eos_full_s
    );


  -----------------------------------------------------------------------------
  -- Testbench element including MMC chip
  -----------------------------------------------------------------------------
  tb_elem_mmc_b : tb_elem
    generic map (
      chip_type_g   => "MMC Chip",
      has_sd_card_g => 0
    )
    port map (
      clk_i   => clk_s,
      reset_i => reset_s,
      eos_o   => eos_mmc_s
    );


  -----------------------------------------------------------------------------
  -- Testbench element including SD chip
  -----------------------------------------------------------------------------
  tb_elem_sd_b : tb_elem
    generic map (
      chip_type_g   => "SD Chip",
      has_sd_card_g => 1
    )
    port map (
      clk_i   => clk_s,
      reset_i => reset_s,
      eos_o   => eos_sd_s
    );


  -----------------------------------------------------------------------------
  -- Testbench element including cip with minimal features
  -----------------------------------------------------------------------------
  tb_elem_minimal_b : tb_elem
    generic map (
      chip_type_g   => "Minimal Chip",
      has_sd_card_g => 0
    )
    port map (
      clk_i   => clk_s,
      reset_i => reset_s,
      eos_o   => eos_minimal_s
    );


  -----------------------------------------------------------------------------
  -- Clock Generator
  -----------------------------------------------------------------------------
  clk: process
  begin
    clk_s <= '0';
    wait for period_c / 2;
    clk_s <= '1';
    wait for period_c / 2;
  end process clk;


  -----------------------------------------------------------------------------
  -- Reset Generator
  -----------------------------------------------------------------------------
  reset: process
  begin
    if reset_level_c = 0 then
      reset_s <= '0';
    else
      reset_s <= '1';
    end if;

    wait for period_c * 4 + 10 ns;

    reset_s <= not reset_s;

    wait;
  end process reset;


  -----------------------------------------------------------------------------
  -- End Of Simulation Detection
  -----------------------------------------------------------------------------
  eos: process (eos_full_s,
                eos_mmc_s,
                eos_sd_s,
                eos_minimal_s)
  begin

    if eos_full_s and eos_mmc_s and eos_sd_s and eos_minimal_s then
      assert false
        report "End of Simulation."
        severity failure;
    end if;

  end process eos;

end behav;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: tb.vhd,v $
-- Revision 1.1  2005/02/08 21:09:20  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

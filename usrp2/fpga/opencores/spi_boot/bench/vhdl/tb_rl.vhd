-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Testbench for ram_loader
--
-- $Id: tb_rl.vhd,v 1.1 2005/04/10 18:07:25 arniml Exp $
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

entity tb_rl is

end tb_rl;


library ieee;
use ieee.std_logic_1164.all;

architecture behav of tb_rl is

  component chip
    port (
      clk_i          : in  std_logic;
      reset_i        : in  std_logic;
      set_sel_n_i    : in  std_logic_vector(3 downto 0);
      spi_clk_o      : out std_logic;
      spi_cs_n_o     : out std_logic;
      spi_data_in_i  : in  std_logic;
      spi_data_out_o : out std_logic;
      start_i        : in  std_logic;
      mode_i         : in  std_logic;
      config_n_o     : out std_logic;
      detached_o     : out std_logic;
      cfg_init_n_i   : in  std_logic;
      cfg_done_i     : in  std_logic;
      dat_done_i     : in  std_logic;
      cfg_clk_o      : out std_logic;
      cfg_dat_o      : out std_logic
    );
  end component;

  component card
    generic (
      card_type_g  : string := "none";
      is_sd_card_g : integer := 1
    );
    port (
      spi_clk_i  : in  std_logic;
      spi_cs_n_i : in  std_logic;
      spi_data_i : in  std_logic;
      spi_data_o : out std_logic
    );
  end component;

  component ram_loader
    port (
      clk_i      : in    std_logic;
      reset_i    : in    std_logic;
      lamp_o     : out   std_logic;
      cfg_clk_i  : in    std_logic;
      cfg_data_i : in    std_logic;
      start_o    : out   std_logic;
      mode_o     : out   std_logic;
      done_o     : out   std_logic;
      detached_i : in    std_logic;
      ram_addr_o : out   std_logic_vector(15 downto 0);
      ram_data_b : out   std_logic_vector( 7 downto 0);
      ram_ce_no  : out   std_logic_vector( 3 downto 0);
      ram_oe_no  : out   std_logic;
      ram_we_no  : out   std_logic
    );
  end component;

  constant period_c      : time := 100 ns;
  constant rl_period_c   : time :=  20 ns;
  constant reset_level_c : integer := 0;

  signal clk_s   : std_logic;
  signal rl_clk_s: std_logic;
  signal reset_s : std_logic;

  -- SPI interface signals
  signal spi_clk_s            : std_logic;
  signal spi_data_to_card_s   : std_logic;
  signal spi_data_from_card_s : std_logic;
  signal spi_cs_n_s           : std_logic;

  -- config related signals
  signal start_s      : std_logic;
  signal mode_s       : std_logic;
  signal config_n_s   : std_logic;
  signal cfg_init_n_s : std_logic;
  signal cfg_done_s   : std_logic;
  signal dat_done_s   : std_logic;
  signal cfg_clk_s    : std_logic;
  signal cfg_dat_s    : std_logic;
  signal detached_s   : std_logic;

  signal set_sel_n_s : std_logic_vector(3 downto 0);

begin

  set_sel_n_s  <= (others => '1');
  cfg_init_n_s <= '1';
  cfg_done_s   <= '1';

  -----------------------------------------------------------------------------
  -- DUT
  -----------------------------------------------------------------------------
  dut_b : chip
    port map (
      clk_i          => clk_s,
      reset_i        => reset_s,
      set_sel_n_i    => set_sel_n_s,
      spi_clk_o      => spi_clk_s,
      spi_cs_n_o     => spi_cs_n_s,
      spi_data_in_i  => spi_data_from_card_s,
      spi_data_out_o => spi_data_to_card_s,
      start_i        => start_s,
      mode_i         => mode_s,
      config_n_o     => config_n_s,
      detached_o     => detached_s,
      cfg_init_n_i   => cfg_init_n_s,
      cfg_done_i     => cfg_done_s,
      dat_done_i     => dat_done_s,
      cfg_clk_o      => cfg_clk_s,
      cfg_dat_o      => cfg_dat_s
    );

  card_b : card
    generic map (
      card_type_g  => "Full Chip",
      is_sd_card_g => 1
    )
    port map (
      spi_clk_i  => spi_clk_s,
      spi_cs_n_i => spi_cs_n_s,
      spi_data_i => spi_data_to_card_s,
      spi_data_o => spi_data_from_card_s
    );

  rl_b : ram_loader
    port map (
      clk_i      => rl_clk_s,
      reset_i    => reset_s,
      lamp_o     => open,
      cfg_clk_i  => cfg_clk_s,
      cfg_data_i => cfg_dat_s,
      start_o    => start_s,
      mode_o     => mode_s,
      done_o     => dat_done_s,
      detached_i => detached_s,
      ram_addr_o => open,
      ram_data_b => open,
      ram_ce_no  => open,
      ram_oe_no  => open,
      ram_we_no  => open
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

  rl_clk: process
  begin
    rl_clk_s <= '0';
    wait for rl_period_c / 2;
    rl_clk_s <= '1';
    wait for rl_period_c / 2;
  end process rl_clk;


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
  -- End of Simulation
  -----------------------------------------------------------------------------
  eos: process
  begin
    wait for 4 ms;
    assert false
      report "No checks have been performed. Investigate waveforms."
      severity note;
    assert false
      report "End of simulation."
      severity failure;
  end process eos;

end behav;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: tb_rl.vhd,v $
-- Revision 1.1  2005/04/10 18:07:25  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Generic testbench element for a specific feature set
--
-- $Id: tb_elem.vhd,v 1.7 2005/04/07 20:43:36 arniml Exp $
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

library ieee;
use ieee.std_logic_1164.all;


entity tb_elem is

  generic (
    chip_type_g   : string := "none";
    has_sd_card_g : integer := 1
  );
  port (
    clk_i   : in  std_logic;
    reset_i : in  std_logic;
    eos_o   : out boolean
  );

end tb_elem;


library ieee;
use ieee.numeric_std.all;
library std;
use std.textio.all;

use work.spi_boot_pack.all;
use work.tb_pack.all;

architecture behav of tb_elem is

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
  signal data_s       : unsigned(7 downto 0);

  signal set_sel_n_s : std_logic_vector(3 downto 0);

  constant verbose_c : boolean := false;

begin

  -- weak pull-ups
  spi_clk_s          <= 'H';
  spi_cs_n_s         <= 'H';
  spi_data_to_card_s <= 'H';

  -----------------------------------------------------------------------------
  -- DUT
  -----------------------------------------------------------------------------
  dut_b : chip
    port map (
      clk_i          => clk_i,
      reset_i        => reset_s,
      set_sel_n_i    => set_sel_n_s,
      spi_clk_o      => spi_clk_s,
      spi_cs_n_o     => spi_cs_n_s,
      spi_data_in_i  => spi_data_from_card_s,
      spi_data_out_o => spi_data_to_card_s,
      start_i        => start_s,
      mode_i         => mode_s,
      config_n_o     => config_n_s,
      detached_o     => open,
      cfg_init_n_i   => cfg_init_n_s,
      cfg_done_i     => cfg_done_s,
      dat_done_i     => dat_done_s,
      cfg_clk_o      => cfg_clk_s,
      cfg_dat_o      => cfg_dat_s
    );

  card_b : card
    generic map (
      card_type_g  => chip_type_g,
      is_sd_card_g => has_sd_card_g
    )
    port map (
      spi_clk_i  => spi_clk_s,
      spi_cs_n_i => spi_cs_n_s,
      spi_data_i => spi_data_to_card_s,
      spi_data_o => spi_data_from_card_s
    );


  -----------------------------------------------------------------------------
  -- DUT Stimuli
  --
  stim: process

    procedure rise_cfg_clk(num : integer) is
    begin
      for i in 1 to num loop
        wait until cfg_clk_s'event and cfg_clk_s = '1';
      end loop;
    end rise_cfg_clk;

--     procedure fall_cfg_clk(num : integer) is
--     begin
--       for i in 1 to num loop
--         wait until cfg_clk_s'event and cfg_clk_s = '0';
--       end loop;
--     end fall_cfg_clk;

    procedure rise_clk(num : integer) is
    begin
      for i in 1 to num loop
        wait until clk_i'event and clk_i = '1';
      end loop;
    end rise_clk;

    procedure read_check_byte(ref : unsigned(7 downto 0)) is
      variable byte_v : unsigned(7 downto 0);
      variable dump_line : line;
    begin
      for bit in 7 downto 0 loop
        rise_cfg_clk(1);
        byte_v(bit) := cfg_dat_s;
      end loop;
      data_s <= byte_v;

      if byte_v /= ref then
        write(dump_line, chip_type_g);
        write(dump_line, string'(" at "));
        write(dump_line, now);
        write(dump_line, string'(": read_check_byte failed "));
        write(dump_line, to_integer(byte_v));
        write(dump_line, string'(" "));
        write(dump_line, to_integer(ref));
        writeline(output, dump_line);
      end if;
    end read_check_byte;

    variable dump_line : line;
    variable addr_v    : unsigned(31 downto 0);
    variable temp_v    : unsigned( 7 downto 0);
    variable set_sel_v : unsigned(3 downto 0);

  begin
    -- default assignments
    -- these defaults show the required pull resistors
    -- except start_i as this must be pulled high for automatic start
    start_s      <= '0';
    mode_s       <= '1';
    cfg_init_n_s <= '1';
    cfg_done_s   <= '0';
    dat_done_s   <= '1';
    data_s       <= (others => '1');
    addr_v       := (others => '0');
    eos_o        <= false;
    set_sel_n_s  <= (others => '1');
    reset_s      <= '0';

    -- loop through some sets
    for set in 0 to 3 loop
      set_sel_v := to_unsigned(set, 4);
      addr_v(23 downto 20) := set_sel_v;  -- must match num_bits_per_img_g
                                          -- plus width_img_cnt_g
      set_sel_n_s <= not std_logic_vector(set_sel_v);

      assert not verbose_c
        report chip_type_g & ": Processing set " & to_string(set)
        severity note;

      wait for 100 us;
      reset_s <= '1';

      assert not verbose_c
        report chip_type_g & ": Requesting image 0"
        severity note;

      -- signal start
      start_s    <= '1';
      mode_s     <= '1';
      cfg_done_s <= '0';
      addr_v(19 downto 0) := (others => '0');
      wait until config_n_s = '0';
      -- run through configuration sequence
      rise_clk(1);
      cfg_init_n_s <= '0';
      rise_clk(3);
      cfg_init_n_s <= '1';

      -- and receive 32 bytes from image 0
      for i in 1 to 32 loop
        temp_v := addr_v(0) & calc_crc(addr_v);
        read_check_byte(temp_v);
        addr_v := addr_v + 1;
      end loop;
      start_s    <= '0';
      cfg_done_s <= '1';

      rise_clk(10);

      assert not verbose_c
        report chip_type_g & ": Requesting image 1"
        severity note;

      -- request next image
      mode_s  <= '0';
      start_s <= '1';
      addr_v(17 downto  0) := (others => '0');
      addr_v(19 downto 18) := "01"; -- must match num_bits_per_img_g in chip-*-a.vhd
      dat_done_s <= '0';

      -- receive another 32 bytes from image 1
      for i in 1 to 32 loop
        temp_v := addr_v(0) & calc_crc(addr_v);
        read_check_byte(temp_v);
        addr_v := addr_v + 1;
      end loop;
      start_s    <= '0';
      dat_done_s <= '1';
      

      rise_clk(10);

      assert not verbose_c
        report chip_type_g & ": Requesting image 2"
        severity note;

      -- request next image
      mode_s  <= '1';
      start_s <= '1';
      addr_v(17 downto  0) := (others => '0');
      addr_v(19 downto 18) := "10"; -- must match num_bits_per_img_g in chip-*-a.vhd

      wait until config_n_s = '0';
      -- run through configuration sequence
      rise_clk(1);
      cfg_done_s   <= '0';
      cfg_init_n_s <= '0';
      rise_clk(3);
      cfg_init_n_s <= '1';

      -- receive another 32 bytes from image 2
      for i in 1 to 32 loop
        temp_v := addr_v(0) & calc_crc(addr_v);
        read_check_byte(temp_v);
        addr_v := addr_v + 1;
      end loop;
      start_s    <= '0';
      cfg_done_s <= '1';

      -- give dut a chance to stop current transfer
      wait until spi_cs_n_s = '1';
      rise_clk(10);

      reset_s <= '0';
    end loop;

    eos_o <= true;
    wait;
  end process stim;
  --
  -----------------------------------------------------------------------------

end behav;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: tb_elem.vhd,v $
-- Revision 1.7  2005/04/07 20:43:36  arniml
-- add new port detached_o
--
-- Revision 1.6  2005/03/09 19:48:04  arniml
-- make verbosity level switchable
--
-- Revision 1.5  2005/03/08 22:06:21  arniml
-- added set selection
--
-- Revision 1.4  2005/02/17 18:59:23  arniml
-- clarify wording for images
--
-- Revision 1.3  2005/02/16 19:34:56  arniml
-- add weak pull-ups for SPI lines
--
-- Revision 1.2  2005/02/13 17:14:03  arniml
-- change dat_done handling
--
-- Revision 1.1  2005/02/08 21:09:20  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

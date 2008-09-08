-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Chip toplevel design with MMC feature set
--
-- $Id: chip-mmc-a.vhd,v 1.6 2005/04/07 20:44:23 arniml Exp $
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


architecture mmc of chip is

  component spi_boot
    generic (
      width_set_sel_g      : integer := 4;
      width_bit_cnt_g      : integer := 6;
      width_img_cnt_g      : integer := 2;
      num_bits_per_img_g   : integer := 18;
      sd_init_g            : integer := 0;
      mmc_compat_clk_div_g : integer := 0;
      width_mmc_clk_div_g  : integer := 0;
      reset_level_g        : integer := 0
    );
    port (
      clk_i          : in  std_logic;
      reset_i        : in  std_logic;
      set_sel_i      : in  std_logic_vector(width_set_sel_g-1 downto 0);
      spi_clk_o      : out std_logic;
      spi_cs_n_o     : out std_logic;
      spi_data_in_i  : in  std_logic;
      spi_data_out_o : out std_logic;
      spi_en_outs_o  : out std_logic;
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

  signal spi_clk_s      : std_logic;
  signal spi_cs_n_s     : std_logic;
  signal spi_data_out_s : std_logic;
  signal spi_en_outs_s  : std_logic;

  signal set_sel_s      : std_logic_vector(3 downto 0);

begin

  set_sel_s <= not set_sel_n_i;

  spi_boot_b : spi_boot
    generic map (
      width_set_sel_g      => 4,        -- 16 sets
      width_bit_cnt_g      => 12,       -- 512 bytes per block
      width_img_cnt_g      => 2,        -- 4 images
      num_bits_per_img_g   => 18,       -- 256 kByte per image
      sd_init_g            => 0,        -- no SD specific initialization
      mmc_compat_clk_div_g => 13,       -- MMC compat 400 kHz > 10 MHz / (13*2)
      width_mmc_clk_div_g  => 4         -- need 5 bits for MMC compat divider
    )
    port map (
      clk_i                => clk_i,
      reset_i              => reset_i,
      set_sel_i            => set_sel_s,
      spi_clk_o            => spi_clk_s,
      spi_cs_n_o           => spi_cs_n_s,
      spi_data_in_i        => spi_data_in_i,
      spi_data_out_o       => spi_data_out_s,
      spi_en_outs_o        => spi_en_outs_s,
      start_i              => start_i,
      mode_i               => mode_i,
      config_n_o           => config_n_o,
      detached_o           => detached_o,
      cfg_init_n_i         => cfg_init_n_i,
      cfg_done_i           => cfg_done_i,
      dat_done_i           => dat_done_i,
      cfg_clk_o            => cfg_clk_o,
      cfg_dat_o            => cfg_dat_o
    );

  -----------------------------------------------------------------------------
  -- Three state drivers for SPI outputs.
  -----------------------------------------------------------------------------
  spi_clk_o      <=   spi_clk_s
                    when spi_en_outs_s = '1' else
                      'Z';
  spi_cs_n_o     <=   spi_cs_n_s
                    when spi_en_outs_s = '1' else
                      'Z';
  spi_data_out_o <=   spi_data_out_s
                    when spi_en_outs_s = '1' else
                      'Z';

end mmc;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: chip-mmc-a.vhd,v $
-- Revision 1.6  2005/04/07 20:44:23  arniml
-- add new port detached_o
--
-- Revision 1.5  2005/03/09 19:48:34  arniml
-- invert level of set_sel input
--
-- Revision 1.4  2005/03/08 22:07:12  arniml
-- added set selection
--
-- Revision 1.3  2005/02/18 06:42:13  arniml
-- clarify wording for images
--
-- Revision 1.2  2005/02/16 18:54:39  arniml
-- added tri-state drivers for spi outputs
--
-- Revision 1.1  2005/02/08 20:41:32  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

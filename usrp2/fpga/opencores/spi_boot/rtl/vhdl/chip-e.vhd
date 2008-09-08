-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Chip toplevel
--
-- $Id: chip-e.vhd,v 1.3 2005/04/07 20:44:23 arniml Exp $
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


entity chip is

  port (
    -- System Interface -------------------------------------------------------
    clk_i          : in  std_logic;
    reset_i        : in  std_logic;
    set_sel_n_i    : in  std_logic_vector(3 downto 0);
    -- SD Card Interface ------------------------------------------------------
    spi_clk_o      : out std_logic;
    spi_cs_n_o     : out std_logic;
    spi_data_in_i  : in  std_logic;
    spi_data_out_o : out std_logic;
    -- FPGA Configuration Interface -------------------------------------------
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

end chip;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: chip-e.vhd,v $
-- Revision 1.3  2005/04/07 20:44:23  arniml
-- add new port detached_o
--
-- Revision 1.2  2005/03/08 22:07:11  arniml
-- added set selection
--
-- Revision 1.1  2005/02/08 20:41:30  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

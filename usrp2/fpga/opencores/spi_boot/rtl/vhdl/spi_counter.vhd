-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Generic counter module
--
-- $Id: spi_counter.vhd,v 1.2 2007/02/25 18:24:12 arniml Exp $
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


entity spi_counter is

  generic (
    cnt_width_g   : integer := 4;
    cnt_max_g     : integer := 15
  );

  port (
    clk_i      : in  std_logic;
    reset_i    : in  boolean;
    cnt_en_i   : in  boolean;
    cnt_o      : out std_logic_vector(cnt_width_g-1 downto 0);
    cnt_ovfl_o : out boolean
  );

end spi_counter;


library ieee;
use ieee.numeric_std.all;
use work.spi_boot_pack.all;

architecture rtl of spi_counter is

  signal cnt_q      : unsigned(cnt_width_g-1 downto 0);
  signal cnt_ovfl_s : boolean;

begin

  cnt: process (clk_i, reset_i)
  begin
    if reset_i then
      cnt_q <= (others => '0');

    elsif clk_i'event and clk_i = '1' then
      if cnt_en_i then
        if not cnt_ovfl_s then
          cnt_q <= cnt_q + 1;
        else
          cnt_q <= (others => '0');
        end if;
      end if;
    end if;
  end process cnt;

  cnt_ovfl_s <= cnt_q = cnt_max_g;


  -----------------------------------------------------------------------------
  -- Output Mapping
  -----------------------------------------------------------------------------
  cnt_ovfl_o <= cnt_ovfl_s;
  cnt_o      <= std_logic_vector(cnt_q);

end rtl;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: spi_counter.vhd,v $
-- Revision 1.2  2007/02/25 18:24:12  arniml
-- fix type handling of resets
--
-- Revision 1.1  2005/02/08 20:41:33  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

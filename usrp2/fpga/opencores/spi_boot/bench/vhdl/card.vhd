-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Simple SD and MMC model
--
-- $Id: card.vhd,v 1.2 2005/02/13 17:06:22 arniml Exp $
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


entity card is

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

end card;


library ieee;
use ieee.numeric_std.all;
library std;
use std.textio.all;

use work.tb_pack.all;

architecture behav of card is

  signal power_on_n_s : std_logic;
  signal soft_res_n_s : std_logic;
  signal res_n_s      : std_logic;

  signal rx_s : std_logic_vector(47 downto 0);

  signal set_spi_mode_s,
         spi_mode_q       : boolean;
  signal set_idle_mode_s,
         poll_idle_mode_s : boolean;
  signal idle_mode_q      : natural;

  signal block_len_q,
         block_len_s     : unsigned(31 downto 0);
  signal set_block_len_s : boolean;

  signal new_read_addr_s,
         read_addr_q     : unsigned(31 downto 0);
  signal set_read_addr_s,
         inc_read_addr_s : boolean;

  signal cmd_spi_data_s,
         read_spi_data_s : std_logic;
  signal start_read_s    : boolean;
  signal reading_s       : boolean;

  procedure rise_clk is
  begin
    wait until spi_clk_i'event and to_X01(spi_clk_i) = '1';
  end rise_clk;

--  procedure rise_clk(num : natural) is
--  begin
--    for i in 1 to num loop
--      rise_clk;
--    end loop;
--  end rise_clk;

  procedure fall_clk is
  begin
    wait until spi_clk_i'event and to_X01(spi_clk_i) = '0';
  end fall_clk;

  procedure fall_clk(num : natural) is
  begin
    for i in 1 to num loop
      fall_clk;
    end loop;
  end fall_clk;

begin

  res_n_s <= power_on_n_s and soft_res_n_s;

  -----------------------------------------------------------------------------
  -- Power on reset
  -----------------------------------------------------------------------------
  por: process
  begin
    power_on_n_s <= '0';
    wait for 200 ns;
    power_on_n_s <= '1';
    wait;
  end process por;


  -----------------------------------------------------------------------------
  --
  ctrl: process

    function check_crc(payload : in std_logic_vector(47 downto 0))
      return boolean is

    begin

      return calc_crc(payload(47 downto 8)) = payload(7 downto 1);
    end check_crc;

    variable rx_v        : std_logic_vector(47 downto 0);
    variable cmd_v       : std_logic_vector( 5 downto 0);
    variable arg_v       : std_logic_vector(31 downto 0);
    variable crc_v       : std_logic_vector( 6 downto 0);
    variable wrong_v     : std_logic;
    variable read_data_v : boolean;

  begin
    rx_s <= (others => '0');
    set_spi_mode_s   <= false;
    set_idle_mode_s  <= false;
    poll_idle_mode_s <= false;
    cmd_spi_data_s   <= '1';
    soft_res_n_s     <= '1';
    set_block_len_s  <= false;
    block_len_s      <= (others => '0');
    new_read_addr_s  <= (others => '0');
    set_read_addr_s  <= false;
    start_read_s     <= false;
    read_data_v      := false;

    loop

      rise_clk;
      -- wait for startbit of command
      while to_X01(spi_data_i) = '1' loop
        rise_clk;
      end loop;
      rx_v(47) := '0';

      -- read remaining 47 bits of command
      for i in 46 downto 0 loop
        rise_clk;
        rx_v(i) := to_X01(spi_data_i);
      end loop;
      rx_s <= rx_v;

      -- dissect received data
      cmd_v := rx_v(45 downto 40);
      arg_v := rx_v(39 downto  8);
      crc_v := rx_v( 7 downto  1);

      assert spi_mode_q or check_crc(payload => rx_v)
        report "CRC mismatch"
        severity error;

      wrong_v     := '0';
      case cmd_v is
        -- CMD0: GO_IDLE_STATE ------------------------------------------------
        when "000000" =>
          set_spi_mode_s  <= true;
          set_idle_mode_s <= true;
        -- CMD1: SEND_OP_COND -------------------------------------------------
        when "000001" =>
          poll_idle_mode_s <= true;
        -- CMD12: STOP_TRANSMISSION -------------------------------------------
        when "001100" =>
          start_read_s <= false;
          read_data_v  := false;
        -- CMD16: SET_BLOCKLEN ------------------------------------------------
        when "010000" =>
          block_len_s     <= unsigned(arg_v);
          set_block_len_s <= true;
        -- CMD18: READ_MULTIPLE_BLOCK -----------------------------------------
        when "010010" =>
          new_read_addr_s <= unsigned(arg_v);
          set_read_addr_s <= true;
          read_data_v     := true;
        -- CMD55: APPL_CMD ----------------------------------------------------
        when "110111" =>
          -- command only available for SD card
          if is_sd_card_g /= 1 then
            wrong_v := '1';
          end if;
        -- ACMD41: SEND_OP_COND -----------------------------------------------
        when "101001" =>
          -- command only available for SD card
          if is_sd_card_g /= 1 then
            wrong_v := '1';
          else
            poll_idle_mode_s <= true;
          end if;

        when others =>
          wrong_v := '1';
          null;
      end case;


      -- spend some time before removing control signals
      fall_clk(2);
      poll_idle_mode_s <= false;
      set_idle_mode_s <= false;
      fall_clk(6);
      set_spi_mode_s  <= false;
      set_block_len_s <= false;
      set_read_addr_s <= false;

      if reading_s then
        wait until not reading_s;
      end if;


      -- wait for a total two "bytes" before sending out response
      for i in 1 to 8 loop
        fall_clk;
      end loop;

      for i in 7 downto 0 loop
        fall_clk;
        case i is
          when 2 =>
            cmd_spi_data_s <= wrong_v;
          when 0 =>
            if idle_mode_q = 0 then
              cmd_spi_data_s <= '0';
            else
              cmd_spi_data_s <= '1';
            end if;
          when others =>
            cmd_spi_data_s <= '0';
        end case;
      end loop;
      fall_clk;
      cmd_spi_data_s <= '1';

      -- transmit data if requested
      start_read_s <= read_data_v;

    end loop;
  end process ctrl;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  --
  seq: process (res_n_s,
                spi_clk_i,
                set_spi_mode_s,
                set_idle_mode_s,
                poll_idle_mode_s,
                set_block_len_s,
                block_len_s)

  begin
    if res_n_s = '0' then
      spi_mode_q  <= false;
      idle_mode_q <= 5;
      block_len_q <= (others => '0');
      read_addr_q <= (others => '0');

    elsif spi_clk_i'event and spi_clk_i = '1' then
      if set_spi_mode_s then
        spi_mode_q  <= true;
      end if;

      if set_idle_mode_s then
        idle_mode_q  <= 5;
      elsif poll_idle_mode_s then
        if idle_mode_q > 0 then
          idle_mode_q <= idle_mode_q - 1;
        end if;
      end if;

      if set_block_len_s then
        block_len_q <= block_len_s;
      end if;

      if set_read_addr_s then
        read_addr_q <= new_read_addr_s;
      elsif inc_read_addr_s then
        read_addr_q <= read_addr_q + 1;
      end if;

    end if;
  end process seq;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  --
  read_block: process

    variable t_v : unsigned(7 downto 0);

  begin
    -- default assignments
    inc_read_addr_s <= false;
    reading_s       <= false;
    read_spi_data_s <= '1';

    loop
      if not start_read_s then
        wait until start_read_s;
      end if;

      reading_s <= true;

      fall_clk(8);                    -- delay for one "byte"

      -- send data token
      fall_clk(7);                    -- 7 ones in a data token
      read_spi_data_s <= '0';

      -- send payload
      payload: for i in 0 to to_integer(block_len_q)-1 loop
        t_v := read_addr_q(0) & calc_crc(read_addr_q);
        for bit in 7 downto 0 loop
          fall_clk;
          read_spi_data_s <= t_v(bit);

          exit payload when not start_read_s;
        end loop;
        inc_read_addr_s <= true;
        rise_clk;
        inc_read_addr_s <= false;
        wait for 10 ns;
      end loop;

      if start_read_s then
        -- send crc
        for i in 0 to 15 loop
          fall_clk;
          t_v := to_unsigned(i, 8);
          read_spi_data_s <= t_v(0);
        end loop;
        fall_clk;
      end if;

      read_spi_data_s <= '1';
      reading_s <= false;
      -- loop for one "byte"
      fall_clk(8);

    end loop;
  end process read_block;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  --
  clk_check: process (spi_clk_i)

    variable last_rising_v : time := 0 ns;
    variable dump_line     : line;

  begin
    if spi_clk_i'event and spi_clk_i = '1' then
      if is_sd_card_g = 0 and card_type_g /= "Minimal Chip" and
         idle_mode_q > 0 then
        if now - last_rising_v < 2.5 us and last_rising_v > 0 ns then
          write(dump_line, card_type_g);
          write(dump_line, string'(" @ "));
          write(dump_line, now);
          write(dump_line, string'(": Last rising edge of SPI clock "));
          write(dump_line, now - last_rising_v);
          write(dump_line, string'(" ago."));
          writeline(output, dump_line);
        end if;

        last_rising_v := now;
      end if;
    end if;
  end process clk_check;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Output Mapping
  -----------------------------------------------------------------------------
  spi_data_o <=   cmd_spi_data_s and read_spi_data_s
                when spi_cs_n_i = '0' else
                  'Z';

end behav;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: card.vhd,v $
-- Revision 1.2  2005/02/13 17:06:22  arniml
-- handle termination properly
--
-- Revision 1.1  2005/02/08 21:09:20  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

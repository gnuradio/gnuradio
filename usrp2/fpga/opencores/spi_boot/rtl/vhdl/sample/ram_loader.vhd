-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
-- Sample client for loading an image to asynchronous SRAM
--
-- $Id: ram_loader.vhd,v 1.2 2005/04/10 17:17:23 arniml Exp $
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


entity ram_loader is

  port (
    -- Global Interface -------------------------------------------------------
    clk_i      : in    std_logic;
    reset_i    : in    std_logic;
    lamp_o     : out   std_logic;
    -- Config Interface -------------------------------------------------------
    cfg_clk_i  : in    std_logic;
    cfg_data_i : in    std_logic;
    start_o    : out   std_logic;
    mode_o     : out   std_logic;
    done_o     : out   std_logic;
    detached_i : in    std_logic;
    -- Asynchronous RAM Interface ---------------------------------------------
    ram_addr_o : out   std_logic_vector(15 downto 0);
    ram_data_b : out   std_logic_vector( 7 downto 0);
    ram_ce_no  : out   std_logic_vector( 3 downto 0);
    ram_oe_no  : out   std_logic;
    ram_we_no  : out   std_logic
  );

end ram_loader;


library ieee;
use ieee.numeric_std.all;

architecture rtl of ram_loader is

  signal addr_q     : unsigned(17 downto 0);
  signal inc_addr_s : boolean;

  signal shift_dat_q : std_logic_vector(7 downto 0);
  signal ser_dat_q   : std_logic_vector(7 downto 0);
  signal bit_q       : unsigned(2 downto 0);
  signal bit_ovfl_q  : boolean;

  type fsm_t is (IDLE,
                 WE_ON,
                 WE_OFF,
                 INC_ADDR1, INC_ADDR2,
                 FINISHED);
  signal fsm_s,
         fsm_q  : fsm_t;
  signal done_q          : std_logic;
  signal done_s          : boolean;
  signal mode_q,
         mode_s          : std_logic;

  signal ram_we_n_q,
         ram_we_n_s  : std_logic;
  signal ram_ce_n_q,
         ram_ce_n_s  : std_logic_vector(3 downto 0);

  type start_fsm_t is (WAIT_DETACH,
                       CHECK_NO_DONE,
                       WAIT_DONE);
  signal start_fsm_s,
         start_fsm_q  : start_fsm_t;

  signal start_s,
         start_q         : std_logic;
  signal enable_s,
         enable_q        : boolean;

begin

  -----------------------------------------------------------------------------
  -- Process seq
  --
  -- Purpose:
  --   Implements the sequential elements clocked with cfg_clk_i.
  --
  seq: process (cfg_clk_i, reset_i)
  begin
    if reset_i = '0' then
      addr_q      <= (others => '0');
      shift_dat_q <= (others => '0');
      ser_dat_q   <= (others => '0');
      bit_q       <= (others => '0');
      bit_ovfl_q  <= false;
      fsm_q       <= IDLE;
      ram_we_n_q  <= '1';
      ram_ce_n_q  <= (others => '1');
      done_q      <= '0';
      mode_q      <= '0';

    elsif cfg_clk_i'event and cfg_clk_i = '1' then
      if inc_addr_s then
        addr_q <= addr_q + 1;
      end if;

      if enable_q then
        bit_q      <= bit_q + 1;
        bit_ovfl_q <= bit_q = 7;

        shift_dat_q(0) <= cfg_data_i;
        shift_dat_q(7 downto 1) <= shift_dat_q(6 downto 0);
      end if;

      -- update register when 8 serial bits have been shifted in
      if bit_ovfl_q then
        ser_dat_q <= shift_dat_q;
      end if;

      fsm_q <= fsm_s;

      ram_we_n_q <= ram_we_n_s;
      ram_ce_n_q <= ram_ce_n_s;

      -- done only settable once
      if done_s then
        done_q <= '1';
      end if;

      mode_q <= mode_s;

    end if;
  end process seq;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process fsm
  --
  -- Purpose:
  --   Implements the combinational logic of the RAM loader FSM.
  --
  fsm: process (fsm_q,
                bit_ovfl_q,
                start_q,
                addr_q)
  begin
    -- default assignments
    inc_addr_s      <= false;
    ram_we_n_s      <= '1';
    done_s          <= false;
    fsm_s           <= IDLE;
    lamp_o          <= '1';
    mode_s          <= '0';

    case fsm_q is
      when IDLE =>
        lamp_o <= '0';
        if start_q = '1' then
          if bit_ovfl_q then
            fsm_s <= WE_ON;
          end if;
        end if;

      when WE_ON =>
        ram_we_n_s <= '0';
        fsm_s      <= WE_OFF;

      when WE_OFF =>
        fsm_s <= INC_ADDR1;

      when INC_ADDR1 =>
        fsm_s      <= INC_ADDR2;

      when INC_ADDR2 =>
        if addr_q = "001111111111111111" then  -- load only 64k
          fsm_s <= FINISHED;
          done_s <= true;
          mode_s <= '1';
        else
          inc_addr_s <= true;
          fsm_s      <= IDLE;
        end if;

      when FINISHED =>
        fsm_s  <= FINISHED;
        lamp_o <= '1';
        mode_s <= '1';

      when others =>
    end case;

  end process fsm;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process ce_gen
  --
  -- Purpose:
  --   Generates the four CE signals for the external RAM chips.
  --
  ce_gen: process (addr_q)
  begin
    ram_ce_n_s <= (others => '1');
    ram_ce_n_s(to_integer(addr_q(17 downto 16))) <= '0';
  end process ce_gen;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process start_seq
  --
  -- Purpose:
  --   Implements the sequential elements clocked with clk_i.
  --
  start_seq: process (clk_i, reset_i)
  begin
    if reset_i = '0' then
      start_fsm_q <= WAIT_DETACH;
      start_q     <= '0';
      enable_q    <= false;

    elsif clk_i'event and clk_i = '1' then
      start_fsm_q <= start_fsm_s;

      enable_q    <= enable_s;

      start_q     <= start_s;

    end if;
  end process start_seq;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process start_comb
  --
  -- Purpose:
  --   Implements the combinational logic of the start FSM.
  --
  start_comb: process (start_fsm_q,
                       detached_i,
                       done_q,
                       enable_q,
                       start_q)
  begin
    -- default assignments
    start_fsm_s <= WAIT_DETACH;
    enable_s    <= enable_q;
    start_s     <= start_q;

    case start_fsm_q is
      -- Wait for detached_i to become '1'
      -- This state is entered/left twice:
      -- 1. after reset to start the data download
      -- 2. after data download to start the next configuration cycle
      when WAIT_DETACH =>
        if detached_i = '1' then
          start_fsm_s <= CHECK_NO_DONE;
          enable_s    <= true;
          start_s     <= '1';

        else
          start_fsm_s <= WAIT_DETACH;
        end if;

      -- Wait until done_q is '0'
      -- This ensures that the FSM stalls when it has started the configuration
      -- download. There must be no further action in this case.
      when CHECK_NO_DONE =>
        if done_q = '0' then
          start_fsm_s <= WAIT_DONE;
        else
          start_fsm_s <= CHECK_NO_DONE;
        end if;

      -- Wait until done_q is '1'
      -- done_q is the signal that the main FSM has finished its work. We
      -- need to start the configuration download.
      when WAIT_DONE =>
        if done_q = '1' then
          start_fsm_s <= WAIT_DETACH;
          enable_s    <= false;
          start_s     <= '0';
        else
          start_fsm_s <= WAIT_DONE;
        end if;

      when others =>
        null;

    end case;

  end process start_comb;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Output Mapping
  -----------------------------------------------------------------------------
  start_o    <= start_q;
  mode_o     <= mode_q;
  done_o     <=   done_q
                when start_q = '1' else
                  '1';
  ram_addr_o <= std_logic_vector(addr_q(15 downto 0));
  ram_data_b <= ser_dat_q;
  ram_oe_no  <= '1';
  ram_ce_no  <= ram_ce_n_q;
  ram_we_no  <= ram_we_n_q;

end rtl;

-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: spi_boot.vhd,v 1.9 2007/02/25 18:24:12 arniml Exp $
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


entity spi_boot is

  generic (
    -- width of set selection
    width_set_sel_g      : integer := 4;
    -- width of bit counter: minimum 6, maximum 12
    width_bit_cnt_g      : integer := 6;
    -- width of image counter: minimum 0, maximum n
    width_img_cnt_g      : integer := 2;
    -- number of bits required to address one image
    num_bits_per_img_g   : integer := 18;
    -- SD specific initialization
    sd_init_g            : integer := 0;
    -- clock divider to reach 400 kHz for MMC compatibility
    mmc_compat_clk_div_g : integer := 0;
    width_mmc_clk_div_g  : integer := 0;
    -- active level of reset_i
    reset_level_g        : integer := 0
  );

  port (
    -- System Interface -------------------------------------------------------
    clk_i          : in  std_logic;
    reset_i        : in  std_logic;
    set_sel_i      : in  std_logic_vector(width_set_sel_g-1 downto 0);
    -- Card Interface ---------------------------------------------------------
    spi_clk_o      : out std_logic;
    spi_cs_n_o     : out std_logic;
    spi_data_in_i  : in  std_logic;
    spi_data_out_o : out std_logic;
    spi_en_outs_o  : out std_logic;
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

end spi_boot;


library ieee;
use ieee.numeric_std.all;
use work.spi_boot_pack.all;

architecture rtl of spi_boot is

  component spi_counter
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
  end component;


  -----------------------------------------------------------------------------
  -- States of the controller FSM
  --
  type ctrl_states_t is (POWER_UP1, POWER_UP2,
                         CMD0,
                         CMD1,
                         CMD55, ACMD41,
                         CMD16,
                         WAIT_START,
                         WAIT_INIT_LOW, WAIT_INIT_HIGH,
                         CMD18, CMD18_DATA,
                         CMD12,
                         INC_IMG_CNT);
  --
  signal ctrl_fsm_q,
         ctrl_fsm_s  : ctrl_states_t;
  --
  -----------------------------------------------------------------------------

  -----------------------------------------------------------------------------
  -- States of the command FSM
  --
  type cmd_states_t is (CMD, START, R1, PAUSE);
  --
  signal cmd_fsm_q,
         cmd_fsm_s  : cmd_states_t;
  --
  -----------------------------------------------------------------------------

  subtype op_r     is integer range 5 downto 0;
  type    res_bc_t is (NONE, RES_MAX, RES_47, RES_15, RES_7);
  signal  bit_cnt_q  : unsigned(width_bit_cnt_g-1 downto 0);
  signal  res_bc_s   : res_bc_t;
  signal  upper_bitcnt_zero_s : boolean;

  signal cfg_dat_q : std_logic;

  signal spi_clk_q         : std_logic;
  signal spi_clk_rising_q  : boolean;
  signal spi_clk_falling_q : boolean;
  signal spi_dat_q,
         spi_dat_s         : std_logic;
  signal spi_cs_n_q,
         spi_cs_n_s        : std_logic;

  signal cfg_clk_q      : std_logic;

  signal start_q        : std_logic;

  signal img_cnt_s      : std_logic_vector(width_img_cnt_g downto 0);
  signal cnt_en_img_s   : boolean;
  signal mmc_cnt_ovfl_s : boolean;
  signal mmc_compat_s   : boolean;

  signal cmd_finished_s : boolean;

  signal r1_result_q    : std_logic;
  signal done_q,
         send_cmd12_q   : boolean;

  signal en_outs_s,
         en_outs_q      : boolean;

  signal reset_s        : boolean;

  signal true_s         : boolean;

begin

  true_s <= true;

  reset_s <=   true
             when (reset_level_g = 1 and reset_i = '1') or
                  (reset_level_g = 0 and reset_i = '0') else
               false;

  -----------------------------------------------------------------------------
  -- Process seq
  --
  -- Purpose:
  --   Implements several sequential elements.
  --
  seq: process (clk_i, reset_s)

    variable bit_cnt_v : unsigned(1 downto 0);

  begin
    if reset_s then
      -- reset bit counter to 63 for power up
      bit_cnt_q       <= (others => '0');
      bit_cnt_q(op_r) <= "111111";
      spi_dat_q    <= '1';
      spi_cs_n_q   <= '1';
      cfg_dat_q    <= '1';
      start_q      <= '0';
      done_q       <= false;
      send_cmd12_q <= false;
      ctrl_fsm_q   <= POWER_UP1;
      cmd_fsm_q    <= CMD;
      r1_result_q  <= '0';
      en_outs_q    <= false;

    elsif clk_i'event and clk_i = '1' then
      -- bit counter control
      if spi_clk_rising_q then
        case res_bc_s is
          when NONE =>
            bit_cnt_q       <= bit_cnt_q - 1;
          when RES_MAX =>
            bit_cnt_q       <= (others => '1');
          when RES_47 =>
            bit_cnt_q       <= (others => '0');
            bit_cnt_q(op_r) <= "101111";
          when RES_15 =>
            bit_cnt_q       <= (others => '0');
            bit_cnt_q(op_r) <= "001111";
          when RES_7 =>
            bit_cnt_q       <= (others => '0');
            bit_cnt_q(op_r) <= "000111";
          when others =>
            bit_cnt_q       <= (others => '0');
        end case;
      end if;

      -- Card data output register
      -- spi_clk_falling_q acts as enable during MMC clock compatibility mode.
      -- As soon as this mode is left, the register must start latching.
      -- There is no explicit relation to spi_clk_q anymore in normal mode.
      -- Instead, spi_dat_s is operated by bit_cnt_q above which changes its
      -- value after the rising edge of spi_clk_q.
      --   -> spi_dat_q changes upon falling edge of spi_clk_q
      if spi_clk_falling_q or not mmc_compat_s then
        spi_dat_q <= spi_dat_s;
      end if;

      -- config data output register
      -- a new value is loaded when config clock is high,
      -- i.e. input data is sampled with rising spi_clk
      -- while output value changes on falling edge of cfg_clk
      if cfg_clk_q = '1' and spi_clk_rising_q then
        cfg_dat_q <= spi_data_in_i;
      end if;

      -- Controller FSM state
      ctrl_fsm_q <= ctrl_fsm_s;

      -- Command FSM state
      cmd_fsm_q <= cmd_fsm_s;

      -- CS signal for SPI card
      if spi_clk_q = '1' then
        spi_cs_n_q <= spi_cs_n_s;
      end if;

      -- Extract flags from R1 response
      if cmd_fsm_q = R1 then
        bit_cnt_v := bit_cnt_q(1 downto 0);
        case bit_cnt_v(1 downto 0) is
          when "10" =>
            -- always save "Illegal Command" flag
            r1_result_q <= to_X01(spi_data_in_i);
          when "00" =>
            -- overwrite with "Idle State" flag when not in CMD55
            if ctrl_fsm_q /= CMD55 then
              r1_result_q <= to_X01(spi_data_in_i);
            end if;
          when others =>
            null;
        end case;
      end if;

      -- Start trigger register for rising edge detection
      -- the reset value is '0' thus a rising edge will always be detected
      -- after reset even though start_i is tied to '1'
      if start_i = '0' then
        start_q <= '0';
      elsif ctrl_fsm_q = WAIT_START and cmd_finished_s then
        start_q <= start_i;
      end if;

      -- Marker for cfg_done and dat_done
      if ctrl_fsm_q = CMD18_DATA then
        if cfg_done_i = '1' and dat_done_i = '1' then
          done_q       <= true;
        end if;

        if done_q and
           (not upper_bitcnt_zero_s or cmd_fsm_q = START) then
          -- activate sending of CMD12 when it is safe:
          -- * upper bits of bit counter are not zero
          --   -> transmission of CMD12 is not running
          -- * cmd FSM is in START state
          --   -> also no transmission running
          send_cmd12_q <= true;
        end if;
      elsif ctrl_fsm_q = WAIT_START then
        -- reset done_q when WAIT_START has been reached
        -- this is necessary to let the stop transmission process come to
        -- an end without interruption or generation of unwanted cfg_clk_q
        done_q         <= false;
        send_cmd12_q   <= false;
      end if;

      -- output enable
      if spi_clk_rising_q then
        en_outs_q <= en_outs_s;
      end if;

    end if;

  end process seq;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process upper_bits
  --
  -- Purpose:
  --   Detects that the upper bits of the bit counter are zero.
  --   Upper bits = n downto 6, i.e. the optional part that is not required for
  --   commands but for extension of data blocks.
  --
  upper_bits: process (bit_cnt_q)
    variable zero_v : boolean;
  begin

    zero_v     := true;
    for i in bit_cnt_q'high downto 6 loop
      if bit_cnt_q(i) = '1' then
        zero_v := false;
      end if;
    end loop;

    upper_bitcnt_zero_s <= zero_v;

  end process upper_bits;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process clk_gen
  --
  -- Purpose:
  --   Generates clocks for card and FPGA configuration.
  --   The card clock is free running with a divide by two of clk_i.
  --   The clock for FPGA config has an enable and is stopped on high level.
  --   There is a phase shift of half a period between spi_clk and cfg_clk.
  --
  clk_gen: process (clk_i, reset_s)
  begin
    if reset_s then
      spi_clk_q        <= '0';
      cfg_clk_q        <= '1';

    elsif clk_i'event and clk_i = '1' then

      -- spi_clk_q rises according to the flag
      -- it falls with overflow indication
      -- the resulting duty cycle is not exactly 50:50,
      -- high time is a bit longer
      if mmc_compat_s then
        -- MMC clock compatibility mode:
        -- spi_clk_q rises when flagged by spi_clk_rising_q
        if spi_clk_rising_q then
          spi_clk_q <= '1';
        elsif mmc_cnt_ovfl_s then
          -- upon counter overflow spi_clk_q falls in case it does not rise
          spi_clk_q <= '0';
        end if;
      else
        -- normal mode
        -- spi_clk_q follows spi_clk_rising_q
        if spi_clk_rising_q then
          spi_clk_q <= '1';
        else
          spi_clk_q <= '0';
        end if;
      end if;

      -- clock for FPGA config must be enabled and follows spi_clk
      if ctrl_fsm_q = CMD18_DATA and cmd_fsm_q = CMD and
         not done_q then
        cfg_clk_q <= spi_clk_q;
      else
        cfg_clk_q <= '1';
      end if;

    end if;

  end process clk_gen;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Indication flags for rising and falling spi_clk_q.
  -- Essential for MMC clock compatibility mode.
  -----------------------------------------------------------------------------
  mmc_comap: if mmc_compat_clk_div_g > 0 generate
    mmc_compat_sig: process (clk_i, reset_s)
    begin
      if reset_s then
        spi_clk_rising_q  <= false;
        spi_clk_falling_q <= false;

      elsif clk_i'event and clk_i = '1' then
        if mmc_compat_s then
          -- MMC clock compatibility mode:
          -- spi_clk_rising_q is an impulse right before rising edge of spi_clk_q
          -- spi_clk_falling_q is an impulse right before falling edge of spi_clk_q
          if mmc_cnt_ovfl_s then
            spi_clk_rising_q  <= spi_clk_q = '0';
            spi_clk_falling_q <= spi_clk_q = '1';
          else
            spi_clk_rising_q  <= false;
            spi_clk_falling_q <= false;
          end if;
        else
          -- normal mode
          spi_clk_rising_q  <= not spi_clk_rising_q;
          spi_clk_falling_q <= true;
        end if;

      end if;
    end process mmc_compat_sig;
  end generate;

  no_mmc_compat: if mmc_compat_clk_div_g = 0 generate
    -- SPI clock rising whenever spi_clk_q is '0'
    spi_clk_rising_q  <= spi_clk_q = '0';
    -- SPI clock falling whenever spi_clk_q is '1'
    spi_clk_falling_q <= spi_clk_q = '1';
  end generate;


  -----------------------------------------------------------------------------
  -- Process ctrl_fsm
  --
  -- Purpose:
  --   Implements the controller FSM.
  --
  ctrl_fsm: process (ctrl_fsm_q,
                     cmd_finished_s, r1_result_q,
                     start_i, start_q, mode_i,
                     cfg_init_n_i)

    variable mmc_compat_v : boolean;

  begin
    -- default assignments
    ctrl_fsm_s   <= POWER_UP1;
    config_n_o   <= '1';
    cnt_en_img_s <= false;
    spi_cs_n_s   <= '0';
    mmc_compat_v := false;
    en_outs_s    <= true;

    case ctrl_fsm_q is
      -- Let card finish power up, step 1 -------------------------------------
      when POWER_UP1 =>
        mmc_compat_v := true;
        spi_cs_n_s   <= '1';
        if cmd_finished_s then
          ctrl_fsm_s <= POWER_UP2;
        else
          ctrl_fsm_s <= POWER_UP1;
        end if;


      -- Let card finish power up, step 2 -------------------------------------
      when POWER_UP2 =>
        mmc_compat_v := true;
        if cmd_finished_s then
          ctrl_fsm_s <= CMD0;
        else
          spi_cs_n_s <= '1';
          ctrl_fsm_s <= POWER_UP2;
        end if;


      -- Issue CMD0: GO_IDLE_STATE --------------------------------------------
      when CMD0 =>
        mmc_compat_v   := true;
        if cmd_finished_s then
          if sd_init_g = 1 then
            ctrl_fsm_s <= CMD55;
          else
            ctrl_fsm_s <= CMD1;
          end if;
        else
          ctrl_fsm_s   <= CMD0;
        end if;


      -- Issue CMD55: APP_CMD -------------------------------------------------
      when CMD55 =>
        if sd_init_g = 1 then

          mmc_compat_v   := true;
          if cmd_finished_s then
            if r1_result_q = '0' then
              -- command accepted, it's an SD card
              ctrl_fsm_s <= ACMD41;
            else
              -- command rejected, it's an MMC card
              ctrl_fsm_s <= CMD1;
            end if;
          else
            ctrl_fsm_s   <= CMD55;
          end if;

        end if;


      -- Issue ACMD41: SEND_OP_COND -------------------------------------------
      when ACMD41 =>
        if sd_init_g = 1 then

          mmc_compat_v   := true;
          if cmd_finished_s then
            if r1_result_q = '0' then
              ctrl_fsm_s <= CMD16;
            else
              ctrl_fsm_s <= CMD55;
            end if;
          else
            ctrl_fsm_s   <= ACMD41;
          end if;

        end if;


      -- Issue CMD1: SEND_OP_COND ---------------------------------------------
      when CMD1 =>
        mmc_compat_v   := true;
        if cmd_finished_s then
          if r1_result_q = '0' then
            ctrl_fsm_s <= CMD16;
          else
            ctrl_fsm_s <= CMD1;
          end if;
        else
          ctrl_fsm_s   <= CMD1;
        end if;


      -- Issue CMD16: SET_BLOCKLEN --------------------------------------------
      when CMD16 =>
        if cmd_finished_s then
          ctrl_fsm_s <= WAIT_START;
        else
          ctrl_fsm_s <= CMD16;
        end if;


      -- Wait for configuration start request ---------------------------------
      when WAIT_START =>
        spi_cs_n_s     <= '1';

        -- detect rising edge of start_i
        if start_i = '1' and start_q = '0' then
          -- decide which mode is requested
          if cmd_finished_s then
            if mode_i = '0' then
              ctrl_fsm_s <= CMD18;
            else
              ctrl_fsm_s <= WAIT_INIT_LOW;
            end if;
          else
            en_outs_s    <= false;
            ctrl_fsm_s   <= WAIT_START;
          end if;
        else
          en_outs_s      <= false;
          ctrl_fsm_s     <= WAIT_START;
        end if;


      -- Wait for INIT to become low ------------------------------------------
      when WAIT_INIT_LOW =>
        spi_cs_n_s   <= '1';
        -- activate FPGA configuration
        config_n_o   <= '0';

        if cfg_init_n_i = '0' then
          ctrl_fsm_s <= WAIT_INIT_HIGH;
        else
          ctrl_fsm_s <= WAIT_INIT_LOW;
        end if;


      -- Wait for INIT to become high -----------------------------------------
      when WAIT_INIT_HIGH =>
        spi_cs_n_s   <= '1';

        if cfg_init_n_i = '1' and cmd_finished_s then
          ctrl_fsm_s <= CMD18;
        else
          ctrl_fsm_s <= WAIT_INIT_HIGH;
        end if;


      -- Issue CMD18: READ_MULTIPLE_BLOCKS ------------------------------------
      when CMD18 =>
        if cmd_finished_s then
          ctrl_fsm_s <= CMD18_DATA;
        else
          ctrl_fsm_s <= CMD18;
        end if;
      --
      -- receive a data block
      when CMD18_DATA =>
        if cmd_finished_s then
          ctrl_fsm_s   <= CMD12;
        else
          ctrl_fsm_s   <= CMD18_DATA;
        end if;


       -- Issued CMD12: STOP_TRANSMISSION --------------------------------------
       when CMD12 =>
         if cmd_finished_s then
           ctrl_fsm_s <= INC_IMG_CNT;
         else
           ctrl_fsm_s <= CMD12;
         end if;


      -- Increment Image Counter ----------------------------------------------
      when INC_IMG_CNT =>
        spi_cs_n_s   <= '1';
        ctrl_fsm_s   <= WAIT_START;
        cnt_en_img_s <= true;



      when others =>
        null;

    end case;

    -- mmc_compat_s is suppressed if MMC clock compatibility is not required
    if mmc_compat_clk_div_g > 0 then
      mmc_compat_s <= mmc_compat_v;
    else
      mmc_compat_s <= false;
    end if;

  end process ctrl_fsm;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process cmd_fsm
  --
  -- Purpose:
  --   Implements the command FSM.
  --
  cmd_fsm: process (spi_clk_rising_q,
                    spi_data_in_i,
                    bit_cnt_q,
                    ctrl_fsm_q,
                    cmd_fsm_q,
                    send_cmd12_q)

    variable cnt_zero_v     : boolean;
    variable spi_data_low_v : boolean;
    variable no_startbit_v  : boolean;

  begin
    -- default assignments
    cmd_finished_s <= false;
    cmd_fsm_s      <= CMD;
    res_bc_s       <= NONE;

    cnt_zero_v     := spi_clk_rising_q and bit_cnt_q = 0;
    spi_data_low_v := spi_clk_rising_q and spi_data_in_i = '0';

    -- these are no real commands thus there will be no startbit
    case ctrl_fsm_q is
      when POWER_UP1  | POWER_UP2 |
           WAIT_START | WAIT_INIT_HIGH | WAIT_INIT_LOW =>
        no_startbit_v := true;
      when others =>
        no_startbit_v := false;
    end case;


    case cmd_fsm_q is
      -- Send the command -----------------------------------------------------
      when CMD =>
        if cnt_zero_v then
          if ctrl_fsm_q /= CMD18_DATA then
            -- normal commands including CMD12 require startbit of R1 response
            cmd_fsm_s <= START;
          else
            if not send_cmd12_q then
              -- CMD18_DATA needs to read CRC
              cmd_fsm_s <= R1;
              res_bc_s <= RES_15;
            else
              -- CMD18_DATA finished, scan for startbit of response
              cmd_finished_s <= true;
              cmd_fsm_s      <= START;
            end if;
          end if;
        else
          cmd_fsm_s <= CMD;
        end if;

      -- Wait for startbit of response ----------------------------------------
      when START =>
        -- startbit detection or skip of this check
        if no_startbit_v and spi_clk_rising_q then
          cmd_fsm_s   <= R1;
          res_bc_s    <= RES_7;
        elsif spi_data_low_v then
          if ctrl_fsm_q /= CMD18_DATA then
            cmd_fsm_s <= R1;
          else
            -- CMD18_DATA startbit detected, read payload
            cmd_fsm_s <= CMD;
            res_bc_s  <= RES_MAX;
          end if;
        else
          cmd_fsm_s   <= START;
          res_bc_s    <= RES_7;
        end if;

      -- Read R1 response -----------------------------------------------------
      when R1 =>
        if cnt_zero_v then
          res_bc_s  <= RES_7;

          if not (ctrl_fsm_q = CMD18 or ctrl_fsm_q = CMD18_DATA) then
            cmd_fsm_s        <= PAUSE;
          else
            -- CMD18 needs another startbit detection for the data token.
            -- CMD18_DATA needs a startbit after having received the CRC, either
            --   * next data token
            --   * R1 response of CMD12
            cmd_fsm_s        <= START;

            if ctrl_fsm_q = CMD18 then
              -- CMD18 response received -> advance to CMD18_DATA
              cmd_finished_s <= true;
            end if;
          end if;
        else
          cmd_fsm_s <= R1;
        end if;

      -- PAUSE state -> required for Nrc, card response to host command -------
      when PAUSE =>
        if cnt_zero_v then
          cmd_fsm_s <= CMD;
          res_bc_s  <= RES_47;
          cmd_finished_s <= true;
        else
          cmd_fsm_s <= PAUSE;
        end if;

      when others =>
        null;

    end case;

  end process cmd_fsm;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Process transmit
  --
  -- Purpose:
  --   Generates the serial data output values based on the current FSM state
  --
  --   The local variable cmd_v is 64 bits wide in contrast to an SPI command
  --   with 48 bits. There are two reasons for this:
  --   * During "overlaid" sending of CMD12 in FSM state CMD18_DATA, the bit
  --     counter will start from 3F on its lowest 6 bits. Therefore, it is
  --     necessary to provide all 64 positions in cmd_v.
  --   * Reduces logic.
  --
  transmit: process (ctrl_fsm_q,
                     cmd_fsm_q,
                     bit_cnt_q,
                     img_cnt_s,
                     send_cmd12_q,
                     set_sel_i,
                     upper_bitcnt_zero_s)

    subtype cmd_r is natural range 47 downto 0;
    subtype cmd_t is std_logic_vector(cmd_r);
    subtype ext_cmd_t is std_logic_vector(63 downto 0);
    --                            STCCCCCCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcccccccS
    constant cmd0_c   : cmd_t := "010000000000000000000000000000000000000010010101";
    constant cmd1_c   : cmd_t := "0100000100000000000000000000000000000000-------1";
    constant cmd12_c  : cmd_t := "0100110000000000000000000000000000000000-------1";
    constant cmd16_c  : cmd_t := "0101000000000000000000000000000000000000-------1";
    constant cmd18_c  : cmd_t := "0101001000000000000000000000000000000000-------1";
    constant cmd55_c  : cmd_t := "0111011100000000000000000000000000000000-------1";
    constant acmd41_c : cmd_t := "0110100100000000000000000000000000000000-------1";

    variable cmd_v      : ext_cmd_t;
    variable tx_v       : boolean;

  begin
    -- default assignments
    spi_dat_s    <= '1';
    cmd_v        := (others => '1');
    tx_v         := false;

    if cmd_fsm_q = CMD then
      case ctrl_fsm_q is
        when CMD0 =>
          cmd_v(cmd_r) := cmd0_c;
          tx_v := true;
        when CMD1 =>
          cmd_v(cmd_r) := cmd1_c;
          tx_v := true;
        when CMD16 =>
          cmd_v(cmd_r) := cmd16_c;
          cmd_v(8 + width_bit_cnt_g-3) := '1';
          tx_v := true;
        when CMD18 =>
          cmd_v(cmd_r) := cmd18_c;
          -- insert image counter
          cmd_v(8 + num_bits_per_img_g + width_img_cnt_g
                downto 8 + num_bits_per_img_g) := img_cnt_s;
          -- insert set selection
          cmd_v(8 + num_bits_per_img_g + width_img_cnt_g + width_set_sel_g-1
                downto 8 + num_bits_per_img_g + width_img_cnt_g) := set_sel_i;
          tx_v := true;
        when CMD18_DATA =>
          cmd_v(cmd_r) := cmd12_c;

          if send_cmd12_q and upper_bitcnt_zero_s then
            tx_v := true;
          end if;
        when CMD55 =>
          cmd_v(cmd_r) := cmd55_c;
          tx_v := true;
        when ACMD41 =>
          cmd_v(cmd_r) := acmd41_c;
          tx_v := true;

        when others =>
          null;
      end case;
    end if;

    if tx_v then
      spi_dat_s <= cmd_v(to_integer(bit_cnt_q(5 downto 0)));
    end if;

  end process transmit;
  --
  -----------------------------------------------------------------------------


  -----------------------------------------------------------------------------
  -- Optional Image Counter
  -----------------------------------------------------------------------------
  img_cnt: if width_img_cnt_g > 0 generate
    img_cnt_b : spi_counter
      generic map (
        cnt_width_g   => width_img_cnt_g,
        cnt_max_g     => 2**width_img_cnt_g - 1
      )
      port map (
        clk_i         => clk_i,
        reset_i       => reset_s,
        cnt_en_i      => cnt_en_img_s,
        cnt_o         => img_cnt_s(width_img_cnt_g-1 downto 0),
        cnt_ovfl_o    => open
      );
    img_cnt_s(width_img_cnt_g) <= '0';
  end generate;

  no_img_cnt: if width_img_cnt_g = 0 generate
    img_cnt_s <= (others => '0');
  end generate;


  -----------------------------------------------------------------------------
  -- Optional MMC compatibility counter
  -----------------------------------------------------------------------------
  mmc_cnt: if mmc_compat_clk_div_g > 0 generate
    mmc_cnt_b : spi_counter
      generic map (
        cnt_width_g   => width_mmc_clk_div_g,
        cnt_max_g     => mmc_compat_clk_div_g
      )
      port map (
        clk_i         => clk_i,
        reset_i       => reset_s,
        cnt_en_i      => true_s,
        cnt_o         => open,
        cnt_ovfl_o    => mmc_cnt_ovfl_s
      );
  end generate;

  no_mmc_cnt: if mmc_compat_clk_div_g = 0 generate
    mmc_cnt_ovfl_s <= true;
  end generate;


  -----------------------------------------------------------------------------
  -- Output Mapping
  -----------------------------------------------------------------------------
  spi_clk_o      <=   spi_clk_q;
  spi_cs_n_o     <=   spi_cs_n_q;
  spi_data_out_o <=   spi_dat_q;
  spi_en_outs_o  <=   '1'
                    when en_outs_q else
                      '0';
  cfg_clk_o      <= cfg_clk_q;
  cfg_dat_o      <= cfg_dat_q;
  detached_o     <=   '0'
                    when en_outs_q else
                      '1';

end rtl;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: spi_boot.vhd,v $
-- Revision 1.9  2007/02/25 18:24:12  arniml
-- fix type handling of resets
--
-- Revision 1.8  2006/09/11 23:03:36  arniml
-- disable outputs with reset
--
-- Revision 1.7  2005/04/07 20:44:23  arniml
-- add new port detached_o
--
-- Revision 1.6  2005/03/09 19:48:34  arniml
-- invert level of set_sel input
--
-- Revision 1.5  2005/03/08 22:07:12  arniml
-- added set selection
--
-- Revision 1.4  2005/02/18 06:42:08  arniml
-- clarify wording for images
--
-- Revision 1.3  2005/02/16 18:59:10  arniml
-- include output enable control for SPI outputs
--
-- Revision 1.2  2005/02/13 17:25:51  arniml
-- major update to fix several problems
-- configuration/data download of multiple sets works now
--
-- Revision 1.1  2005/02/08 20:41:33  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

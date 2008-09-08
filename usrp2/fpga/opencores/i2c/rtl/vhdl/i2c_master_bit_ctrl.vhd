---------------------------------------------------------------------
----                                                             ----
----  WISHBONE revB2 I2C Master Core; bit-controller             ----
----                                                             ----
----                                                             ----
----  Author: Richard Herveille                                  ----
----          richard@asics.ws                                   ----
----          www.asics.ws                                       ----
----                                                             ----
----  Downloaded from: http://www.opencores.org/projects/i2c/    ----
----                                                             ----
---------------------------------------------------------------------
----                                                             ----
---- Copyright (C) 2000 Richard Herveille                        ----
----                    richard@asics.ws                         ----
----                                                             ----
---- This source file may be used and distributed without        ----
---- restriction provided that this copyright statement is not   ----
---- removed from the file and that any derivative work contains ----
---- the original copyright notice and the associated disclaimer.----
----                                                             ----
----     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ----
---- EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ----
---- TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ----
---- FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ----
---- OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ----
---- INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ----
---- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ----
---- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ----
---- BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ----
---- LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ----
---- (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ----
---- OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ----
---- POSSIBILITY OF SUCH DAMAGE.                                 ----
----                                                             ----
---------------------------------------------------------------------

--  CVS Log
--
--  $Id: i2c_master_bit_ctrl.vhd,v 1.14 2006/10/11 12:10:13 rherveille Exp $
--
--  $Date: 2006/10/11 12:10:13 $
--  $Revision: 1.14 $
--  $Author: rherveille $
--  $Locker:  $
--  $State: Exp $
--
-- Change History:
--               $Log: i2c_master_bit_ctrl.vhd,v $
--               Revision 1.14  2006/10/11 12:10:13  rherveille
--               Added missing semicolons ';' on endif
--
--               Revision 1.13  2006/10/06 10:48:24  rherveille
--               fixed short scl high pulse after clock stretch
--
--               Revision 1.12  2004/05/07 11:53:31  rherveille
--               Fixed previous fix :) Made a variable vs signal mistake.
--
--               Revision 1.11  2004/05/07 11:04:00  rherveille
--               Fixed a bug where the core would signal an arbitration lost (AL bit set), when another master controls the bus and the other master generates a STOP bit.
--
--               Revision 1.10  2004/02/27 07:49:43  rherveille
--               Fixed a bug in the arbitration-lost signal generation. VHDL version only.
--
--               Revision 1.9  2003/08/12 14:48:37  rherveille
--               Forgot an 'end if' :-/
--
--               Revision 1.8  2003/08/09 07:01:13  rherveille
--               Fixed a bug in the Arbitration Lost generation caused by delay on the (external) sda line.
--               Fixed a potential bug in the byte controller's host-acknowledge generation.
--
--               Revision 1.7  2003/02/05 00:06:02  rherveille
--               Fixed a bug where the core would trigger an erroneous 'arbitration lost' interrupt after being reset, when the reset pulse width < 3 clk cycles.
--
--               Revision 1.6  2003/02/01 02:03:06  rherveille
--               Fixed a few 'arbitration lost' bugs. VHDL version only.
--
--               Revision 1.5  2002/12/26 16:05:47  rherveille
--               Core is now a Multimaster I2C controller.
--
--               Revision 1.4  2002/11/30 22:24:37  rherveille
--               Cleaned up code
--
--               Revision 1.3  2002/10/30 18:09:53  rherveille
--               Fixed some reported minor start/stop generation timing issuess.
--
--               Revision 1.2  2002/06/15 07:37:04  rherveille
--               Fixed a small timing bug in the bit controller.\nAdded verilog simulation environment.
--
--               Revision 1.1  2001/11/05 12:02:33  rherveille
--               Split i2c_master_core.vhd into separate files for each entity; same layout as verilog version.
--               Code updated, is now up-to-date to doc. rev.0.4.
--               Added headers.
--


--
-------------------------------------
-- Bit controller section
------------------------------------
--
-- Translate simple commands into SCL/SDA transitions
-- Each command has 5 states, A/B/C/D/idle
--
-- start:    SCL  ~~~~~~~~~~~~~~\____
--	     SDA  XX/~~~~~~~\______
--	          x | A | B | C | D | i
--
-- repstart  SCL  ______/~~~~~~~\___
--	     SDA  __/~~~~~~~\______
--	          x | A | B | C | D | i
--
-- stop      SCL  _______/~~~~~~~~~~~
--	     SDA  ==\___________/~~~~~
--	          x | A | B | C | D | i
--
--- write    SCL  ______/~~~~~~~\____
--	     SDA  XXX===============XX
--	          x | A | B | C | D | i
--
--- read     SCL  ______/~~~~~~~\____
--	     SDA  XXXXXXX=XXXXXXXXXXX
--	          x | A | B | C | D | i
--

-- Timing:      Normal mode     Fast mode
-----------------------------------------------------------------
-- Fscl         100KHz          400KHz
-- Th_scl       4.0us           0.6us   High period of SCL
-- Tl_scl       4.7us           1.3us   Low period of SCL
-- Tsu:sta      4.7us           0.6us   setup time for a repeated start condition
-- Tsu:sto      4.0us           0.6us   setup time for a stop conditon
-- Tbuf         4.7us           1.3us   Bus free time between a stop and start condition
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity i2c_master_bit_ctrl is
	port (
		clk    : in std_logic;
		rst    : in std_logic;
		nReset : in std_logic;
		ena    : in std_logic;				-- core enable signal

		clk_cnt : in unsigned(15 downto 0);		-- clock prescale value

		cmd     : in std_logic_vector(3 downto 0);
		cmd_ack : out std_logic; -- command completed
		busy    : out std_logic; -- i2c bus busy
		al      : out std_logic; -- arbitration lost

		din  : in std_logic;
		dout : out std_logic;

		-- i2c lines
		scl_i   : in std_logic;  -- i2c clock line input
		scl_o   : out std_logic; -- i2c clock line output
		scl_oen : out std_logic; -- i2c clock line output enable, active low
		sda_i   : in std_logic;  -- i2c data line input
		sda_o   : out std_logic; -- i2c data line output
		sda_oen : out std_logic  -- i2c data line output enable, active low
	);
end entity i2c_master_bit_ctrl;

architecture structural of i2c_master_bit_ctrl is
	constant I2C_CMD_NOP    : std_logic_vector(3 downto 0) := "0000";
	constant I2C_CMD_START  : std_logic_vector(3 downto 0) := "0001";
	constant I2C_CMD_STOP   : std_logic_vector(3 downto 0) := "0010";
	constant I2C_CMD_READ   : std_logic_vector(3 downto 0) := "0100";
	constant I2C_CMD_WRITE  : std_logic_vector(3 downto 0) := "1000";

	type states is (idle, start_a, start_b, start_c, start_d, start_e,
	                stop_a, stop_b, stop_c, stop_d, rd_a, rd_b, rd_c, rd_d, wr_a, wr_b, wr_c, wr_d);
	signal c_state : states;

	signal iscl_oen, isda_oen : std_logic;          -- internal I2C lines
	signal sda_chk            : std_logic;          -- check SDA status (multi-master arbitration)
	signal dscl_oen           : std_logic;          -- delayed scl_oen signals
	signal sSCL, sSDA         : std_logic;          -- synchronized SCL and SDA inputs
	signal clk_en, slave_wait : std_logic;          -- clock generation signals
	signal ial                : std_logic;          -- internal arbitration lost signal
--	signal cnt : unsigned(15 downto 0) := clk_cnt;  -- clock divider counter (simulation)
	signal cnt : unsigned(15 downto 0);             -- clock divider counter (synthesis)

begin
	-- whenever the slave is not ready it can delay the cycle by pulling SCL low
	-- delay scl_oen
	process (clk)
	begin
	    if (clk'event and clk = '1') then
	      dscl_oen <= iscl_oen;
	    end if;
	end process;
	slave_wait <= dscl_oen and not sSCL;

	-- generate clk enable signal
	gen_clken: process(clk, nReset)
	begin
	    if (nReset = '0') then
	      cnt    <= (others => '0');
	      clk_en <= '1';
	    elsif (clk'event and clk = '1') then
	      if (rst = '1') then
	        cnt    <= (others => '0');
	        clk_en <= '1';
	      elsif ( (cnt = 0) or (ena = '0') ) then
	        cnt    <= clk_cnt;
	        clk_en <= '1';
	      elsif (slave_wait = '1') then
	        cnt    <= cnt;
	        clk_en <= '0';
	      else
	        cnt    <= cnt -1;
	        clk_en <= '0';
	      end if;
	    end if;
	end process gen_clken;


	-- generate bus status controller
	bus_status_ctrl: block
	  signal dSCL, dSDA          : std_logic;  -- delayes sSCL and sSDA
	  signal sta_condition       : std_logic;  -- start detected
	  signal sto_condition       : std_logic;  -- stop detected
	  signal cmd_stop            : std_logic;  -- STOP command
	  signal ibusy               : std_logic;  -- internal busy signal
	begin
	    -- synchronize SCL and SDA inputs
	    synch_scl_sda: process(clk, nReset)
	    begin
	        if (nReset = '0') then
	          sSCL <= '1';
	          sSDA <= '1';

	          dSCL <= '1';
	          dSDA <= '1';
	        elsif (clk'event and clk = '1') then
	          if (rst = '1') then
	            sSCL <= '1';
	            sSDA <= '1';

	            dSCL <= '1';
	            dSDA <= '1';
	          else
	            sSCL <= scl_i;
	            sSDA <= sda_i;

	            dSCL <= sSCL;
	            dSDA <= sSDA;
	          end if;
	        end if;
	    end process synch_SCL_SDA;

	    -- detect start condition => detect falling edge on SDA while SCL is high
	    -- detect stop condition  => detect rising edge on SDA while SCL is high
	    detect_sta_sto: process(clk, nReset)
	    begin
	        if (nReset = '0') then
	          sta_condition <= '0';
	          sto_condition <= '0';
	        elsif (clk'event and clk = '1') then
	          if (rst = '1') then
	            sta_condition <= '0';
	            sto_condition <= '0';
	          else
	            sta_condition <= (not sSDA and dSDA) and sSCL;
	            sto_condition <= (sSDA and not dSDA) and sSCL;
	          end if;
	        end if;
	    end process detect_sta_sto;

	    -- generate i2c-bus busy signal
	    gen_busy: process(clk, nReset)
	    begin
	        if (nReset = '0') then
	          ibusy <= '0';
	        elsif (clk'event and clk = '1') then
	          if (rst = '1') then
	            ibusy <= '0';
	          else
	            ibusy <= (sta_condition or ibusy) and not sto_condition;
	          end if;
	        end if;
	    end process gen_busy;
	    busy <= ibusy;


	    -- generate arbitration lost signal
	    -- aribitration lost when:
	    -- 1) master drives SDA high, but the i2c bus is low
	    -- 2) stop detected while not requested (detect during 'idle' state)
	    gen_al: process(clk, nReset)
	    begin
	      if (nReset = '0') then
	        cmd_stop  <= '0';
	        ial       <= '0';
	      elsif (clk'event and clk = '1') then
	        if (rst = '1') then
	          cmd_stop  <= '0';
	          ial       <= '0';
	        else
	          if (clk_en = '1') then
	            if (cmd = I2C_CMD_STOP) then
	              cmd_stop <= '1';
	            else
	              cmd_stop <= '0';
	            end if;
	          end if;

	          if (c_state = idle) then
	            ial <= (sda_chk and not sSDA and isda_oen);
	          else
	            ial <= (sda_chk and not sSDA and isda_oen) or (sto_condition and not cmd_stop);
	          end if;

	        end if;
	      end if;
	    end process gen_al;
	    al <= ial;

	    -- generate dout signal, store dout on rising edge of SCL
	    gen_dout: process(clk)
	    begin
	      if (clk'event and clk = '1') then
	        if (sSCL = '1' and dSCL = '0') then
	          dout <= sSDA;
	        end if;
	      end if;
	    end process gen_dout;
	end block bus_status_ctrl;


	-- generate statemachine
	nxt_state_decoder : process (clk, nReset, c_state, cmd)
	begin
	    if (nReset = '0') then
	      c_state  <= idle;
	      cmd_ack  <= '0';
	      iscl_oen <= '1';
	      isda_oen <= '1';
	      sda_chk  <= '0';
	    elsif (clk'event and clk = '1') then
	      if (rst = '1' or ial = '1') then
	        c_state  <= idle;
	        cmd_ack  <= '0';
	        iscl_oen <= '1';
	        isda_oen <= '1';
	        sda_chk  <= '0';
	      else
	        cmd_ack <= '0'; -- default no acknowledge

	        if (clk_en = '1') then
	          case (c_state) is
	             -- idle
	             when idle =>
	                case cmd is
	                  when I2C_CMD_START => c_state <= start_a;
	                  when I2C_CMD_STOP  => c_state <= stop_a;
	                  when I2C_CMD_WRITE => c_state <= wr_a;
	                  when I2C_CMD_READ  => c_state <= rd_a;
	                  when others        => c_state <= idle; -- NOP command
	                end case;

	                iscl_oen <= iscl_oen; -- keep SCL in same state
	                isda_oen <= isda_oen; -- keep SDA in same state
	                sda_chk  <= '0';      -- don't check SDA

	             -- start
	             when start_a =>
	                c_state  <= start_b;
	                iscl_oen <= iscl_oen; -- keep SCL in same state (for repeated start)
	                isda_oen <= '1';      -- set SDA high
	                sda_chk  <= '0';      -- don't check SDA

	             when start_b =>
	                c_state  <= start_c;
	                iscl_oen <= '1'; -- set SCL high
	                isda_oen <= '1'; -- keep SDA high
	                sda_chk  <= '0'; -- don't check SDA

	             when start_c =>
	                c_state  <= start_d;
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= '0'; -- set SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             when start_d =>
	                c_state  <= start_e;
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= '0'; -- keep SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             when start_e =>
	                c_state  <= idle;
	                cmd_ack  <= '1'; -- command completed
	                iscl_oen <= '0'; -- set SCL low
	                isda_oen <= '0'; -- keep SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             -- stop
	             when stop_a =>
	                c_state  <= stop_b;
	                iscl_oen <= '0'; -- keep SCL low
	                isda_oen <= '0'; -- set SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             when stop_b =>
	                c_state  <= stop_c;
	                iscl_oen <= '1'; -- set SCL high
	                isda_oen <= '0'; -- keep SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             when stop_c =>
	                c_state  <= stop_d;
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= '0'; -- keep SDA low
	                sda_chk  <= '0'; -- don't check SDA

	             when stop_d =>
	                c_state  <= idle;
	                cmd_ack  <= '1'; -- command completed
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= '1'; -- set SDA high
	                sda_chk  <= '0'; -- don't check SDA

	             -- read
	             when rd_a =>
	                c_state  <= rd_b;
	                iscl_oen <= '0'; -- keep SCL low
	                isda_oen <= '1'; -- tri-state SDA
	                sda_chk  <= '0'; -- don't check SDA

	             when rd_b =>
	                c_state  <= rd_c;
	                iscl_oen <= '1'; -- set SCL high
	                isda_oen <= '1'; -- tri-state SDA
	                sda_chk  <= '0'; -- don't check SDA

	             when rd_c =>
	                c_state  <= rd_d;
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= '1'; -- tri-state SDA
	                sda_chk  <= '0'; -- don't check SDA

	             when rd_d =>
	                c_state  <= idle;
	                cmd_ack  <= '1'; -- command completed
	                iscl_oen <= '0'; -- set SCL low
	                isda_oen <= '1'; -- tri-state SDA
	                sda_chk  <= '0'; -- don't check SDA

	             -- write
	             when wr_a =>
	                c_state  <= wr_b;
	                iscl_oen <= '0'; -- keep SCL low
	                isda_oen <= din; -- set SDA
	                sda_chk  <= '0'; -- don't check SDA (SCL low)

	             when wr_b =>
	                c_state  <= wr_c;
	                iscl_oen <= '1'; -- set SCL high
	                isda_oen <= din; -- keep SDA
	                sda_chk  <= '1'; -- check SDA

	             when wr_c =>
	                c_state  <= wr_d;
	                iscl_oen <= '1'; -- keep SCL high
	                isda_oen <= din; -- keep SDA
	                sda_chk  <= '1'; -- check SDA

	             when wr_d =>
	                c_state  <= idle;
	                cmd_ack  <= '1'; -- command completed
	                iscl_oen <= '0'; -- set SCL low
	                isda_oen <= din; -- keep SDA
	                sda_chk  <= '0'; -- don't check SDA (SCL low)

	             when others =>

	          end case;
	        end if;
	      end if;
	    end if;
	end process nxt_state_decoder;


	-- assign outputs
	scl_o   <= '0';
	scl_oen <= iscl_oen;
	sda_o   <= '0';
	sda_oen <= isda_oen;
end architecture structural;


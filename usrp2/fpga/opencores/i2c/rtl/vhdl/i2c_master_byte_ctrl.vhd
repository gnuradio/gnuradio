---------------------------------------------------------------------
----                                                             ----
----  WISHBONE revB2 compl. I2C Master Core; byte-controller     ----
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
--  $Id: i2c_master_byte_ctrl.vhd,v 1.5 2004/02/18 11:41:48 rherveille Exp $
--
--  $Date: 2004/02/18 11:41:48 $
--  $Revision: 1.5 $
--  $Author: rherveille $
--  $Locker:  $
--  $State: Exp $
--
-- Change History:
--               $Log: i2c_master_byte_ctrl.vhd,v $
--               Revision 1.5  2004/02/18 11:41:48  rherveille
--               Fixed a potential bug in the statemachine. During a 'stop' 2 cmd_ack signals were generated. Possibly canceling a new start command.
--
--               Revision 1.4  2003/08/09 07:01:13  rherveille
--               Fixed a bug in the Arbitration Lost generation caused by delay on the (external) sda line.
--               Fixed a potential bug in the byte controller's host-acknowledge generation.
--
--               Revision 1.3  2002/12/26 16:05:47  rherveille
--               Core is now a Multimaster I2C controller.
--
--               Revision 1.2  2002/11/30 22:24:37  rherveille
--               Cleaned up code
--
--               Revision 1.1  2001/11/05 12:02:33  rherveille
--               Split i2c_master_core.vhd into separate files for each entity; same layout as verilog version.
--               Code updated, is now up-to-date to doc. rev.0.4.
--               Added headers.
--




--
------------------------------------------
-- Byte controller section
------------------------------------------
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity i2c_master_byte_ctrl is
	port (
		clk    : in std_logic;
		rst    : in std_logic; -- synchronous active high reset (WISHBONE compatible)
		nReset : in std_logic;	-- asynchornous active low reset (FPGA compatible)
		ena    : in std_logic; -- core enable signal

		clk_cnt : in unsigned(15 downto 0);	-- 4x SCL

		-- input signals
		start,
		stop,
		read,
		write,
		ack_in : std_logic;
		din    : in std_logic_vector(7 downto 0);

		-- output signals
		cmd_ack  : out std_logic; -- command done
		ack_out  : out std_logic;
		i2c_busy : out std_logic; -- arbitration lost
		i2c_al   : out std_logic; -- i2c bus busy
		dout     : out std_logic_vector(7 downto 0);

		-- i2c lines
		scl_i   : in std_logic;  -- i2c clock line input
		scl_o   : out std_logic; -- i2c clock line output
		scl_oen : out std_logic; -- i2c clock line output enable, active low
		sda_i   : in std_logic;  -- i2c data line input
		sda_o   : out std_logic; -- i2c data line output
		sda_oen : out std_logic  -- i2c data line output enable, active low
	);
end entity i2c_master_byte_ctrl;

architecture structural of i2c_master_byte_ctrl is
	component i2c_master_bit_ctrl is
	port (
		clk    : in std_logic;
		rst    : in std_logic;
		nReset : in std_logic;
		ena    : in std_logic;				-- core enable signal

		clk_cnt : in unsigned(15 downto 0);		-- clock prescale value

		cmd     : in std_logic_vector(3 downto 0);
		cmd_ack : out std_logic; -- command done
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
	end component i2c_master_bit_ctrl;

	-- commands for bit_controller block
	constant I2C_CMD_NOP  	: std_logic_vector(3 downto 0) := "0000";
	constant I2C_CMD_START	: std_logic_vector(3 downto 0) := "0001";
	constant I2C_CMD_STOP	 : std_logic_vector(3 downto 0) := "0010";
	constant I2C_CMD_READ	 : std_logic_vector(3 downto 0) := "0100";
	constant I2C_CMD_WRITE	: std_logic_vector(3 downto 0) := "1000";

	-- signals for bit_controller
	signal core_cmd : std_logic_vector(3 downto 0);
	signal core_ack, core_txd, core_rxd : std_logic;
	signal al : std_logic;

	-- signals for shift register
	signal sr : std_logic_vector(7 downto 0); -- 8bit shift register
	signal shift, ld : std_logic;

	-- signals for state machine
	signal go, host_ack : std_logic;
	signal dcnt : unsigned(2 downto 0); -- data counter
	signal cnt_done : std_logic;

begin
	-- hookup bit_controller
	bit_ctrl: i2c_master_bit_ctrl port map(
		clk     => clk,
		rst     => rst,
		nReset  => nReset,
		ena     => ena,
		clk_cnt => clk_cnt,
		cmd     => core_cmd,
		cmd_ack => core_ack,
		busy    => i2c_busy,
		al      => al,
		din     => core_txd,
		dout    => core_rxd,
		scl_i   => scl_i,
		scl_o   => scl_o,
		scl_oen => scl_oen,
		sda_i   => sda_i,
		sda_o   => sda_o,
		sda_oen => sda_oen
	);
	i2c_al <= al;

	-- generate host-command-acknowledge
	cmd_ack <= host_ack;

	-- generate go-signal
	go <= (read or write or stop) and not host_ack;

	-- assign Dout output to shift-register
	dout <= sr;

	-- generate shift register
	shift_register: process(clk, nReset)
	begin
	    if (nReset = '0') then
	      sr <= (others => '0');
	    elsif (clk'event and clk = '1') then
	      if (rst = '1') then
	        sr <= (others => '0');
	      elsif (ld = '1') then
	        sr <= din;
	      elsif (shift = '1') then
	        sr <= (sr(6 downto 0) & core_rxd);
	      end if;
	    end if;
	end process shift_register;

	-- generate data-counter
	data_cnt: process(clk, nReset)
	begin
	    if (nReset = '0') then
	      dcnt <= (others => '0');
	    elsif (clk'event and clk = '1') then
	      if (rst = '1') then
	        dcnt <= (others => '0');
	      elsif (ld = '1') then
	        dcnt <= (others => '1');  -- load counter with 7
	      elsif (shift = '1') then
	        dcnt <= dcnt -1;
	      end if;
	    end if;
	end process data_cnt;

	cnt_done <= '1' when (dcnt = 0) else '0';

	--
	-- state machine
	--
	statemachine : block
	    type states is (st_idle, st_start, st_read, st_write, st_ack, st_stop);
	    signal c_state : states;
	begin
	    --
	    -- command interpreter, translate complex commands into simpler I2C commands
	    --
	    nxt_state_decoder: process(clk, nReset)
	    begin
	        if (nReset = '0') then
	          core_cmd <= I2C_CMD_NOP;
	          core_txd <= '0';
	          shift    <= '0';
	          ld       <= '0';
	          host_ack <= '0';
	          c_state  <= st_idle;
	          ack_out  <= '0';
	        elsif (clk'event and clk = '1') then
	          if (rst = '1' or al = '1') then
	            core_cmd <= I2C_CMD_NOP;
	            core_txd <= '0';
	            shift    <= '0';
	            ld       <= '0';
	            host_ack <= '0';
	            c_state  <= st_idle;
	            ack_out  <= '0';
	          else
	            -- initialy reset all signal
	            core_txd <= sr(7);
	            shift    <= '0';
	            ld       <= '0';
	            host_ack <= '0';

	            case c_state is
	              when st_idle =>
	                 if (go = '1') then
	                   if (start = '1') then
	                     c_state  <= st_start;
	                     core_cmd <= I2C_CMD_START;
	                   elsif (read = '1') then
	                     c_state  <= st_read;
	                     core_cmd <= I2C_CMD_READ;
	                   elsif (write = '1') then
	                     c_state  <= st_write;
	                     core_cmd <= I2C_CMD_WRITE;
	                   else -- stop
	                     c_state  <= st_stop;
	                     core_cmd <= I2C_CMD_STOP;
	                   end if;

	                   ld <= '1';
	                 end if;

	              when st_start =>
	                 if (core_ack = '1') then
	                   if (read = '1') then
	                     c_state  <= st_read;
	                     core_cmd <= I2C_CMD_READ;
	                   else
	                     c_state  <= st_write;
	                     core_cmd <= I2C_CMD_WRITE;
	                   end if;

	                   ld <= '1';
	                 end if;

	              when st_write =>
	                 if (core_ack = '1') then
	                   if (cnt_done = '1') then
	                     c_state  <= st_ack;
	                     core_cmd <= I2C_CMD_READ;
	                   else
	                     c_state  <= st_write;       -- stay in same state
	                     core_cmd <= I2C_CMD_WRITE;  -- write next bit
	                     shift    <= '1';
	                   end if;
	                 end if;

	              when st_read =>
	                 if (core_ack = '1') then
	                   if (cnt_done = '1') then
	                     c_state  <= st_ack;
	                     core_cmd <= I2C_CMD_WRITE;
	                   else
	                     c_state  <= st_read;      -- stay in same state
	                     core_cmd <= I2C_CMD_READ; -- read next bit
	                   end if;

	                   shift    <= '1';
	                   core_txd <= ack_in;
	                 end if;

	              when st_ack =>
	                 if (core_ack = '1') then
	                   -- check for stop; Should a STOP command be generated ?
	                   if (stop = '1') then
	                     c_state  <= st_stop;
	                     core_cmd <= I2C_CMD_STOP;
	                   else
	                     c_state  <= st_idle;
	                     core_cmd <= I2C_CMD_NOP;

	                     -- generate command acknowledge signal
	                     host_ack <= '1';
	                   end if;

	                   -- assign ack_out output to core_rxd (contains last received bit)
	                   ack_out  <= core_rxd;

	                   core_txd <= '1';
	                 else
	                   core_txd <= ack_in;
	                 end if;

	              when st_stop =>
	                 if (core_ack = '1') then
	                   c_state  <= st_idle;
	                   core_cmd <= I2C_CMD_NOP;

	                   -- generate command acknowledge signal
	                   host_ack <= '1';
	                 end if;

	              when others => -- illegal states
	                 c_state  <= st_idle;
	                 core_cmd <= I2C_CMD_NOP;
	                 report ("Byte controller entered illegal state.");

	            end case;

	          end if;
	        end if;
	    end process nxt_state_decoder;

	end block statemachine;

end architecture structural;


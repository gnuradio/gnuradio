--
-- Simple I2C controller
--
-- 1) No multimaster
-- 2) No slave mode
-- 3) No fifo's
--
-- notes:
-- Every command is acknowledged. Do not set a new command before previous is acknowledged.
-- Dout is available 1 clock cycle later as cmd_ack
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

package I2C is
	component simple_i2c is
	port (
		clk : in std_logic;
		ena : in std_logic;
		nReset : in std_logic;

		clk_cnt : in unsigned(7 downto 0);	-- 4x SCL 

		-- input signals
		start,
		stop,
		read,
		write,
		ack_in : std_logic;
		Din : in std_logic_vector(7 downto 0);

		-- output signals
		cmd_ack : out std_logic;
		ack_out : out std_logic;
		Dout : out std_logic_vector(7 downto 0);

		-- i2c signals
		SCL : inout std_logic;
		SDA : inout std_logic
	);
	end component simple_i2c;
end package I2C;


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity simple_i2c is
	port (
		clk : in std_logic;
		ena : in std_logic;
		nReset : in std_logic;

		clk_cnt : in unsigned(7 downto 0);	-- 4x SCL 

		-- input signals
		start,
		stop,
		read,
		write,
		ack_in : std_logic;
		Din : in std_logic_vector(7 downto 0);

		-- output signals
		cmd_ack : out std_logic;
		ack_out : out std_logic;
		Dout : out std_logic_vector(7 downto 0);

		-- i2c signals
		SCL : inout std_logic;
		SDA : inout std_logic
	);
end entity simple_i2c;

architecture structural of simple_i2c is
	component i2c_core is
	port (
		clk : in std_logic;
		nReset : in std_logic;

		clk_cnt : in unsigned(7 downto 0);

		cmd : in std_logic_vector(2 downto 0);
		cmd_ack : out std_logic;
		busy : out std_logic;

		Din : in std_logic;
		Dout : out std_logic;

		SCL : inout std_logic;
		SDA : inout std_logic
	);
	end component i2c_core;

	-- commands for i2c_core
	constant CMD_NOP	: std_logic_vector(2 downto 0) := "000";
	constant CMD_START	: std_logic_vector(2 downto 0) := "010";
	constant CMD_STOP	: std_logic_vector(2 downto 0) := "011";
	constant CMD_READ	: std_logic_vector(2 downto 0) := "100";
	constant CMD_WRITE	: std_logic_vector(2 downto 0) := "101";

	-- signals for i2c_core
	signal core_cmd : std_logic_vector(2 downto 0);
	signal core_ack, core_busy, core_txd, core_rxd : std_logic;

	-- signals for shift register
	signal sr : std_logic_vector(7 downto 0); -- 8bit shift register
	signal shift, ld : std_logic;

	-- signals for state machine
	signal go, host_ack : std_logic;
begin
	-- hookup i2c core
	u1: i2c_core port map (clk, nReset, clk_cnt, core_cmd, core_ack, core_busy, core_txd, core_rxd, SCL, SDA);

	-- generate host-command-acknowledge
	cmd_ack <= host_ack;
	
	-- generate go-signal
	go <= (read or write) and not host_ack;

	-- assign Dout output to shift-register
	Dout <= sr;

	-- assign ack_out output to core_rxd (contains last received bit)
	ack_out <= core_rxd;

	-- generate shift register
	shift_register: process(clk)
	begin
		if (clk'event and clk = '1') then
			if (ld = '1') then
				sr <= din;
			elsif (shift = '1') then
				sr <= (sr(6 downto 0) & core_rxd);
			end if;
		end if;
	end process shift_register;

	--
	-- state machine
	--
	statemachine : block
		type states is (st_idle, st_start, st_read, st_write, st_ack, st_stop);
		signal state : states;
		signal dcnt : unsigned(2 downto 0);
	begin
		--
		-- command interpreter, translate complex commands into simpler I2C commands
		--
		nxt_state_decoder: process(clk, nReset, state)
			variable nxt_state : states;
			variable idcnt : unsigned(2 downto 0);
			variable ihost_ack : std_logic;
			variable icore_cmd : std_logic_vector(2 downto 0);
			variable icore_txd : std_logic;
			variable ishift, iload : std_logic;
		begin
			-- 8 databits (1byte) of data to shift-in/out
			idcnt := dcnt;

			-- no acknowledge (until command complete)
			ihost_ack := '0';

			icore_txd := core_txd;

			-- keep current command to i2c_core
			icore_cmd := core_cmd;

			-- no shifting or loading of shift-register
			ishift := '0';
			iload := '0';

			-- keep current state;
			nxt_state := state;
			case state is
				when st_idle =>
					if (go = '1') then
						if (start = '1') then
							nxt_state := st_start;	
							icore_cmd := CMD_START;
						elsif (read = '1') then
							nxt_state := st_read;
							icore_cmd := CMD_READ;
							idcnt := "111";
						else
							nxt_state := st_write;
							icore_cmd := CMD_WRITE;
							idcnt := "111";
							iload := '1';
						end if;
					end if;

				when st_start =>
					if (core_ack = '1') then
						if (read = '1') then
							nxt_state := st_read;
							icore_cmd := CMD_READ;
							idcnt := "111";
						else
							nxt_state := st_write;
							icore_cmd := CMD_WRITE;
							idcnt := "111";
							iload := '1';
						end if;
					end if;

				when st_write =>
					if (core_ack = '1') then
						idcnt := dcnt -1;	-- count down Data_counter
						icore_txd := sr(7);
						if (dcnt = 0) then
							nxt_state := st_ack;
							icore_cmd := CMD_READ;
						else
							ishift := '1';
--							icore_txd := sr(7);
						end if;
					end if;			

				when st_read =>
					if (core_ack = '1') then
						idcnt := dcnt -1;	-- count down Data_counter
						ishift := '1';
						if (dcnt = 0) then
							nxt_state := st_ack;
							icore_cmd := CMD_WRITE;
							icore_txd := ack_in;
						end if;
					end if;			

				when st_ack =>
					if (core_ack = '1') then
						-- generate command acknowledge signal
						ihost_ack := '1';

						-- Perform an additional shift, needed for 'read' (store last received bit in shift register)
						ishift := '1';

						-- check for stop; Should a STOP command be generated ?
						if (stop = '1') then
							nxt_state := st_stop;
							icore_cmd := CMD_STOP;
						else
							nxt_state := st_idle;
							icore_cmd := CMD_NOP;
						end if;
					end if;

				when st_stop =>
					if (core_ack = '1') then
						nxt_state := st_idle;
						icore_cmd := CMD_NOP;
					end if;

				when others => -- illegal states
					nxt_state := st_idle;
					icore_cmd := CMD_NOP;
			end case;

			-- generate registers
			if (nReset = '0') then
				core_cmd <= CMD_NOP;
				core_txd <= '0';
				
				shift <= '0';
				ld <= '0';

				dcnt <= "111";
				host_ack <= '0';

				state <= st_idle;
			elsif (clk'event and clk = '1') then
				if (ena = '1') then
					state <= nxt_state;

					dcnt <= idcnt;
					shift <= ishift;
					ld <= iload;

					core_cmd <= icore_cmd;
					core_txd <= icore_txd;

					host_ack <= ihost_ack;
				end if;
			end if;
		end process nxt_state_decoder;

	end block statemachine;

end architecture structural;


--
--
-- I2C Core
--
-- Translate simple commands into SCL/SDA transitions
-- Each command has 5 states, A/B/C/D/idle
--
-- start:	SCL	~~~~~~~~~~\____
--	SDA	~~~~~~~~\______
--		 x | A | B | C | D | i
--
-- repstart	SCL	____/~~~~\___
--	SDA	__/~~~\______
--		 x | A | B | C | D | i
--
-- stop	SCL	____/~~~~~~~~
--	SDA	==\____/~~~~~
--		 x | A | B | C | D | i
--
--- write	SCL	____/~~~~\____
--	SDA	==X=========X=
--		 x | A | B | C | D | i
--
--- read	SCL	____/~~~~\____
--	SDA	XXXX=====XXXX
--		 x | A | B | C | D | i
--

-- Timing:		Normal mode	Fast mode
-----------------------------------------------------------------
-- Fscl		100KHz		400KHz
-- Th_scl		4.0us		0.6us	High period of SCL
-- Tl_scl		4.7us		1.3us	Low period of SCL
-- Tsu:sta		4.7us		0.6us	setup time for a repeated start condition
-- Tsu:sto		4.0us		0.6us	setup time for a stop conditon
-- Tbuf		4.7us		1.3us	Bus free time between a stop and start condition
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity i2c_core is
	port (
		clk : in std_logic;
		nReset : in std_logic;

		clk_cnt : in unsigned(7 downto 0);

		cmd : in std_logic_vector(2 downto 0);
		cmd_ack : out std_logic;
		busy : out std_logic;

		Din : in std_logic;
		Dout : out std_logic;

		SCL : inout std_logic;
		SDA : inout std_logic
	);
end entity i2c_core;

architecture structural of i2c_core is
	constant CMD_NOP	: std_logic_vector(2 downto 0) := "000";
	constant CMD_START	: std_logic_vector(2 downto 0) := "010";
	constant CMD_STOP	: std_logic_vector(2 downto 0) := "011";
	constant CMD_READ	: std_logic_vector(2 downto 0) := "100";
	constant CMD_WRITE	: std_logic_vector(2 downto 0) := "101";

	type cmds is (idle, start_a, start_b, start_c, start_d, stop_a, stop_b, stop_c, rd_a, rd_b, rd_c, rd_d, wr_a, wr_b, wr_c, wr_d);
	signal state : cmds;
	signal SDAo, SCLo : std_logic;
	signal txd : std_logic;
	signal clk_en, slave_wait :std_logic;
	signal cnt : unsigned(7 downto 0) := clk_cnt;
begin
	-- whenever the slave is not ready it can delay the cycle by pulling SCL low
	slave_wait <= '1' when ((SCLo = '1') and (SCL = '0')) else '0';

	-- generate clk enable signal
	gen_clken: process(clk, nReset)
	begin
		if (nReset = '0') then
			cnt <= (others => '0');
			clk_en <= '1'; --'0';
		elsif (clk'event and clk = '1') then
			if (cnt = 0) then
				clk_en <= '1';
				cnt <= clk_cnt;
			else
				if (slave_wait = '0') then
					cnt <= cnt -1;
				end if;
				clk_en <= '0';
			end if;
		end if;
	end process gen_clken;

	-- generate statemachine
	nxt_state_decoder : process (clk, nReset, state, cmd, SDA)
		variable nxt_state : cmds;
		variable icmd_ack, ibusy, store_sda : std_logic;
		variable itxd : std_logic;
	begin

		nxt_state := state;

		icmd_ack := '0'; -- default no acknowledge
		ibusy := '1'; -- default busy

		store_sda := '0';

		itxd := txd;

		case (state) is
			-- idle
			when idle =>
				case cmd is
					when CMD_START =>
						nxt_state := start_a;
						icmd_ack := '1'; -- command completed

					when CMD_STOP =>
						nxt_state := stop_a;
						icmd_ack := '1'; -- command completed

					when CMD_WRITE =>
						nxt_state := wr_a;
						icmd_ack := '1'; -- command completed
						itxd := Din;

					when CMD_READ =>
						nxt_state := rd_a;
						icmd_ack := '1'; -- command completed

					when others =>
						nxt_state := idle;
-- don't acknowledge NOP command						icmd_ack := '1'; -- command completed
						ibusy := '0';
				end case;

			-- start
			when start_a =>
				nxt_state := start_b;

			when start_b =>
				nxt_state := start_c;

			when start_c =>
				nxt_state := start_d;

			when start_d =>
				nxt_state := idle;
				ibusy := '0'; -- not busy when idle


			-- stop
			when stop_a =>
				nxt_state := stop_b;

			when stop_b =>
				nxt_state := stop_c;

			when stop_c =>
--				nxt_state := stop_d;

--			when stop_d =>
				nxt_state := idle;
				ibusy := '0'; -- not busy when idle

			-- read
			when rd_a =>
				nxt_state := rd_b;

			when rd_b =>
				nxt_state := rd_c;

			when rd_c =>
				nxt_state := rd_d;
				store_sda := '1';

			when rd_d =>
				nxt_state := idle;
				ibusy := '0'; -- not busy when idle

			-- write
			when wr_a =>
				nxt_state := wr_b;

			when wr_b =>
				nxt_state := wr_c;

			when wr_c =>
				nxt_state := wr_d;

			when wr_d =>
				nxt_state := idle;
				ibusy := '0'; -- not busy when idle

		end case;

		-- generate regs
		if (nReset = '0') then
			state <= idle;
			cmd_ack <= '0';
			busy <= '0';
			txd <= '0';
			Dout <= '0';
		elsif (clk'event and clk = '1') then
			if (clk_en = '1') then
				state <= nxt_state;
				busy <= ibusy;

				txd <= itxd;
				if (store_sda = '1') then
					Dout <= SDA;
				end if;
			end if;

			cmd_ack <= icmd_ack and clk_en;
		end if;
	end process nxt_state_decoder;

	--
	-- convert states to SCL and SDA signals
	--
	output_decoder: process (clk, nReset, state)
		variable iscl, isda : std_logic;
	begin
		case (state) is
			when idle =>
				iscl := SCLo; -- keep SCL in same state
				isda := SDA; -- keep SDA in same state

			-- start
			when start_a =>
				iscl := SCLo; -- keep SCL in same state (for repeated start)
				isda := '1'; -- set SDA high

			when start_b =>
				iscl := '1';	-- set SCL high
				isda := '1'; -- keep SDA high

			when start_c =>
				iscl := '1';	-- keep SCL high
				isda := '0'; -- sel SDA low

			when start_d =>
				iscl := '0'; -- set SCL low
				isda := '0'; -- keep SDA low

			-- stop
			when stop_a =>
				iscl := '0'; -- keep SCL disabled
				isda := '0'; -- set SDA low

			when stop_b =>
				iscl := '1'; -- set SCL high
				isda := '0'; -- keep SDA low

			when stop_c =>
				iscl := '1'; -- keep SCL high
				isda := '1'; -- set SDA high

			-- write
			when wr_a =>
				iscl := '0';	-- keep SCL low
--				isda := txd; -- set SDA
				isda := Din;

			when wr_b =>
				iscl := '1';	-- set SCL high
--				isda := txd; -- set SDA
				isda := Din;

			when wr_c =>
				iscl := '1';	-- keep SCL high
--				isda := txd; -- set SDA
				isda := Din;

			when wr_d =>
				iscl := '0'; -- set SCL low
--				isda := txd; -- set SDA
				isda := Din;

			-- read
			when rd_a =>
				iscl := '0'; -- keep SCL low
				isda := '1'; -- tri-state SDA

			when rd_b =>
				iscl := '1'; -- set SCL high
				isda := '1'; -- tri-state SDA

			when rd_c =>
				iscl := '1'; -- keep SCL high
				isda := '1'; -- tri-state SDA

			when rd_d =>
				iscl := '0'; -- set SCL low
				isda := '1'; -- tri-state SDA
		end case;

		-- generate registers
		if (nReset = '0') then
			SCLo <= '1';
			SDAo <= '1';
		elsif (clk'event and clk = '1') then
			if (clk_en = '1') then
				SCLo <= iscl;
				SDAo <= isda;
			end if;
		end if;
	end process output_decoder;

	SCL <= '0' when (SCLo = '0') else 'Z'; -- since SCL is externally pulled-up convert a '1' to a 'Z'(tri-state)
	SDA <= '0' when (SDAo = '0') else 'Z'; -- since SDA is externally pulled-up convert a '1' to a 'Z'(tri-state)
--	SCL <= SCLo;
--	SDA <= SDAo;

end architecture structural;





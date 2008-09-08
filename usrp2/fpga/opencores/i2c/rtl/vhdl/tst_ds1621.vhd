--
--
-- State machine for reading data from Dallas 1621
--
-- Testsystem for i2c controller
--
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

use work.i2c.all;

entity DS1621_interface is
	port (
		clk : in std_logic;
		nReset : in std_logic;

		Dout : out std_logic_vector(7 downto 0);	-- data read from ds1621

		error : out std_logic; -- no correct ack received

		SCL : inout std_logic;
		SDA : inout std_logic
	);
end entity DS1621_interface;

architecture structural of DS1621_interface is
	constant SLAVE_ADDR : std_logic_vector(6 downto 0) := "1001000";
	constant CLK_CNT : unsigned(7 downto 0) := conv_unsigned(20, 8);

	signal cmd_ack : std_logic;
	signal D : std_logic_vector(7 downto 0);
	signal lack, store_dout : std_logic;

	signal start, read, write, ack, stop : std_logic;
	signal i2c_dout : std_logic_vector(7 downto 0);

begin
	-- hookup I2C controller
	u1: simple_i2c port map (clk => clk, ena => '1', clk_cnt => clk_cnt, nReset => nReset,
			read => read, write => write, start => start, stop => stop, ack_in => ack, cmd_ack => cmd_ack, 
			Din => D, Dout => i2c_dout, ack_out => lack, SCL => SCL, SDA => SDA);

	init_statemachine : block
		type states is (i1, i2, i3, i4, i5, t1, t2, t3, t4, t5);
		signal state : states;
	begin
		nxt_state_decoder: process(clk, nReset, state)
			variable nxt_state : states;
			variable iD : std_logic_vector(7 downto 0);
			variable ierr : std_logic;
			variable istart, iread, iwrite, iack, istop : std_logic;
			variable istore_dout : std_logic;
		begin
			nxt_state := state;
			ierr := '0';
			istore_dout := '0';

			istart := start;
			iread := read;
			iwrite := write;
			iack := ack;
			istop := stop;
			iD := D;

			case (state) is
				-- init DS1621
				-- 1) send start condition
				-- 2) send slave address + write
				-- 3) check ack
				-- 4) send "access config" command (0xAC)
				-- 5) check ack
				-- 6) send config register data (0x00)
				-- 7) check ack
				-- 8) send stop condition
				-- 9) send start condition
				-- 10) send slave address + write
				-- 11) check ack
				-- 12) send "start conversion" command (0xEE)
				-- 13) check ack
				-- 14) send stop condition

				when i1 =>	-- send start condition, sent slave address + write
					nxt_state := i2;
					istart := '1';
					iread := '0';
					iwrite := '1';
					iack := '0';
					istop := '0';
					iD := (slave_addr & '0'); -- write to slave (R/W = '0')

				when i2 =>	-- send "access config" command
					if (cmd_ack = '1') then
						nxt_state := i3;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '0';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '0';
						iD := x"AC";
					end if;

				when i3 =>	-- send config register data, sent stop condition
					if (cmd_ack = '1') then
						nxt_state := i4;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '0';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '1';
						iD := x"00";
					end if;

				when i4 =>	-- send start condition, sent slave address + write
					if (cmd_ack = '1') then
						nxt_state := i5;
	
						istart := '1';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '0';
						iD := (slave_addr & '0'); -- write to slave (R/W = '0')
					end if;

				when i5 =>	-- send "start conversion" command + stop condition
					if (cmd_ack = '1') then
						nxt_state := t1;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '0';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '1';
						iD := x"EE";
					end if;
				-- read temperature
				-- 1) sent start condition
				-- 2) sent slave address + write
				-- 3) check ack
				-- 4) sent "read temperature" command (0xAA)
				-- 5) check ack
				-- 6) sent start condition
				-- 7) sent slave address + read
				-- 8) check ack
				-- 9) read msb
				-- 10) send ack
				-- 11) read lsb
				-- 12) send nack
				-- 13) send stop condition

				when t1 =>	-- send start condition, sent slave address + write
					if (cmd_ack = '1') then
						nxt_state := t2;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '1';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '0';
						iD := (slave_addr & '0'); -- write to slave (R/W = '0')
					end if;

				when t2 =>	-- send read temperature command
					if (cmd_ack = '1') then
						nxt_state := t3;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '0';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '0';
						iD := x"AA";
					end if;

				when t3 =>	-- send (repeated) start condition, send slave address + read
					if (cmd_ack = '1') then
						nxt_state := t4;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received, expected ACK
						end if;

						istart := '1';
						iread := '0';
						iwrite := '1';
						iack := '0';
						istop := '0';
						iD := (slave_addr & '1'); -- read from slave (R/W = '1')
					end if;

				when t4 =>	-- read MSB (hi-byte), send acknowledge
					if (cmd_ack = '1') then
						nxt_state := t5;
						-- check aknowledge bit
						if (lack = '1') then
							ierr := '1'; -- no acknowledge received from last command, expected ACK
						end if;

						istart := '0';
						iread := '1';
						iwrite := '0';
						iack := '0'; --ACK
						istop := '0';
					end if;

				when t5 =>	-- read LSB (lo-byte), send acknowledge, sent stop
					if (cmd_ack = '1') then
						nxt_state := t1;

						istart := '0';
						iread := '1';
						iwrite := '0';
						iack := '1'; --NACK
						istop := '1';

						istore_dout := '1';
					end if;
			end case;

			-- genregs
			if (nReset = '0') then
				state <= i1;
				error <= '0';
				store_dout <= '0';

				start <= '0';
				read <= '0';
				write <= '0';
				ack <= '0';
				stop <= '0';
				D <= (others => '0');
			elsif (clk'event and clk = '1') then
				state <= nxt_state;
				error <= ierr;
				store_dout <= istore_dout;

				start <= istart;
				read <= iread;
				write <= iwrite;
				ack <= iack;
				stop <= istop;
				D <= iD;
			end if;
		end process nxt_state_decoder;
	end block init_statemachine;

	-- store temp
	gen_dout : process(clk)
	begin
		if (clk'event and clk = '1') then
			if (store_dout = '1') then
				Dout <= i2c_dout;
			end if;
		end if;
	end process gen_dout;

end architecture structural;



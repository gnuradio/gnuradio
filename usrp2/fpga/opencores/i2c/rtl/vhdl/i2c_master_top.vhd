---------------------------------------------------------------------
----                                                             ----
----  WISHBONE revB2 compl. I2C Master Core; top level           ----
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
--  $Id: i2c_master_top.vhd,v 1.7 2004/03/14 10:17:03 rherveille Exp $
--
--  $Date: 2004/03/14 10:17:03 $
--  $Revision: 1.7 $
--  $Author: rherveille $
--  $Locker:  $
--  $State: Exp $
--
-- Change History:
--               $Log: i2c_master_top.vhd,v $
--               Revision 1.7  2004/03/14 10:17:03  rherveille
--               Fixed simulation issue when writing to CR register
--
--               Revision 1.6  2003/08/09 07:01:13  rherveille
--               Fixed a bug in the Arbitration Lost generation caused by delay on the (external) sda line.
--               Fixed a potential bug in the byte controller's host-acknowledge generation.
--
--               Revision 1.5  2003/02/01 02:03:06  rherveille
--               Fixed a few 'arbitration lost' bugs. VHDL version only.
--
--               Revision 1.4  2002/12/26 16:05:47  rherveille
--               Core is now a Multimaster I2C controller.
--
--               Revision 1.3  2002/11/30 22:24:37  rherveille
--               Cleaned up code
--
--               Revision 1.2  2001/11/10 10:52:44  rherveille
--               Changed PRER reset value from 0x0000 to 0xffff, conform specs.
--


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

entity i2c_master_top is
	generic(
		ARST_LVL : std_logic := '0'                   -- asynchronous reset level
	);
	port (
		-- wishbone signals
		wb_clk_i  : in  std_logic;                    -- master clock input
		wb_rst_i  : in  std_logic := '0';             -- synchronous active high reset
		arst_i    : in  std_logic := not ARST_LVL;    -- asynchronous reset
		wb_adr_i  : in  unsigned(2 downto 0);         -- lower address bits
		wb_dat_i  : in  std_logic_vector(7 downto 0); -- Databus input
		wb_dat_o  : out std_logic_vector(7 downto 0); -- Databus output
		wb_we_i   : in  std_logic;	              -- Write enable input
		wb_stb_i  : in  std_logic;                    -- Strobe signals / core select signal
		wb_cyc_i  : in  std_logic;	              -- Valid bus cycle input
		wb_ack_o  : out std_logic;                    -- Bus cycle acknowledge output
		wb_inta_o : out std_logic;                    -- interrupt request output signal

		-- i2c lines
		scl_pad_i     : in  std_logic;                -- i2c clock line input
		scl_pad_o     : out std_logic;                -- i2c clock line output
		scl_padoen_o  : out std_logic;                -- i2c clock line output enable, active low
		sda_pad_i     : in  std_logic;                -- i2c data line input
		sda_pad_o     : out std_logic;                -- i2c data line output
		sda_padoen_o  : out std_logic                 -- i2c data line output enable, active low
	);
end entity i2c_master_top;

architecture structural of i2c_master_top is
	component i2c_master_byte_ctrl is
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
		cmd_ack  : out std_logic;
		ack_out  : out std_logic;
		i2c_busy : out std_logic;
		i2c_al   : out std_logic;
		dout     : out std_logic_vector(7 downto 0);

		-- i2c lines
		scl_i   : in std_logic;  -- i2c clock line input
		scl_o   : out std_logic; -- i2c clock line output
		scl_oen : out std_logic; -- i2c clock line output enable, active low
		sda_i   : in std_logic;  -- i2c data line input
		sda_o   : out std_logic; -- i2c data line output
		sda_oen : out std_logic  -- i2c data line output enable, active low
	);
	end component i2c_master_byte_ctrl;

	-- registers
	signal prer : unsigned(15 downto 0);             -- clock prescale register
	signal ctr  : std_logic_vector(7 downto 0);      -- control register
	signal txr  : std_logic_vector(7 downto 0);      -- transmit register
	signal rxr  : std_logic_vector(7 downto 0);      -- receive register
	signal cr   : std_logic_vector(7 downto 0);      -- command register
	signal sr   : std_logic_vector(7 downto 0);      -- status register

	-- internal reset signal
	signal rst_i : std_logic;

	-- wishbone write access
	signal wb_wacc : std_logic;

	-- internal acknowledge signal
	signal iack_o : std_logic;

	-- done signal: command completed, clear command register
	signal done : std_logic;

	-- command register signals
	signal sta, sto, rd, wr, ack, iack : std_logic;

	signal core_en : std_logic;                      -- core enable signal
	signal ien     : std_logic;                      -- interrupt enable signal

	-- status register signals
	signal irxack, rxack : std_logic;                -- received aknowledge from slave
	signal tip           : std_logic;                -- transfer in progress
	signal irq_flag      : std_logic;                -- interrupt pending flag
	signal i2c_busy      : std_logic;                -- i2c bus busy (start signal detected)
	signal i2c_al, al    : std_logic;                -- arbitration lost

begin
	-- generate internal reset signal
	rst_i <= arst_i xor ARST_LVL;

	-- generate acknowledge output signal
	gen_ack_o : process(wb_clk_i)
	begin
	    if (wb_clk_i'event and wb_clk_i = '1') then
	      iack_o <= wb_cyc_i and wb_stb_i and not iack_o;         -- because timing is always honored
	    end if;
	end process gen_ack_o;
	wb_ack_o <= iack_o;
	
	
	-- generate wishbone write access signal
	wb_wacc <= wb_cyc_i and wb_stb_i and wb_we_i;

	-- assign wb_dat_o
	assign_dato : process(wb_clk_i)
	begin
	    if (wb_clk_i'event and wb_clk_i = '1') then
	      case wb_adr_i is
	        when "000"  => wb_dat_o <= std_logic_vector(prer( 7 downto 0));
	        when "001"  => wb_dat_o <= std_logic_vector(prer(15 downto 8));
	        when "010"  => wb_dat_o <= ctr;
	        when "011"  => wb_dat_o <= rxr; -- write is transmit register TxR
	        when "100"  => wb_dat_o <= sr;  -- write is command register CR

	        -- Debugging registers:
	        -- These registers are not documented.
	        -- Functionality could change in future releases
	        when "101"  => wb_dat_o <= txr;
	        when "110"  => wb_dat_o <= cr;
	        when "111"  => wb_dat_o <= (others => '0');
	        when others => wb_dat_o <= (others => 'X');	-- for simulation only
	      end case;
	    end if;
	end process assign_dato;


	-- generate registers (CR, SR see below)
	gen_regs: process(rst_i, wb_clk_i)
	begin
	    if (rst_i = '0') then
	      prer <= (others => '1');
	      ctr  <= (others => '0');
	      txr  <= (others => '0');
	    elsif (wb_clk_i'event and wb_clk_i = '1') then
	      if (wb_rst_i = '1') then
	        prer <= (others => '1');
	        ctr  <= (others => '0');
	        txr  <= (others => '0');
	      elsif (wb_wacc = '1') then
	        case wb_adr_i is
	           when "000" => prer( 7 downto 0) <= unsigned(wb_dat_i);
	           when "001" => prer(15 downto 8) <= unsigned(wb_dat_i);
	           when "010" => ctr               <= wb_dat_i;
	           when "011" => txr               <= wb_dat_i;
	           when "100" => null; --write to CR, avoid executing the others clause

	           -- illegal cases, for simulation only
	           when others =>
	              report ("Illegal write address, setting all registers to unknown.");
	              prer <= (others => 'X');
	              ctr  <= (others => 'X');
	              txr  <= (others => 'X');
	        end case;
	      end if;
	    end if;
	end process gen_regs;


	-- generate command register
	gen_cr: process(rst_i, wb_clk_i)
	begin
	    if (rst_i = '0') then
	        cr <= (others => '0');
	    elsif (wb_clk_i'event and wb_clk_i = '1') then
	        if (wb_rst_i = '1') then
	            cr <= (others => '0');
	        elsif (wb_wacc = '1') then
	            if ( (core_en = '1') and (wb_adr_i = 4) ) then
	                -- only take new commands when i2c core enabled
	                -- pending commands are finished
	                cr <= wb_dat_i;
	            end if;
	        else
	            if (done = '1' or i2c_al = '1') then
	                cr(7 downto 4) <= (others => '0'); -- clear command bits when command done or arbitration lost
	            end if;

	            cr(2 downto 1) <= (others => '0');   -- reserved bits, always '0'
	            cr(0) <= '0';                        -- clear IRQ_ACK bit
	        end if;
	    end if;
	end process gen_cr;

	-- decode command register
	sta  <= cr(7);
	sto  <= cr(6);
	rd   <= cr(5);
	wr   <= cr(4);
	ack  <= cr(3);
	iack <= cr(0);

	-- decode control register
	core_en <= ctr(7);
	ien     <= ctr(6);

	-- hookup byte controller block
	byte_ctrl: i2c_master_byte_ctrl port map (
		clk      => wb_clk_i,
		rst      => wb_rst_i,
		nReset   => rst_i,
		ena      => core_en,
		clk_cnt  => prer,
		start    => sta,
		stop     => sto,
		read     => rd,
		write    => wr,
		ack_in   => ack,
		i2c_busy => i2c_busy,
		i2c_al   => i2c_al,
		din      => txr,
		cmd_ack  => done,
		ack_out  => irxack,
		dout     => rxr,
		scl_i    => scl_pad_i,
		scl_o    => scl_pad_o,
		scl_oen  => scl_padoen_o,
		sda_i    => sda_pad_i,
		sda_o    => sda_pad_o,
		sda_oen  => sda_padoen_o
	);


	-- status register block + interrupt request signal
	st_irq_block : block
	begin
	    -- generate status register bits
	    gen_sr_bits: process (wb_clk_i, rst_i)
	    begin
	        if (rst_i = '0') then
	          al       <= '0';
	          rxack    <= '0';
	          tip      <= '0';
	          irq_flag <= '0';
	        elsif (wb_clk_i'event and wb_clk_i = '1') then
	          if (wb_rst_i = '1') then
	            al       <= '0';
	            rxack    <= '0';
	            tip      <= '0';
	            irq_flag <= '0';
	          else
	            al       <= i2c_al or (al and not sta);
	            rxack    <= irxack;
	            tip      <= (rd or wr);

	            -- interrupt request flag is always generated
	            irq_flag <= (done or i2c_al or irq_flag) and not iack;
	          end if;
	        end if;
	    end process gen_sr_bits;

	    -- generate interrupt request signals
	    gen_irq: process (wb_clk_i, rst_i)
	    begin
	        if (rst_i = '0') then
	          wb_inta_o <= '0';
	        elsif (wb_clk_i'event and wb_clk_i = '1') then
	          if (wb_rst_i = '1') then
	            wb_inta_o <= '0';
	          else
	            -- interrupt signal is only generated when IEN (interrupt enable bit) is set
	            wb_inta_o <= irq_flag and ien;
	          end if;
	        end if;
	    end process gen_irq;

	    -- assign status register bits
	    sr(7)          <= rxack;
	    sr(6)          <= i2c_busy;
	    sr(5)          <= al;
	    sr(4 downto 2) <= (others => '0'); -- reserved
	    sr(1)          <= tip;
	    sr(0)          <= irq_flag;
	end block;

end architecture structural;

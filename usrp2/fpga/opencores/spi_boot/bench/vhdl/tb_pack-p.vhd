-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: tb_pack-p.vhd,v 1.2 2005/03/08 22:06:39 arniml Exp $
--
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package tb_pack is

  function calc_crc(payload : in std_logic_vector) return std_logic_vector;
  function calc_crc(payload : in unsigned) return unsigned;

  function to_string(value : in integer) return string;

end tb_pack;


package body tb_pack is

  function calc_crc(payload : in std_logic_vector) return std_logic_vector is

    variable crc_v  : std_logic_vector(6 downto 0);
    variable temp_v : std_logic;

  begin

    crc_v := (others => '0');

    for i in payload'high downto payload'low loop
      temp_v := payload(i) xor crc_v(6);

      crc_v(6 downto 4) := crc_v(5 downto 3);
      crc_v(3) := crc_v(2) xor temp_v;
      crc_v(2 downto 1) := crc_v(1 downto 0);
      crc_v(0) := temp_v;
    end loop;

    return crc_v;
  end calc_crc;

  function calc_crc(payload : in unsigned) return unsigned is
  begin
    return unsigned(calc_crc(std_logic_vector(payload)));
  end calc_crc;

  function to_string(value : in integer) return string is
    variable str: string (11 downto 1);
    variable val: integer := value;
    variable digit: natural;
    variable index: natural := 0;
  begin
    -- Taken from:
    --  textio package body.  This file is part of GHDL.
    --  Copyright (C) 2002 Tristan Gingold.
    --  Note: the absolute value of VAL cannot be directly taken, since
    --  it may be greather that the maximum value of an INTEGER.
    loop
      --  LRM93 7.2.6
      --  (A rem B) has the sign of A and an absolute value less then
      --   the absoulte value of B.
      digit := abs (val rem 10);
      val := val / 10;
      index := index + 1;
      str (index) := character'val(48 + digit);
      exit when val = 0;
    end loop;
    if value < 0 then
      index := index + 1;
      str(index) := '-';
    end if;

    return str;
  end to_string;

end tb_pack;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: tb_pack-p.vhd,v $
-- Revision 1.2  2005/03/08 22:06:39  arniml
-- added integer->string conversion function
--
-- Revision 1.1  2005/02/08 21:09:20  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

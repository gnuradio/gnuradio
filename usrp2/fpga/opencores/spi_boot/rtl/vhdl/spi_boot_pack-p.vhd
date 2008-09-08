-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: spi_boot_pack-p.vhd,v 1.1 2005/02/08 20:41:33 arniml Exp $
--
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

package spi_boot_pack is

  function "=" (a : std_logic; b : integer) return boolean;

end spi_boot_pack;

package body spi_boot_pack is

  function "=" (a : std_logic; b : integer) return boolean is
    variable result_v : boolean;
  begin
    result_v := false;

    case a is
      when '0' =>
        if b = 0 then
          result_v := true;
        end if;

      when '1' =>
        if b = 1 then
          result_v := true;
        end if;

      when others =>
        null;

    end case;

    return result_v;
  end;

end spi_boot_pack;


-------------------------------------------------------------------------------
-- File History:
--
-- $Log: spi_boot_pack-p.vhd,v $
-- Revision 1.1  2005/02/08 20:41:33  arniml
-- initial check-in
--
-------------------------------------------------------------------------------

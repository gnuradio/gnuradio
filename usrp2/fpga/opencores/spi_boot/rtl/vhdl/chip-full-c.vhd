-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: chip-full-c.vhd,v 1.1 2005/02/08 20:41:31 arniml Exp $
--
-------------------------------------------------------------------------------

configuration chip_full_c0 of chip is

  for full

    for spi_boot_b : spi_boot
      use configuration work.spi_boot_rtl_c0;
    end for;

  end for;

end chip_full_c0;

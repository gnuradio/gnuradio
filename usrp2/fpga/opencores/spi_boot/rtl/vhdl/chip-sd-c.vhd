-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: chip-sd-c.vhd,v 1.1 2005/02/08 20:41:33 arniml Exp $
--
-------------------------------------------------------------------------------

configuration chip_sd_c0 of chip is

  for sd

    for spi_boot_b : spi_boot
      use configuration work.spi_boot_rtl_c0;
    end for;

  end for;

end chip_sd_c0;

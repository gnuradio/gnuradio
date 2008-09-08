-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: chip-minimal-c.vhd,v 1.1 2005/02/08 20:41:32 arniml Exp $
--
-------------------------------------------------------------------------------

configuration chip_minimal_c0 of chip is

  for minimal

    for spi_boot_b : spi_boot
      use configuration work.spi_boot_rtl_c0;
    end for;

  end for;

end chip_minimal_c0;

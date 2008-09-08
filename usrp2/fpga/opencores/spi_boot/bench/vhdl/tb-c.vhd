-------------------------------------------------------------------------------
--
-- SD/MMC Bootloader
--
-- $Id: tb-c.vhd,v 1.1 2005/02/08 21:09:20 arniml Exp $
--
-------------------------------------------------------------------------------

configuration tb_behav_c0 of tb is

  for behav

    for tb_elem_full_b : tb_elem
      use configuration work.tb_elem_behav_full;
    end for;

    for tb_elem_mmc_b : tb_elem
      use configuration work.tb_elem_behav_mmc;
    end for;

    for tb_elem_sd_b : tb_elem
      use configuration work.tb_elem_behav_sd;
    end for;

    for tb_elem_minimal_b : tb_elem
      use configuration work.tb_elem_behav_minimal;
    end for;

  end for;

end tb_behav_c0;

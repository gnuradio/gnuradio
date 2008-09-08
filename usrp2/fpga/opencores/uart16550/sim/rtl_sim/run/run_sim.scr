#!/bin/csh -f


# GLOBAL VARIABLES
###################

set sim_top  = testbench;
set arg_tool = "NCSim";        # By default NCSim is used as simulation tool 
set arg_wave = 0;              # By default waveform is not recorded 
set arg_verb = 0;              # By default basic display on monitor (no verbose)
set arg_test = 0;              # By default all testcases are simulated 


# GETTING PARAMETERS FROM COMMAND LINE
#######################################

set cur_arg = 1;

if ($#argv < 1) then
    echo ""
    echo "    Verification without any argument:"
else

    while ($cur_arg <= $#argv)

        switch ("$argv[$cur_arg]")
        # HELP ARGUMENT
        case "-h":
            goto help
            breaksw
        case "help":
            goto help
            breaksw
        # TOOL ARGUMENT
        case "-m":
            set arg_tool = "ModelSim";
            echo "      $argv[$cur_arg] - ModelSim tool"
            breaksw
        case "modelsim"
            set arg_tool = "ModelSim";
            echo "      $argv[$cur_arg] - ModelSim tool"
            breaksw
        # WAVEFORM ARGUMENT
        case "-w":
            @ arg_wave = 1;
            echo "      $argv[$cur_arg] - Waveform"
            breaksw
        case "waveform":
            @ arg_wave = 1;
            echo "      $argv[$cur_arg] - Waveform"
            breaksw
        # VERBOSE ARGUMENT
        case "-v":
            @ arg_verb = 1;
            echo "      $argv[$cur_arg] - Verbose"
            breaksw
        case "verbose":
            @ arg_verb = 1;
            echo "      $argv[$cur_arg] - Verbose"
            breaksw
        # TESTCASE ARGUMENT
        default:
            if (-e ../../../bench/verilog/testcases/$argv[$cur_arg].v) then
                set arg_test = $argv[$cur_arg];
                echo "      $argv[$cur_arg] - Testcase"
        # INVALID ARGUMENT
            else
                echo ""
                echo "    Invalid verification argument: $argv[$cur_arg]"
                goto help
            endif
            breaksw
        endsw

        @ cur_arg++
    end

endif


# SIMULATION LOOP
##################

set cur_test_num = 0;

simulate:


    # DELETING FILES
    #################
    
    # Prepared files
    if (-e ./file_list.lst) then
        rm -rf ./file_list.lst
    endif
    if (-e ../bin/cds.lib) then
        rm -rf ../bin/cds.lib
    endif
    if (-e ../bin/hdl.var) then
        rm -rf ../bin/hdl.var
    endif
    if (-e ./compile.args) then
        rm -rf ./compile.args
    endif
    if (-e ./elab.args) then
        rm -rf ./elab.args
    endif
    if (-e ./sim.args) then
        rm -rf ./sim.args
    endif
    if (-e ./sim.tcl) then
        rm -rf ./sim.tcl
    endif
    if (-e ./sim.do) then
        rm -rf ./sim.do
    endif
    
    # Projects, Libraries and Logs
    if (-e ./uart.mpf) then
        rm -rf ./uart.mpf
    endif
    if (-e ./work) then
        rm -rf ./work
    endif
    if (-e ./INCA_libs/worklib) then
        rm -rf ./INCA_libs/worklib
    endif


    # PREPARING FILE LIST
    ######################
    
    # Design files
    echo "../../../rtl/verilog/uart_top.v"                    >> ./file_list.lst
    echo "../../../rtl/verilog/uart_wb.v"                     >> ./file_list.lst
    echo "../../../rtl/verilog/uart_transmitter.v"            >> ./file_list.lst
    echo "../../../rtl/verilog/uart_receiver.v"               >> ./file_list.lst
    echo "../../../rtl/verilog/uart_tfifo.v"                  >> ./file_list.lst
    echo "../../../rtl/verilog/uart_rfifo.v"                  >> ./file_list.lst
    echo "../../../rtl/verilog/uart_regs.v"                   >> ./file_list.lst
    echo "../../../rtl/verilog/uart_debug_if.v"               >> ./file_list.lst

    # Testcase file
    if ($arg_test == 0) then
        set i = 0;
        foreach testcase (../../../bench/verilog/testcases/uart*.v)
            if ($i == $cur_test_num) then
                set testcase_i = $testcase:t:r
            endif
            @ i++
        end
        set max_test_num = $i;
    else
        set testcase_i   = $arg_test;
        set max_test_num = 1;
    endif
    echo "//////////////////////////////////////////////////" >  ./file_list.lst
    echo "// File created within script ${0}"                 >> ./file_list.lst
    echo "//     path: $cwd"                                  >> ./file_list.lst
    echo "//     user: $user"                                 >> ./file_list.lst
    echo "//////////////////////////////////////////////////" >> ./file_list.lst
    echo "../../../bench/verilog/testcases/$testcase_i.v"     >> ./file_list.lst
    # Delete vawe out file for this testcase, if it already exists
    if (-e ../out/$testcase_i.wlf) then
        rm -rf ../out/$testcase_i.wlf
    endif
    # Delete log out file for this testcase, if it already exists
    if (-e ../log/$testcase_i.log) then
        rm -rf ../log/$testcase_i.log
    endif

    # Testbench files
    echo "../../../bench/verilog/uart_testbench.v"            >> ./file_list.lst
    echo "../../../bench/verilog/wb_master_model.v"           >> ./file_list.lst
    echo "../../../bench/verilog/uart_device.v"               >> ./file_list.lst
    echo "../../../bench/verilog/uart_testbench_utilities.v"  >> ./file_list.lst
    echo "../../../bench/verilog/uart_wb_utilities.v"         >> ./file_list.lst
    echo "../../../bench/verilog/uart_device_utilities.v"     >> ./file_list.lst


    # COMPILING & ELABORATING
    ##########################
    
    if ("$arg_tool" == "NCSim") then

        # cds.lib library file
        echo "//////////////////////////////////////////////////" >  ../bin/cds.lib
        echo "// File created within script ${0}"                 >> ../bin/cds.lib
        echo "//     path: $cwd"                                  >> ../bin/cds.lib
        echo "//     user: $0"                                    >> ../bin/cds.lib
        echo "//////////////////////////////////////////////////" >> ../bin/cds.lib
        echo "DEFINE worklib ./INCA_libs/worklib"                 >> ../bin/cds.lib
        
        # hdl.var variable file
        echo "//////////////////////////////////////////////////" >  ../bin/hdl.var
        echo "// File created within script ${0}"                 >> ../bin/hdl.var
        echo "//     path: $cwd"                                  >> ../bin/hdl.var
        echo "//     user: $0"                                    >> ../bin/hdl.var
        echo "//////////////////////////////////////////////////" >> ../bin/hdl.var
        echo "INCLUDE \$CDS_INST_DIR/tools/inca/files/hdl.var"    >> ../bin/hdl.var
        echo "DEFINE WORK worklib"                                >> ../bin/hdl.var
        
        # compile.args argument file
        echo "//////////////////////////////////////////////////" >  ./compile.args
        echo "// File created within script ${0}"                 >> ./compile.args
        echo "//     path: $cwd"                                  >> ./compile.args
        echo "//     user: $0"                                    >> ./compile.args
        echo "//////////////////////////////////////////////////" >> ./compile.args
        echo "-CDSLIB ../bin/cds.lib"                             >> ./compile.args
        echo "-HDLVAR ../bin/hdl.var"                             >> ./compile.args
        echo "-MESSAGES"                                          >> ./compile.args
        echo "-NOCOPYRIGHT"                                       >> ./compile.args
        echo "-INCDIR ../../../rtl/verilog"                       >> ./compile.args
        echo "-INCDIR ../../../bench/verilog"                     >> ./compile.args
        echo "-INCDIR ../../../bench/verilog/testcases"           >> ./compile.args
        if ($arg_verb == 1) then
          echo "-DEFINE VERBOSE"                                  >> ./compile.args
        endif
        cat ./file_list.lst                                       >> ./compile.args

        # compiling
        ncvlog -LOGFILE ../log/$testcase_i.compile.log -f ./compile.args #> /dev/null
        
        # elab.args argument file
        echo "//////////////////////////////////////////////////" >  ./elab.args
        echo "// File created within script ${0}"                 >> ./elab.args
        echo "//     path: $cwd"                                  >> ./elab.args
        echo "//     user: $0"                                    >> ./elab.args
        echo "//////////////////////////////////////////////////" >> ./elab.args
        echo "-CDSLIB ../bin/cds.lib"                             >> ./elab.args
        echo "-HDLVAR ../bin/hdl.var"                             >> ./elab.args
        echo "-MESSAGES"                                          >> ./elab.args
        echo "-NOCOPYRIGHT"                                       >> ./elab.args
        echo "-NOTIMINGCHECKS"                                    >> ./elab.args
        echo "-SNAPSHOT worklib.testbench:rtl"                    >> ./elab.args
        echo "-NO_TCHK_MSG"                                       >> ./elab.args
        echo "-ACCESS +RWC"                                       >> ./elab.args
        echo "worklib.$sim_top"                                   >> ./elab.args

        # elaborating
        ncelab -LOGFILE ../log/$testcase_i.elab.log -f ./elab.args #> /dev/null
    else
    
        # compile.args argument file
        echo "+libext+.v"                               >> ./compile.args
        echo "-y ../../../rtl/verilog"                  >> ./compile.args
        echo "-y ../../../bench/verilog"                >> ./compile.args
        echo "-y ../../../bench/verilog/testcases"      >> ./compile.args
        echo "-work ./work"                             >> ./compile.args
        echo "+incdir+../../../rtl/verilog"             >> ./compile.args
        echo "+incdir+../../../bench/verilog"           >> ./compile.args
        echo '+define+LOG_DIR=\"../log/$testcase_i\"'   >> ./compile.args
        if ($arg_verb == 1) then
          echo "+define+VERBOSE"                        >> ./compile.args
        endif
        cat ./file_list.lst                             >> ./compile.args
        
        # open project
#        echo "project new ./ testbench ./work"           >> ./sim.do
        vlib -dos ./work
        
        # compiling
 #       echo "vlog -f ./compile.args"                   >> ./sim.do
        vlog -f ./compile.args
    endif


    # SIMULATING
    #############

    if ("$arg_tool" == "NCSim") then

        # sim.args argument file
        echo "//////////////////////////////////////////////////" >  ./sim.args
        echo "// File created within script ${0}"                 >> ./sim.args
        echo "//     path: $cwd"                                  >> ./sim.args
        echo "//     user: $0"                                    >> ./sim.args
        echo "//////////////////////////////////////////////////" >> ./sim.args
        echo "-CDSLIB ../bin/cds.lib"                             >> ./sim.args
        echo "-HDLVAR ../bin/hdl.var"                             >> ./sim.args
        echo "-MESSAGES"                                          >> ./sim.args
        echo "-NOCOPYRIGHT"                                       >> ./sim.args
        echo "-INPUT ./sim.tcl"                                   >> ./sim.args
        echo "worklib.testbench:rtl"                              >> ./sim.args

        # sim.tcl file
        echo "//////////////////////////////////////////////////"              >  ./sim.tcl
        echo "// File created within script ${0}"                              >> ./sim.tcl
        echo "//     path: $cwd"                                               >> ./sim.tcl
        echo "//     user: $0"                                                 >> ./sim.tcl
        echo "//////////////////////////////////////////////////"              >> ./sim.tcl
        if ($arg_wave) then
            echo "database -open waves -shm -into ../out/waves.shm"            >> ./sim.tcl
            echo "probe -create -database waves $sim_top -shm -all -depth all" >> ./sim.tcl
            echo "run"                                                         >> ./sim.tcl
        else
            echo "run"                                                         >> ./sim.tcl
        endif
        echo     "quit"                                                        >> ./sim.tcl

        # simulating
        ncsim -LICQUEUE -LOGFILE ../log/$testcase_i.sim.log -f ./sim.args
    else
    
        # sim.do do file
        echo "vsim work.testbench work.testbench_utilities work.uart_wb_utilities work.uart_device_utilities work.testcase -wlf ../out/$testcase_i.wlf" >> ./sim.do
        if ($arg_wave) then
          echo "log -r -internal -ports /testbench/*"                          >> ./sim.do
        endif
        echo "run -all"                                                        >> ./sim.do

        vsim -c -do ./sim.do
             
    endif

    @ cur_test_num++
    
    if ($cur_test_num < $max_test_num) then
        goto simulate
    endif

exit


# HELP DISPLAY
###############

help:
    echo ""
    echo "    Valid verification arguments:"
    echo "      'help'     / '-h' : This help is displayed"
    echo "      'modelsim' / '-m' : ModelSim simulation tool is used, otherwise"
    echo "                          NCSim is used (default)"
    echo "      'waveform' / '-w' : Waveform output is recorded, otherwise"
    echo "                          NO waveform is recorded (default)"
    echo "      'verbose'  / '-v' : Verbose display on monitor, otherwise"
    echo "                          basic display on monitor (default)"
    echo "      '\042testcase\042'      : Testcase which is going to be simulated, otherwise"
    echo "                          ALL testcases are simulated - regression (default);"
    echo "                          Available testcases:"
    foreach testcase (../../../bench/verilog/testcases/uart*.v)
        echo "                            "$testcase:t:r
    end
    echo ""
exit

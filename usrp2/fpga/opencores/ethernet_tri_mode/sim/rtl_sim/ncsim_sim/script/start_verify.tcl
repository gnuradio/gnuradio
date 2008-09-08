proc start_verify {batch index} {
variable compiling
variable simulate_times
variable test_case_number
variable batch_data {}
variable batchmode
variable piple
###############################################################################
    set batchmode $batch
    toplevel .l
    focus .l
    wm title .l "Simulation Log"
    frame .l.f1 
    frame .l.f2
    pack .l.f1 .l.f2  -side top
     
    text .l.f1.t1 -width 80 -height 40 -yscrollcommand {.l.f1.scroll set}
    
    scrollbar .l.f1.scroll -command {.l.f1.t1 yview}
    button .l.f2.b1 -text "Exit" -command {destroy .l} -width 10
    button .l.f2.b2 -text "Stop" -command {Stop_sim} -width 10
    pack .l.f1.scroll -side right -fill y
    pack .l.f1.t1 
    pack .l.f2.b1 .l.f2.b2 -side left
    
    set output_win .l.f1.t1
    set simulate_times 0
    set test_case_number [lindex $index 0]    
    cd ../bin
    set compiling 1
    if {$batch==0} {
        Run "bash sim.nc " $output_win
    } elseif {[catch {open ../data/batch.dat r} fileid]} {
        $output_win insert end  "Failed open ../data/batch.dat file\n" 
    } elseif {[lindex $index 0] == "empty"} {
        $output_win insert end  "Not any test case was selected\n"
    } else {
        set i 0
        gets $fileid line
        while {[eof $fileid]==0} { 
           lappend batch_data $line
           gets $fileid line
           incr i
        }
        $output_win insert end "Verifcation Started\n"
        $output_win insert end "Starting Compiling Source file and libs......\n"
        Run "bash com.nc" $output_win
        close $fileid
    }
}

proc Run {command output_win} {
    global piple
    if [catch {open "|$command |& cat "} piple] {
        $output_win insert end $piple\n
    } else {
        fileevent $piple readable [list Log $piple $output_win]
    }
}

proc Log {piple output_win} {
    global compiling
    global batchmode
    set saparator "###################################################################\n"
    if {[eof $piple]} {
        if {$batchmode==0} {
            $output_win insert end $saparator  
            $output_win insert end "end of Simulation....\n"
            $output_win insert end $saparator         
        } elseif {$compiling==1} {
            $output_win insert end $saparator  
            $output_win insert end "end of Compiling....\n"
            $output_win insert end $saparator 
            $output_win insert end "\n \n \n" 
            set compiling 0
            sim_next_case 
        } else {
            $output_win insert end $saparator  
            $output_win insert end "end of Testcase....\n"
            $output_win insert end $saparator 
            $output_win insert end "\n \n \n"  
            sim_next_case        
        }
        close $piple
    } else {
        gets $piple line
        $output_win insert end $line\n
        $output_win see end
    }
}   
        
proc sim_next_case {} {
    global test_case_number
    global simulate_times
    global batch_data
    global index
    set saparator "###################################################################\n"    
    set output_win .l.f1.t1
    set test_case_number [lindex $index $simulate_times] 
    if {$simulate_times < [llength $index]} {
        set line [lindex $batch_data $test_case_number]
        set source_sti [lindex $line 1]
        set source_reg [lindex $line 2] 
        
        if {$simulate_times==0 ||[file exists "../log/.sim_succeed"]} {
            exec cp "../data/$source_sti" "../data/config.ini"
            exec cp "../data/$source_reg" "../data/CPU.vec"
            
            $output_win insert end $saparator  
            $output_win insert end "starting test case :[lindex $line 0]....\n"
            $output_win insert end $saparator  
            $output_win insert end "\n \n \n"  
            incr simulate_times
            Run "bash sim_only.nc" $output_win
        } else {
            $output_win insert end "test_case :[lindex $line 0] simulation failed!!\n" 
        }    
            
    } else {
        $output_win insert end "All test_case passed simulation Successfully!!\n"      
    }           
}

proc Stop_sim {} {
    global piple 
    catch {close $piple}
}
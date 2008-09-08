proc batch_mode {} {
variable i
variable index empty
variable batch_data
set namelist \
{- Description Stimulus RegVector}
###############################################################################    
    toplevel .b
    wm title .b "Setting Reg Data"
    focus .b
    frame .b.f100 

    set lth1 20
    set lth2 15
    set lth3 15
    cd ../data
    if {[catch {open batch.dat r} fileid]} {
        break
    } else {
        set batch_data {} 
        gets $fileid line 
        while {[eof $fileid]==0} {
            lappend batch_data $line
            if {[string length [lindex $line 0]] > $lth1 } {
                set lth1 [string length [lindex $line 0]]
            }
            if {[string length [lindex $line 1]] > $lth2 } {
                set lth2 [string length [lindex $line 1]]
            }
            if {[string length [lindex $line 2]] > $lth3 } {
                set lth3 [string length [lindex $line 2]]
            }
            gets $fileid line 
        }  
        close $fileid      
    }
    
    label .b.f100.l1 -text [lindex $namelist 0] -fg blue -width 5           
    label .b.f100.l2 -text [lindex $namelist 1] -fg blue -width [expr $lth1 +5] 
    label .b.f100.l3 -text [lindex $namelist 2] -fg blue -width [expr $lth2 +5] 
    label .b.f100.l4 -text [lindex $namelist 3] -fg blue -width [expr $lth3 +5] 
    pack .b.f100
    pack .b.f100.l1 .b.f100.l2 .b.f100.l3 .b.f100.l4 -side left   
    
        set i 0
        foreach line $batch_data {
            frame .b.f$i
            set tmp [expr $i+1]
            checkbutton .b.f$i.c -variable check$i -text $tmp   -width 5 
            label       .b.f$i.l1 -text [lindex $line 0] -width $lth1 
            button      .b.f$i.b1 -text "Change" -width 5 -command [list ChangeDescript .b.f$i.l1 $i 0]
            label       .b.f$i.l2 -text [lindex $line 1] -width $lth2
            button      .b.f$i.b2 -text "Change" -width 5 -command [list ChangeLabel .b.f$i.l2 $i 1]
            label       .b.f$i.l3 -text [lindex $line 2] -width $lth3
            button      .b.f$i.b3 -text "Change" -width 5 -command [list ChangeLabel .b.f$i.l3 $i 2]
            pack .b.f$i
            pack .b.f$i.c .b.f$i.l1 .b.f$i.b1 .b.f$i.l2 .b.f$i.b2  .b.f$i.l3 .b.f$i.b3 -side left
            incr i
        }


    frame .b.f102
    button .b.f102.b1 -text "Start Verify"  -width 10 
    button .b.f102.b2 -text "Save"          -width 10 -command {save_batch_data}
    button .b.f102.b3 -text "Exit"          -width 10 -command {destroy .b}
    pack .b.f102
    pack .b.f102.b1 .b.f102.b2 .b.f102.b3 -side left
    bind .b.f102.b1 <ButtonPress> {create_index $i}
    bind .b.f102.b1 <ButtonRelease> {start_verify 1 $index}            
} 

proc create_index {i} {
    global index
    set index empty
    set sim_times 0 
    save_batch_data
    for {set c 0} {$c < $i} {incr c} {
        upvar 1 check$c tmp
        if {$tmp==1 && $sim_times==0} {
            set index $c
            incr sim_times
        } elseif {$tmp==1} {
            lappend index $c
            incr sim_times
        }
    }       
} 

proc ChangeLabel {target_label y x} {
    global batch_data
    global fileselect
    fileselect
    tkwait window .fileSelectWindow
    if {$fileselect(canceled)==1} {
        return
    } elseif  {$fileselect(selectedfile)==""} {
        warning_message "file have not been selected!"
    } else {
        $target_label config -text $fileselect(selectedfile)
        set line [lindex $batch_data $y]
        set line [lreplace $line $x $x $fileselect(selectedfile)]
        set batch_data [lreplace $batch_data $y $y $line] 
    }
}

proc save_batch_data {} {
    global batch_data
    if {[catch {open batch.dat w} fileid]} {
        break
    } else {
        foreach line $batch_data {
            puts $fileid $line
        }
        close $fileid
    }
} 

proc ChangeDescript {target_label y x} {
    global batch_data
    variable value
    variable target
    variable newy
    variable newx
    toplevel .tmp
    focus .tmp
    set target $target_label
    set newy $y
    set newx $x
    entry .tmp.en -width 20 -textvariable value
    button .tmp.b -width 5 -text "Apply" -command {destroy .tmp}
    button .tmp.b1 -width 5 -text "quit"  
    bind .tmp.b <ButtonPress> {ChangeDescriptApply $target $newy $newx $value}

    
    pack .tmp.en .tmp.b   
}

proc ChangeDescriptApply {target_label y x value} {
    global batch_data 
    if {$value==""} {
        warning_message "Description can not be empty"
    } else {  
        $target_label config -text $value 
        set line [lindex $batch_data $y]
        set line [lreplace $line 0 0 $value]
        set batch_data [lreplace $batch_data $y $y $line] 
   } 
}         
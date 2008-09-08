proc set_reg_data {} {
variable help_strings
variable RegVector 
variable datalist
set help_strings {
Set Reg Data:
    The verifcation system will config DUT(Design Under Test) 
with those data automatically at startup time. You must read 
the tri-mode thernet spec-doc before setting the REG.Only the
correct configuration can make sitmulation continue.}   

set namelist {
RegName                Address default Data}  
set datalist {  
 
{Tx_Hwmark                      0     0x001e    0x001e}
{Tx_Lwmark                      1     0x0019    0x0019}
{pause_frame_send_en            2     0x0000    0x0000}
{pause_quanta_set               3     0x0000    0x0000}
{IFGset                         4     0x001e    0x001e}   
{FullDuplex                     5     0x0001    0x0001}
{MaxRetry                       6     0x0002    0x0002}
{MAC_tx_add_en                  7     0x0000    0x0000}
{MAC_tx_add_prom_data           8     0x0000    0x0000}
{MAC_tx_add_prom_add            9     0x0000    0x0000}
{MAC_tx_add_prom_wr            10     0x0000    0x0000}
{tx_pause_en                   11     0x0000    0x0000}
{xoff_cpu                      12     0x0000    0x0000}
{xon_cpu                       13     0x0000    0x0000}
{MAC_rx_add_chk_en             14     0x0000    0x0000}
{MAC_rx_add_prom_data          15     0x0000    0x0000}
{MAC_rx_add_prom_add           16     0x0000    0x0000}
{MAC_rx_add_prom_wr            17     0x0000    0x0000}
{broadcast_filter_en           18     0x0000    0x0000}
{broadcast_bucket_depth        19     0x0000    0x0000}
{broadcast_bucket_interval     20     0x0000    0x0000}
{RX_APPEND_CRC                 21     0x0000    0x0000}
{Rx_Hwmark                     22     0x001a    0x001a}
{Rx_Lwmark                     23     0x0010    0x0010}
{CRC_chk_en                    24     0x0000    0x0000}
{RX_IFG_SET                    25     0x001e    0x001e}
{RX_MAX_LENGTH                 26     0x2710    0x2710}
{RX_MIN_LENGTH                 27     0x0040    0x0040}
{CPU_rd_addr                   28     0x0000    0x0000}
{CPU_rd_apply                  29     0x0000    0x0000}
{Line_loop_en                  33     0x0000    0x0000}
{Speed                         34     0x0004    0x0004}
}
 
cd ../data

  if {[catch {open CPU.vec r} fileid]} {
        set RegVector {}       
  } else {
      set RegVector {}  
      gets $fileid line
      while {[eof $fileid]==0} {
        scan $line "%02x%02x%04x" counter addr NewValue
        set y [search_add $addr]
        if {$y!=-1} {
            set x [lindex $datalist $y]
            set x [lreplace $x 3 3 [format "0x%04x" $NewValue]]
            set datalist [lreplace $datalist $y $y $x]
            lappend RegVector "$y  $NewValue"           
        }
        gets $fileid line
      }
      close $fileid   
      
  }

toplevel .p
wm title .p "Setting Reg Data"
focus .p
frame .p.f1 
frame .p.f2
pack .p.f1 .p.f2  -side top
 
frame .p.f1.f1
frame .p.f1.f2
frame .p.f1.f3
frame .p.f1.f4
frame .p.f1.f5
pack .p.f1.f1 .p.f1.f2 .p.f1.f3 .p.f1.f4  .p.f1.f5 -side left

label .p.f1.f1.l100 -text [lindex $namelist 0] -fg blue 
label .p.f1.f2.l100 -text [lindex $namelist 1] -fg blue -width 10
label .p.f1.f3.l100 -text [lindex $namelist 2] -fg blue -width 5
label .p.f1.f4.l100 -text [lindex $namelist 3] -fg blue -width 10

pack .p.f1.f1.l100 .p.f1.f2.l100 .p.f1.f3.l100 .p.f1.f4.l100
 
for {set i 0} {$i < [llength $datalist]} {incr i} {
    label .p.f1.f1.l$i -text [lindex [lindex $datalist $i] 0] 
    label .p.f1.f2.l$i -text [lindex [lindex $datalist $i] 1]
    label .p.f1.f3.l$i -text [lindex [lindex $datalist $i] 2] 
    entry .p.f1.f4.en$i -width 10
    .p.f1.f4.en$i insert 0 [lindex [lindex $datalist $i] 3]
    if {[lindex [lindex $datalist $i] 2] !=[lindex [lindex $datalist $i] 3]} {
        .p.f1.f4.en$i config -fg red
    }
    pack .p.f1.f1.l$i .p.f1.f2.l$i .p.f1.f3.l$i .p.f1.f4.en$i
    bind .p.f1.f4.en$i <FocusOut> [list CheckModify .p.f1.f4.en$i $i ]    
}


button .p.f2.b1 -text "Save" -width 10 -command {WriteVect CPU.vec}
button .p.f2.b2 -text "SaveAs" -width 10 -command {WriteVect [reg_save_as]}
button .p.f2.b3 -text "Exit" -width 10 -command {destroy .p} 
button .p.f2.b4 -text "Help" -width 10 -command {print_help $help_strings}
pack .p.f2.b1 .p.f2.b2 .p.f2.b3 .p.f2.b4 -side left
bind .p.f2.b1 <ButtonPress> {focus .p}


}

proc CheckModify {target_en index} {
    global RegVector
    global datalist
    set data [$target_en get]     
    set NewValue "$index $data"   

    
    if {[info exists RegVector]} {
        set counter [llength $RegVector]
        for {set c 0} {$c<$counter} {incr c} {
            if {[lindex [lindex $RegVector $c] 0]== $index} {
                if {[lindex [lindex $datalist $index] 2]==$data} { 
                    $target_en config -fg black
                    set RegVector [lreplace  $RegVector $c $c] 
                } else {
                    set RegVector [lreplace  $RegVector $c $c $NewValue]
                }  
                return
            }
        }
    }
    if {[lindex [lindex $datalist $index] 3]==$data} {
        return
    } else {
        $target_en config -fg red
        set RegVector [lappend RegVector $NewValue]  
    }  
}

proc WriteVect {filename} {
    global RegVector
    global datalist
    if {[info exists RegVector]} {
        set counter [llength $RegVector]
        if {[catch {open $filename w} fileid]} {
            puts {failed to open vector file "$filename"}
            destrop .p
        } else {
        for {set i 0} {$i < $counter} {incr i} {
            set addr [lindex [lindex $datalist [lindex [lindex $RegVector $i] 0]] 1]
            set NewValue [lindex [lindex $RegVector $i] 1]
            puts $fileid [format "%02x%02x%04x" $counter $addr $NewValue]
            }
        close $fileid
        unset RegVector
        destroy .p
        }  
    } else {
    destroy .p
    } 
}

proc reg_save_as {} {
    global fileselect
    fileselect
    tkwait window .fileSelectWindow
    if {$fileselect(canceled)==1} {
        return
    } elseif  {$fileselect(selectedfile)==""} {
        warning_message "file have not been selected!"
    } else {
        return $fileselect(selectedfile)
    }
}

proc search_add {addr} {
    global datalist
    set n 0
    foreach line $datalist {
        if {[lindex $line 1]==$addr} {
            return $n
        }
        incr n
    }
    return -1
} 
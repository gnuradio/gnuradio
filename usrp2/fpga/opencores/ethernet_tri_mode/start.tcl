variable MAC_SOURCE_REPLACE_EN   
variable MAC_TARGET_CHECK_EN     
variable MAC_BROADCAST_FILTER_EN 
variable MAC_TX_FF_DEPTH
variable MAC_RX_FF_DEPTH
variable header_data

source sim/rtl_sim/ncsim_sim/script/run_proc.tcl

frame .f1
frame .f2
text .f1.t1 -width 60 -height 20 
button .f2.b1 -text "Quit" -width 5 -command {exit}
button .f2.b2 -text "Next" -width 5 -command {module_conf}

pack .f1 .f2
pack .f1.t1
pack .f2.b1 .f2.b2 -side left

set strings \
{Hi guys,
    Thanks for your interest about this tri-speed ethernet MAC controller.
Since this project was created at 25-Nov-2005,I put almost all my free time
on this project.I am exhausted for this two month's extra work.If you think
this project is useful,let me know that and i will feel much better. 

    To increase the flexibility, some optional modules can be removed from 
the design to reduce area. 

    Any problem or bug report please contact me by email:
    gaojon@yahoo.com
                                              
                                              
                                              
                                              
                                              
                                              
                                                                jon
                                                                18-Jan-2006
}

.f1.t1 insert end $strings                                    

proc module_conf {} {    
    global MAC_SOURCE_REPLACE_EN     
    global MAC_TARGET_CHECK_EN       
    global MAC_BROADCAST_FILTER_EN   
    global MAC_TX_FF_DEPTH           
    global MAC_RX_FF_DEPTH     
    global header_data      

    if {[catch {open ./rtl/verilog/header.v r} fileid]} {
        puts "Failed open ./rtl/verilog/header.v file\n"
    } else {
        gets $fileid line
        if {[lindex $line 0]=="//"} {
            set line [lreplace $line 0 0]
            set MAC_SOURCE_REPLACE_EN 0
        } else {
            set MAC_SOURCE_REPLACE_EN 1
        } 
        lappend header_data $line
        gets $fileid line
        if {[lindex $line 0]=="//"} {
            set line [lreplace $line 0 0]
            set MAC_TARGET_CHECK_EN 0
        } else {
            set MAC_TARGET_CHECK_EN 1
        } 
        lappend header_data $line
        gets $fileid line
        if {[lindex $line 0]=="//"} {
            set line [lreplace $line 0 0]
            set MAC_BROADCAST_FILTER_EN 0
        } else {
            set MAC_BROADCAST_FILTER_EN 1
        }
        lappend header_data $line
        gets $fileid line
        set MAC_TX_FF_DEPTH [lindex $line 2]
        lappend header_data $line
        gets $fileid line
        set MAC_RX_FF_DEPTH [lindex $line 2]
        lappend header_data $line
        close $fileid
     }
                        
    destroy .f1 .f2
    frame .f1 
    frame .f2
    frame .f1.f1
    frame .f1.f2
    frame .f1.f3
    frame .f1.f4
    frame .f1.f5
    frame .f1.f6
    
    pack .f1 .f2 
    pack .f1.f1 .f1.f2 .f1.f3 .f1.f4 .f1.f5 .f1.f6
    label       .f1.f1.lb -text "enable source MAC replace module" -width 30
    checkbutton .f1.f1.cb -variable MAC_SOURCE_REPLACE_EN
    label       .f1.f2.lb -text "enable target MAC check module " -width 30
    checkbutton .f1.f2.cb -variable MAC_TARGET_CHECK_EN
    label       .f1.f3.lb -text "enable broadcast packet filter module" -width 30
    checkbutton .f1.f3.cb -variable MAC_BROADCAST_FILTER_EN
    
    label       .f1.f4.lb -text "MAC_TX_FF_DEPTH" -width 30
    entry       .f1.f4.en -textvariable MAC_TX_FF_DEPTH -width 5
    label       .f1.f5.lb -text "MAC_RX_FF_DEPTH" -width 30
    entry       .f1.f5.en -textvariable MAC_RX_FF_DEPTH -width 5
    
    button .f2.b1 -width 10 -text "Save"            -command {save_header}
    button .f2.b2 -width 10 -text "Verify"          -command {run_sim}
    button .f2.b4 -width 10 -text "Exit"            -command {exit}
    
    pack .f1.f1.cb .f1.f1.lb -side right 
    pack .f1.f2.cb .f1.f2.lb -side right 
    pack .f1.f3.cb .f1.f3.lb -side right 
    pack .f1.f4.en .f1.f4.lb -side right 
    pack .f1.f5.en .f1.f5.lb -side right 
    
    pack .f2.b1 .f2.b2 .f2.b4 -side left

}     

proc save_header {} {
    global MAC_SOURCE_REPLACE_EN     
    global MAC_TARGET_CHECK_EN       
    global MAC_BROADCAST_FILTER_EN   
    global MAC_TX_FF_DEPTH           
    global MAC_RX_FF_DEPTH     
    global header_data 
    if {[catch {open ./rtl/verilog/header.v w} fileid]} {
        puts "Failed open ./rtl/verilog/header.v file\n"
    } else {
        set line [lindex $header_data 0]
        if {$MAC_SOURCE_REPLACE_EN==0} {
            set line [linsert $line 0 "//"]
        } 
        puts $fileid $line

        set line [lindex $header_data 1]        
        if {$MAC_TARGET_CHECK_EN==0} {
            set line [linsert $line 0 "//"]
        } 
        puts $fileid $line

        set line [lindex $header_data 2]
        if {$MAC_BROADCAST_FILTER_EN==0} {
            set line [linsert $line 0 "//"]
        } 
        puts $fileid $line
        
        set line [lindex $header_data 3]
        set line [lreplace $line 2 2 $MAC_TX_FF_DEPTH]
        puts $fileid $line
        
        set line [lindex $header_data 4]
        set line [lreplace $line 2 2 $MAC_RX_FF_DEPTH]
        puts $fileid $line                
        
        close $fileid
     }        
            
}

proc start_verify {} {
    cd sim/rtl_sim/ncsim_sim/script
    vish run.tcl
}

proc start_syn {} {
    cd syn
    synplify_pro syn.prj
}

proc run_sim {} {
    cd sim/rtl_sim/ncsim_sim/script/
    run_proc
}
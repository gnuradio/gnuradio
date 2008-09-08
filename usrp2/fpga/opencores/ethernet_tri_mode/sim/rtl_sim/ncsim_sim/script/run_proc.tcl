proc run_proc {} {
source user_lib.tcl
source set_stimulus.tcl
source set_reg_data.tcl
source start_verify.tcl
source batch_mode.tcl
source filesel.tcl

toplevel .frame
focus .frame
wm title .frame "main"
button .frame.b1 -width 20 -text "set_stimulus" 
button .frame.b2 -width 20 -text "set_cpu_data"
button .frame.b3 -width 20 -text "start_verify"
button .frame.b4 -width 20 -text "batch_mode"
button .frame.b40 -width 20 -text "exit" -command {cd ../../../../ ;destroy .frame}

bind .frame.b1 <Button-1> {set_stimulus}
bind .frame.b2 <Button-1> {set_reg_data}
bind .frame.b3 <Button-1> {start_verify 0 empty}
bind .frame.b4 <Button-1> {batch_mode}



pack .frame.b1 .frame.b2 .frame.b3 .frame.b4 .frame.b40

} 
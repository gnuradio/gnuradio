proc set_stimulus {} {
    toplevel .stimulus   
    focus .stimulus
    wm title .stimulus "Setting Stimulus Prameters"
    
    variable StartLength  
    variable EndLength
    variable PacketNumber
    variable Random
    variable Broadcast
    variable help_string
    
set help_string {
Set Stimulus Help:
    You can select to generate packet with sequential packet length or 
random packet length. If you choise random packet length, the length of
generated packet  will be a random value between the "Packet begin length" 
to the "Packet end length". "Total  Gen Packet number"is used to set the
number of packet will be generated as stimulus
   As well, you can select "sequence" mode . The first Packet will be
generated with "Packet begin length",and the next Packet length will be
"Packet begin length" pluse one untile the packet length reach the value
of "Packet end length". Packet in each length will be generated according
to the value of "Packet number per length" }    
    
    
  if {[catch {open ../data/config.ini r} fileid]} {
      set StartLength 46
      set EndLength 60   
      set PacketNumber 1
      set Random 0
      set Broadcast 0
      } \
      else {
      gets $fileid content
      scan $content "%d,%d,%d,%d,%d" StartLength EndLength  PacketNumber Random Broadcast
      close $fileid
       }

    frame .stimulus.f1 
    label .stimulus.f1.lb -text "Packet begin length:"
    entry .stimulus.f1.en -textvariable StartLength 
    pack .stimulus.f1 -fill x
    pack .stimulus.f1.en .stimulus.f1.lb  -side right

    frame .stimulus.f2 
    label .stimulus.f2.lb -text "Packet end length:"
    entry .stimulus.f2.en -textvariable EndLength 
    pack .stimulus.f2 -fill x
    pack .stimulus.f2.en .stimulus.f2.lb   -side right

    
    frame .stimulus.f3 
    label .stimulus.f3.lb -text "Packet number per length:"
    entry .stimulus.f3.en -textvariable PacketNumber 
    pack .stimulus.f3 -fill x
    pack .stimulus.f3.en .stimulus.f3.lb  -side right

    frame .stimulus.f4
    radiobutton .stimulus.f4.1 -text "Sequence" -variable Random -value 0 \
                -command {.stimulus.f3.lb config -text "Packet number per length:"}
    radiobutton .stimulus.f4.2 -text "Random" -variable Random -value 1 \
                -command {.stimulus.f3.lb config -text "Total  Gen Packet number:"}
    pack .stimulus.f4 -fill x 
    pack .stimulus.f4.1 .stimulus.f4.2 -side right
 
    frame .stimulus.f5
    checkbutton .stimulus.f5.1 -text "Broadcast" -variable Broadcast
    pack .stimulus.f5 -fill x 
    pack .stimulus.f5.1 -side right
         
    frame .stimulus.f20 
    button .stimulus.f20.1 -text "Save" -width 10 \
        -command {  set fileid [open ../data/config.ini w 0600]
                puts $fileid "$StartLength,$EndLength,$PacketNumber,$Random,$Broadcast"
                close $fileid
                destroy .stimulus
                }
    button .stimulus.f20.2 -text "Save as" -command {sti_save_as $StartLength $EndLength $PacketNumber $Random $Broadcast} -width 10
    button .stimulus.f20.3 -text "Cancel" -command {destroy .stimulus} -width 10
    button .stimulus.f20.4 -text "Help" -width 10 -command {print_help $help_string}
    pack .stimulus.f20 -fill x
    pack .stimulus.f20.1 .stimulus.f20.2 .stimulus.f20.3 .stimulus.f20.4 -side left
}

proc sti_save_as {StartLength EndLength PacketNumber Random Broadcast} {
    global fileselect
    cd ../data
    fileselect
    tkwait window .fileSelectWindow
    if {$fileselect(canceled)==1} {
        return
    } elseif {$fileselect(selectedfile)==""} {
        warning_message "file have not been selected!"
    } else {
        set fileid [open $fileselect(selectedfile) w 0600]
        puts $fileid "$StartLength,$EndLength,$PacketNumber,$Random,$Broadcast"
        close $fileid
        destroy .stimulus
    }
}    
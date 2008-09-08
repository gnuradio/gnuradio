proc print_help {help_string} {
    toplevel .help 
    frame .help.1 
    message .help.1.1 -text $help_string -width 10c  
    frame .help.2 
    button .help.2.1 -text "exit" -command {destroy .help} -width 10
    pack .help.1 .help.2 .help.1.1 .help.2.1
}

proc warning_message {warning_string} {
    toplevel .warning 
    frame .warning.1 
    message .warning.1.1 -text $warning_string -width 10c  
    frame .warning.2 
    button .warning.2.1 -text "exit" -command {destroy .warning} -width 10
    pack .warning.1 .warning.2 .warning.1.1 .warning.2.1
}
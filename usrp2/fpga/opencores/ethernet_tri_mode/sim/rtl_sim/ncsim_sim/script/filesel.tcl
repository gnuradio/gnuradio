#
# fileselect.tcl --
# simple file selector.
#
# Mario Jorge Silva			          msilva@cs.Berkeley.EDU
# University of California Berkeley                 Ph:    +1(510)642-8248
# Computer Science Division, 571 Evans Hall         Fax:   +1(510)642-5775
# Berkeley CA 94720                                 
# 

# Layout:
#
#  file:                  +----+
#  ____________________   | OK |
#                         +----+
#
#  +------------------+    Cancel
#  | ..               |S
#  | file1            |c
#  | file2            |r
#  |                  |b
#  | filen            |a
#  |                  |r
#  +------------------+
#  currrent-directory
#
# Copyright 1993 Regents of the University of California
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.  The University of California
# makes no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty.
#

# Copyright 1996

# Slight modifications to and adoption to Tk4.0 were made to this
# fileselectionbox code by Lakshmi Sastry, Rutherford Appleton Laboratory,
# chilton, Didcot, OXON, OX11 0QX, UK.

# You can now type in a non-existing file name as well. This file name is
# returned for the application to open a new file to write to 

# AGOCG Tcl/Tk Cookbook
# Authors

# Lakshmi Sastry
# Computing and Information Systems Department
# Rutherford Appleton Laboratory, Chilton, Didcot. OX11 0QX
# lakshmi.sastry@rl.ac.uk

#                         and

# Venkat VSS Sastry
# Department of Applied Mathematics and Operational Research
# Cranfield University, RMCS Shrivenham, Swindon, SN6 8LA
# sastry@rmcs.cran.ac.uk

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and without
# fee is hereby granted, provided that this copyright
# notice appears in all copies.
  
# The authors, RAL, RMCS Shrivenham, Cranfield University and AGOCG
# make no representations about the suitability of this
# software for any purpose.  It is provided "as is" without
# express or implied warranty. Likewise they accept no responsibility
# whatsoever for any public domain software modules used (which are
# hereby acknowledged) in this software 


# names starting with "fileselect" are reserved by this module
# no other names used.

# use the "option" command for further configuration

option add *Listbox*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
option add *Entry*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
option add *Label*font \
    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile


# this is the default proc  called when "OK" is pressed
# to indicate yours, give it as the first arg to "fileselect"

proc fileselect.default.cmd {f} {
global fileselect
set fileselect(selectedfile) $f
set fileselect(canceled) 0
#puts stderr "selected file $f"
 
}


# this is the default proc called when error is detected
# indicate your own pro as an argument to fileselect

proc fileselect.default.errorHandler {errorMessage} {
    puts stdout "error: $errorMessage"
    catch { cd ~ }
}

# this is the proc that creates the file selector box

proc fileselect {
    {cmd fileselect.default.cmd} 
    {purpose "Open file:"} 
    {w .fileSelectWindow} 
    {errorHandler fileselect.default.errorHandler}} {

    catch {destroy $w}

    toplevel $w
    grab $w
    wm title $w "Select File"


    # path independent names for the widgets
    global fileselect
    global selected
    set fileselect(entry) $w.file.eframe.entry
    set fileselect(list) $w.file.sframe.list 
    set fileselect(scroll) $w.file.sframe.scroll
    set fileselect(ok) $w.bframe.okframe.ok
    set fileselect(cancel) $w.bframe.cancel
    set fileselect(dirlabel) $w.file.dirlabel

    # widgets
    frame $w.file -bd 10 
    frame $w.bframe -bd 10
    pack append $w \
        $w.file {left filly} \
        $w.bframe {left expand frame n}

    frame $w.file.eframe
    frame $w.file.sframe
    label $w.file.dirlabel -anchor e -width 24 -text [pwd] 

    pack append $w.file \
        $w.file.eframe {top frame w} \
	$w.file.sframe {top fillx} \
	$w.file.dirlabel {top frame w}


    label $w.file.eframe.label -anchor w -width 24 -text $purpose
    entry $w.file.eframe.entry -relief sunken 

    pack append $w.file.eframe \
		$w.file.eframe.label {top expand frame w} \
                $w.file.eframe.entry {top fillx frame w} 


    scrollbar $w.file.sframe.yscroll -relief sunken \
	 -command "$w.file.sframe.list yview"
    listbox $w.file.sframe.list -relief sunken -selectmode single \
	-yscroll "$w.file.sframe.yscroll set" 
    #$fileselect(list) configure -selectmode single
    pack append $w.file.sframe \
        $w.file.sframe.yscroll {right filly} \
 	$w.file.sframe.list {left expand fill} 

    # buttons
    frame $w.bframe.okframe -borderwidth 2 -relief sunken
 
    button $w.bframe.okframe.ok -text OK -relief raised -padx 10 \
        -command "fileselect.ok.cmd $w $cmd $errorHandler"

    button $w.bframe.cancel -text cancel -relief raised -padx 10 \
        -command "fileselect.cancel.cmd $w"
    pack append $w.bframe.okframe $w.bframe.okframe.ok {padx 10 pady 10}

    pack append $w.bframe $w.bframe.okframe {expand padx 20 pady 20}\
                          $w.bframe.cancel {top}

    # Fill the listbox with a list of the files in the directory (run
    # the "/bin/ls" command to get that information).
    # to not display the "." files, remove the -a option and fileselect
    # will still work
 
    $fileselect(list) insert end ".."
    foreach i [exec ls -a [pwd]] {
        if {[string compare $i "."] != 0 && \
	    [string compare $i ".."] != 0 } {
            $fileselect(list) insert end $i
        }
    }

   # Set up bindings for the browser.
    bind $fileselect(entry) <Return> {eval $fileselect(ok) invoke} 
    bind $fileselect(entry) <Control-c> {eval $fileselect(cancel) invoke}

    bind $fileselect(list) <Button-1> {
        # puts stderr "button 1 release"
	set x [$fileselect(list) curselection]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
    }

    bind $fileselect(list) <Key> {
	set x [$fileselect(list) curselection]
        $fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
    }

    bind $fileselect(list) <Double-ButtonPress-1> {
        # puts stderr "double button 1"
       	set x [$fileselect(list) curselection]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
	$fileselect(ok) invoke
    }

    bind $fileselect(list) <Return> {
	set x [$fileselect(list) curselection]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
	$fileselect(ok) invoke
    }

    # set kbd focus to entry widget

    focus $fileselect(entry)

}


# auxiliary button procedures

proc fileselect.cancel.cmd {w} {
    # puts stderr "Cancel"
    global fileselect
    set fileselect(canceled) 1
    destroy $w
}

proc fileselect.ok.cmd {w cmd errorHandler} {
    global fileselect
   global selected
    set selected [$fileselect(entry) get]
        # some nasty file names may cause "file isdirectory" to return an error
    set sts [catch { 
	file isdirectory $selected
    }  errorMessage ]

    if { $sts != 0 } then {
	$errorHandler $errorMessage
	destroy $w
	return

    }

    # clean the text entry and prepare the list
      $fileselect(entry) delete 0 end
      $fileselect(list) delete 0 end
      $fileselect(list) insert end ".."

    # perform globbing on the selection. 
    # If globing returns an error (no match) check if a non-null name is
    # entered. If name string is non-empty return it as a new file name
    # else give an error message.
    # If resulting list length > 1, put the list on the file listbox and return
    # If globing expands to a list of filenames in multiple directories,
    # the indicated regexp is invalid and the error handler is called instead.
set globlist 0

    set sts [catch {
	set globlist [glob [list $selected]]
    } errorMessage ]

    if { $sts != 0 } then {
	if { [llength $globlist] == 1 } {
		destroy $w
		$cmd $selected
		return 
	} else {

	$errorHandler $errorMessage
	destroy $w
	return

   	 }

	
    } 

    if {[llength $globlist] > 1} {
	if {[regexp "/" $globlist] != 0} {
	    $errorHandler [list "Invalid regular expression, " $selected, "."]
	    destroy $w
	    return
	}
	foreach i $globlist {
	    if {[string compare $i "."] != 0 && \
		[string compare $i ".."] != 0} {
		$fileselect(list) insert end $i
	    }
	}
	return
    }

    # selection may be a directory. Expand it.

    if {[file isdirectory $selected] != 0} {
	cd $selected
	set dir [pwd]
	$fileselect(dirlabel) configure -text $dir

	foreach i [exec ls -a $dir] {
	    if {[string compare $i "."] != 0 && \
		[string compare $i ".."] != 0} {
		$fileselect(list) insert end $i
	    }
	}
	return
    }

    destroy $w
    $cmd $selected
	
}


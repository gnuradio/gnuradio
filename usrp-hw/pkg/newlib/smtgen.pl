#! /usr/bin/env perl
#
# smtgen.pl -- Perl script which generates PCB land patterns for two pad
#              passives.
#
# Copyright (C) 2005 Stuart D. Brorson.
#
#======================================================================
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
#======================================================================
#
#  Revision history
#  
#  20050117 -- Initial release.  SDB.
#
#======================================================================


use POSIX;

#  global variables
$VERBOSE = 0;                         # set to 1 for lots of spew
$l = $w = $x = $y = $g = -1;          # initialize geometry to unrealistic value.


# These numbers taken from the Cutler-Hammer design guidelines doc which is
# avaliable on the web at the San Diego chapter of the IPC: 
# http://dcchapters.ipc.org/SanDiego/library1.htm
# Note that they are very conservative numbers & produce large land patterns.
# You can replace them with number more to your liking if desired.
#
$SilkWidth = 12;                      # Silkscreen line width
$SilkClearance = 20+$SilkWidth/2;     # Clearance from end of pad metal to beginning of silk.
$MaskClearance = 8;                   # Clearance from end of pad metal to beginning of mask.
$PadPlaneClearance = 20;              # Clearance from end of pad metal to any 
                                      # surrounding metal planes. This can be made smaller if 
                                      # desired.

$description = "";                    # Text which is put into the footprint
$pcb_name = "";
$value = "";

$UsageString = "
===============================================================================
smtgen -- Copyright 2005 by Stuart Brorson.  This script is GPL'ed software
Please look at the source to read licencing details.

This script creates PCB footprint files for two pad SMT passives such as 0805 
resistors and caps.  You specify the actual pad dimensions on the command
line, and the program outputs (to STDOUT) the corresponding newlib footprint.
Redirect STDOUT to a file when you want to store your results.


Args:

  -v              (Optional) Use verbose mode when running (for debug purposes).
  --help          (Optional) Prints this help menu.
  y=<number>      (Mandatory) Set pad width to <number>.  Dimensions are mils.  Refer to 
                  pad drawing below for explanation of geometric parameters.
  x=<number>      (Mandatory) Set pad length to <number>.  Dimensions are mils.  Refer to 
                  pad drawing below for explanation of geometric parameters.
  g=<number>      (Mandatory) Set pad to pad distance to <number>.  Dimensions are mils.  
                  Refer to  pad drawing below for explanation of geometric parameters.
  l=<number>      (Optional) Set footprint length to <number>.  Dimensions are mils.  
                  Refer to pad drawing below for explanation of geometric parameters.
                  If this is not declared, then the silkscreen is placed one clearance distance
                  away from the pad edge.
  w=<number>      (Optional) Set footprint width to <number>.  Dimensions are mils.  
                  Refer to  pad drawing below for explanation of geometric parameters.
                  If this is not declared, then the silkscreen is placed one clearance distance
                  away from the pad edge.
  description     (Optional) String description which goes into Element declaration.  
  value           (Optional) String description which goes into Element declaration.  
  pcb_name        (Optional) String description (of refdes) which goes into Element declaration.  


Example usage:

perl smtgen.pl -v l=85 w=55 g=32 x=38 y=59 description=SMD_Res_0805

All values are in mil.  The pad is always drawn horizontally.  smtgen assumes 
the following geometrical description of the landing pattern:

 
     |<--------------------  l  -------------------->|
     |                                               |
     |                                               |
     |                                               |

 --- *************************************************
  ^  *                                               *
  |  *   --------------             --------------   * ---
  |  *   |            |             |            |   *  ^
     *   |            |             |            |   *  |
  w  *   |            |      +      |            |   *  x
     *   |            |             |          \\ |   *  |
  |  *   |            |             |           \\|   *  v
  |  *   --------------             -------------\\   * ---
  v  *                                            \\  *
 --- **********************************************\\**
                                                 \\  \\
         |            |             |            |\\  \\------  Pad.
         |            |             |            | \\ 
         |<---  y  -->|<--   g   -->|<---  y  -->|  \\
                                                     \\------  Silkscreen outline.


===============================================================================
";


#  Get args.
@ArgList = @ARGV;
ParseArgs(@ArgList);  #  Args are global variables, so they just appear after this call.


#################  Write out Element preamble  ####################
print("Element[0x00000000 \"$description\" \"$pcb_name\" \"$value\" 0 0 0 0 3 100 0x00000000]\n");
print("(\n");

print("#  Part input parameters: w=$w, l=$l, x=$x, y=$y, g=$g \n");

#################  This bit creates the two pads  ####################

if ($y > $x) {
  #  pad is longer than wide, so draw pad line horizontally
  print_verbose("# Pad is wider than high, so draw pad line horizontally.\n");
  $wp = $y - $x;                               # length of line defining pad
  $t = ceil($x);                               # pad thickness parameter

  #  Note that units are screwey here.  x1 & y1 are PCB's units
  #  units (x increases to the right, y increases down).  Above, x and y 
  #  were those defined by the IPC to describe footprints.
  ($x1, $y1) = ( -ceil( ($g/2 + $y/2)), 0);    # center point of left pad.
  ($x1L, $y1B) = ( ceil($x1-$wp/2), 0) ;        # start point of left pad.
  ($x1R, $y1T) = ( ceil($x1+$wp/2), 0 );        # end point of left pad.

  ($x2, $y2) = ( ceil(+($g/2 + $y/2)), 0);     # center point of right pad.
  ($x2L, $y2B) = ( ceil($x2-$wp/2), 0 );        # start point of left pad.
  ($x2R, $y2T) = ( ceil($x2+$wp/2), 0 );        # end point of left pad.
} else {
  #  pad is higher than wide, so draw pad line vertically
  print_verbose("# Pad is higher than wide, so draw pad line vertically.\n");
  $hp = $x - $y;                               # length of line defining pad
  $t = ceil($y);                               # pad thickness parameter

  ($x1, $y1) = ( ceil( -($g/2 + $y/2)), 0);    # center point of left pad.
  ($x1L, $y1B) = ( $x1, -ceil($hp/2)) ;         # start point of left pad.
  ($x1R, $y1T) = ( $x1,  ceil($hp/2));          # end point of left pad.

  ($x2, $y2) = ( ceil(+($g/2 + $y/2)), 0);     # center point of right pad.
  ($x2L, $y2B) = ( $x2, -ceil($hp/2)) ;         # start point of left pad.
  ($x2R, $y2T) = ( $x2,  ceil($hp/2));          # end point of left pad.
}

$Mask = $t + $MaskClearance;

print("#  Create two pads.\n");
print("   Pad($x1L $y1B $x1R $y1T $t $PadPlaneClearance $Mask \"Pad_1\" \"1\" 0x00000100)\n");
print("   Pad($x2L $y2B $x2R $y2T $t $PadPlaneClearance $Mask \"Pad_2\" \"2\" 0x00000100)\n");


#################  Create the four silk lines  ####################
# The silkscreen lines are either determined by the larger of:
# the body width (WxL), or
# the max extent of the pads + $SilkClearance

# X extents:
if (ceil($l/2) > ceil($g/2 + $y + $SilkClearance)) {
  #  User's l is larger.  Use it.
  print_verbose("# Using user's def for l.\n");
  $XL_M = - ceil($l/2);
  $XL_P = ceil($l/2);
} else {
  #  Silk defined by pad size is larger.
  print_verbose("# Using maximum pad size def for .\n");
  $XL_M = - ceil($g/2 + $y + $SilkClearance);
  $XL_P = ceil($g/2 + $y + $SilkClearance);
}

# Y extents:
if (ceil($w/2) > ceil($x/2 + $SilkClearance)) {
  #  User's w is larger.  Use it.
  print_verbose("# Using user's def for w.\n");
  $YL_M = - ceil($w/2);
  $YL_P = ceil($w/2);
} else {
  #  Silk defined by pad size is larger.
  print_verbose("# Using maximum pad size def for w.\n");
  $YL_M = - ceil($x/2 + $SilkClearance);
  $YL_P = ceil($x/2 + $SilkClearance);
}

print("#  Create package body outline on silk layer.\n");
print("   ElementLine($XL_M $YL_M $XL_P $YL_M $SilkWidth)\n");
print("   ElementLine($XL_M $YL_P $XL_P $YL_P $SilkWidth)\n");
print("   ElementLine($XL_M $YL_M $XL_M $YL_P $SilkWidth)\n");
print("   ElementLine($XL_P $YL_M $XL_P $YL_P $SilkWidth)\n");


#################  Write out closing paren  ####################
print(")\n");

exit(0);


#########################  Subs  ########################
sub ParseArgs {
#  This subroutine runs through the arg list submitted on the command line
#  and stuffs the values found into the appropriate variable.  If an
#  inappropriate value is found, it just prints out the usage info and
#  calls exit(-1).  Remember that all variables in Perl are global unless
#  otherwise declared; I take advantage of this feature heavily!  :-)
  my $Arg;

  # First figure out if we are in verbose mode.
  foreach $Arg (@_) {
    #match -v
    if ( $Arg =~ m/-v/ ) { 
      $VERBOSE = 1;
      break;
    }
  }

  #  OK, having settled that, we now process real args.
  foreach $Arg (@_) {
    print_verbose("#  Examining Arg = $Arg\n");

    # match w=
    if ( $Arg =~ m/w=[\d+][\.]*[\d]*/ ) { 
      ($ArgType, $w) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $w) in argument list\n");
    }

    # match l=
    elsif ( $Arg =~ m/l=[\d+][\.]*[\d]*/ ) { 
      ($ArgType, $l) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $l) in argument list\n");
    }

    # match g=
    elsif ( $Arg =~ m/g=[\d+][\.]*[\d]*/ ) { 
      ($ArgType, $g) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $g) in argument list\n");
    }

    #match x=
    elsif ( $Arg =~ m/x=[\d+][\.]*[\d]*/ ) { 
      ($ArgType, $x) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $x) in argument list\n");
    }

    #match y=
    elsif ( $Arg =~ m/y=[\d+][\.]*[\d]*/ ) { 
      ($ArgType, $y) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $y) in argument list\n");
    }

    #match description=
    elsif ( $Arg =~ m/description=\w/ ) { 
      ($ArgType, $description) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $description) in argument list\n");
    }

    #match pcb_name=
    elsif ( $Arg =~ m/pcb_name=\w/ ) { 
      ($ArgType, $pcb_name) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $pcb_name) in argument list\n");
    }

    #match value=
    elsif ( $Arg =~ m/value=\w/ ) { 
      ($ArgType, $value) = split(/=/, $Arg);
      print_verbose("#  Found ($ArgType, $value) in argument list\n");
    }

    #match -v
    elsif ( $Arg =~ m/-v/ ) { 
      break;
    }

    #match ---help
    elsif ( $Arg =~ m/--help/ ) { 
      print($UsageString);
      exit(0);
    }

    #match -h
    elsif ( $Arg =~ m/-h/ ) { 
      print($UsageString);
      exit(0);
    }


    #  Unknown token on input line.
    else {
      print("Unknown token on input line.  Required parameters are:\n");
      print("   x=<pad width (vertical) in mils>\n");
      print("   y=<pad length (horizontal) in mils>\n");
      print("   g=<pad to pad distance in mils>\n");
      print("Example usage:\n");
      print("   perl smtgen.pl x=30 y=25 g=25\n");
      print("type \"perl smtgen.pl --help\" for more information.\n\n");
      exit(-1)
    }
  }  # foreach

  if ( ($x == -1) || ($y == -1) || ($g == -1) ) {
    #  We didn't get enough information.  Tell user and exit.
    printf("You need to specify x, y, and g on the command line.  One\n");
    printf("or more of those parameters was missing.  Please try again.\n");
    exit(-1);
  }

  return

}

sub print_verbose() {
  if ($VERBOSE == 1) {
    print(@_);
  }
  return
}

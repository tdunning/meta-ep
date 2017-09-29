# initialize the graphics state the nominal values

proc check_session {{levels 1}} {
    global session
    upvar $levels our_session our_session
    return [expr $session != $our_session]
}

proc ginit {} {
    global xscale yscale xoffset yoffset    
    set xscale 1.0
    set yscale 1.0
    set xoffset 0
    set yoffset 0
}

ginit
    
# transform a point into screen coordinates
proc transform {x y} {
    global xscale yscale xoffset yoffset
    upvar $x xx
    upvar $y yy

    set xx [expr $xscale*$xx + $xoffset]
    set yy [expr $yscale*$yy + $yoffset]
}

# with no args, return current graphics state, 
# with one arg, set current graphics state
proc gstate {args} {
    global xscale yscale xoffset yoffset

    if {[llength $args]} {
	foreach var {xscale yscale xoffset yoffset} val [lindex $args 0] {
	    set $var $val
	}
    } else {
	return [list $xscale $yscale $xoffset $yoffset]
    }
}

proc scale {args} {
    global xscale yscale xoffset yoffset

    switch [llength $args] {
	0 {
	    set x 1
	    set y 1
	}
	1 {
	    set x [lindex $args 0]
	    set y $x
	}
	2 {
	    set x [lindex $args 0]
	    set y [lindex $args 1]
	}
    }
    set xscale [expr $xscale*$x]
    set yscale [expr $yscale*$y]
}

proc translate {x y} {
    global xscale yscale xoffset yoffset

    set xoffset [expr $xoffset+$x]
    set yoffset [expr $yoffset+$y]
}

rename text tk_text
proc text {color x y s} {
}

proc dot {color args} {
    foreach {x y} $args {
	transform x y
	if {[check_session 2]} {
	    break
	}
	.c create rect $x $y [expr $x+5] [expr $y+5] -fill red
    }
    update
}

proc line {color args} {
    set line {}
    foreach {x y} $args {
	transform x y
#	 if {[check_session 1]} {
#	     break
#	 }
	lappend line $x $y
    }
    eval .c create line $line
    update
}

proc clear {} {
    after 500
    .c delete all
}

proc notice {m} {
    global message

    set message $m
    update
}

proc done {} {
    global message
    clear
    set message ""
}

set session 0

set log_setup 0
proc log {s} {
    global log_setup
    if {!$log_setup} {
	toplevel .log
	tk_text .log.t
	pack .log.t
	set log_setup 1
    }

    .log.t insert end "$s\n"
    .log.t see end
}

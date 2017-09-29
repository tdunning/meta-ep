#!/usr/local/bin/wish8.0

source ./graphics.tcl


proc demo_files {files} {
    global session initial_gstate
    incr session
    set our_session $session
    gstate $initial_gstate
    foreach f $files {
	if {$session == $our_session} {source $f}
    }
    if {$session == $our_session} done
}

proc get_file {} {
    demo_files [tk_getOpenFile -title demo_file]
}

label .m -textvariable message -font {Arial 16} -width 40
pack .m

canvas .c -width 400 -height 400 -borderwidth 2 -relief sunken
pack .c -fill both -expand yes

frame .bot
button .bot.a -text Bowl -command {after idle {demo_files [glob x_bowl?*]}}
button .bot.b -text Valley -command {after idle {demo_files [glob x_valley?*]}}
button .bot.c -text Bohachevsky -command {demo_files [glob x_boha?*]}
button .bot.d -text Spiral -command {demo_files [glob x_sp?*]}
button .bot.o -text other -command {demo_files [get_file]}
button .bot.q -text Exit -command exit
pack .bot.a .bot.b .bot.c .bot.d .bot.o -side left -padx 0.05i -pady .1i
pack .bot.q -side right -padx 0.1i

pack .bot -expand y -fill x

proc test {x y} {
    transform x y
    puts "$x $y"
}

scale 5
translate 100 100

set initial_gstate [gstate]

# if {[llength $argv]} {
#     foreach f $argv {
# 	  source $f
#     }
# } else {
#     set x [read stdin]
#     eval $x
# }

 

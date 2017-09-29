#!/usr/local/bin/wish8.0


source graphics.tcl

proc demo_files {which} {
    global initial_gstate
    set cmd [exec contour/Debug/contour.exe]
    gstate $initial_gstate
    eval $cmd
}

canvas .c -width 400 -height 400 -borderwidth 2 -relief sunken
pack .c -fill both -expand yes


frame .bot
button .bot.a -text Bowl -command {after idle {demo_files f1}}
button .bot.b -text Valley -command {after idle {demo_files valley}}
button .bot.c -text Bohachevsky -command {demo_files f7}
button .bot.d -text Spiral -command {demo_files sp}
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

GNU Shogi internals
===================

This document is based on code reverse engineering, very incomplete,
probably wrong in many places.  If you find out something about GNU
Shogi's inner workings that is not described here, please consider
contributing to this document!

Interface modes
---------------

There are currently 2 interface modes: Curses and XShogi.  A third
mode, XBoard, is currently implemented as a minor variant of the
XShogi mode (see `xboard` boolean).

Interface functions that common to all modes are in `commondsp.c`;
those that change with the mode are defined through a `struct display`
instance, respectively defined in `cursesdsp.c` and `rawdsp.c`.  The
`dsp` global pointer is set to one or the other depending on
command-line flags.

Time control
------------

Time-control settings are selected through the `level` command, whose
args are processed by `dsp->SelectLevel`.  Those args are translated
into a set of time-control globals:

TCflag
 wether timecontrol is active

TCminutes, TCseconds
 length of the main time period

TCmoves
 number of moves that must be played in main time period

TCadd
 Fischer clock increment

MaxResponseTime
 ?

Some other parameters influence time control:

hard_time_limit
 If true, if you exceed time limit, you lose.  Introduced in 1.3,
 never set to false - anticipating another mode ?

flags.onemove
 set when time period only has 1 move from the start ?  another way to
 specify a per-move time-limit ?  then what's the difference beteen
 "level 0 n" and "level 1 n" ?
flags.gamein
 controls sudden-death ?

A number of variables are used to manage time-control during the game
 
time0
 the starting time of the current search

et
 elapsed time since time0 (in centiseconds)

ResponseTime, ExtraTime, OperatorTime, TCcount, TCleft
 ? 

During the game, each player's clock is handled by the `TimeControl`
structure, which tracks for the current time period the number of
moves still to be played, and the number centiseconds left to play
them.  `SetTimeControl` is used to initialize them from the
above-mentionned globals.

`SetTimeControl` is also called in the `main()` loop whenever any
player's number of moves left is zero, to switch to the next time
period (see XC), or apparently renew the last time period.  CHECKME:
understand and explain how resetting both player clocks at the same
time is done at the right moment.

Open questions
~~~~~~~~~~~~~~

Why are Fisher clock games set to `TCmoves = 50` ?

How is sudden-death handled ?

How are set/handled XC* ?  Does it work ?  looks like it resets both
player's times whenever one of them drops to 0 moves...

How do ResponseTime / TCcount / TCleft work ?

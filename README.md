#wrxtools

This package implement a C library and tools for parsing
[Weigh X Reps](http://weightxreps.net) website log format.

WxR is a website that allows athletes to track their progress
in strength training pursuits.  Each log entry includes a date,
body weight, and a series of lifts.  Each lift contains multiple
sets; each set has some number of repetitions of a weight being lifted.

## log format

Below is an example of two consecutive entries in the log.
The entries that start with `#` symbol are names of exercises.  Some
movements have an optional tag.

```
2015-05-29
@ 178 bw

#squat #sq
50 x 5 x 5

#OHP #ohp
60 x 5 x 5

#deadlift #dl
100 x 5

2015-06-01
@ 177 bw

#squat #sq
85 x 5 x 5

#bench-press #bp
95 x 5 x 5

#barbell-row
95 x 5 x 5
```

A member of the website can download their own data from the `settings` menu.

## status

This library is under development.  Series of test executables are available
that can parse and summarize a WxR log file.

Currently available:

* `test-best` - reports the best `squat`, `bench`, `deadlift`, and `OHP` result from a log file.
* `test-summary` - reports some high level metrics found in a log file.
* `test-plot` - can plot various data from a log file.
* `test-table` - reports best weight lifted at various reps from a log.

## build

After cloning this repository...

```
./dependencies.sh
git submodule update --init
make
```

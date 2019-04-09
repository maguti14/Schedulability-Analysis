CSE522: Real time embedded systems
Assignment 2 - Spring 2018

Team members:
Vu Coughlin - 1206036828
Mauricio Gutierrez - 1206284660


----------------------------------------------------
				System Requirements
----------------------------------------------------
- Linux (Ubuntu 16.04)
- gcc
- make


----------------------------------------------------
	To compile our program, there are two options:
----------------------------------------------------
1) To compile the program, run:
$ make


*** Note *** : Before compile or re-compile the program, please make sure to run clean so there's no side effect:
$ make clean


----------------------------------------------------
	To run our program, please run:
----------------------------------------------------
For part 1:
$ ./homework2 < <input-file>

For part 2:
$ ./homework2 2

*** Note *** : The program will print out a sequence of methods that have been applied and the analysis result from each method as requested to concole. It is a lot, to keep the console from being cluttered you may want to pipe output to a file

$ ./homework2 2 > <report-file>

*** Note *** : If you wish to easily see the points to plot on excel at the end of the file, you may want to generate only one plot at a time, in which case you can comment out the respective gen_plot() function calls at the bottom fo the .c file in main. Just a suggestion.


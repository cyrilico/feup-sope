all: sfind

sfind: signal_functions.c sfind.c file_processing.c startup_functions.c
	cc -Wall -o sfind sfind.c file_processing.c signal_functions.c startup_functions.c

clean:
	rm sfind

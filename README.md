# feup-sope
Code developed for the first project: A (very simplified) simulation of the Unix utility find.
Makefile included for easier compilation. To run, use "sfind base_dir identifier action", where:
- base_dir is the base directory for the recursive search to start at
- identifier is either -name 'value', -type 'value' or -perm 'value' to identify directory entries to be processed either by name, file type or permissions, respectively
- action is either -print, -delete or -exec 'command' [args] to identify which action to be taken for the matched entries: print (the filename), delete the entry or execute a Unix utility over it

At any time the execution can be stopped through the SIGINT signal. A message will appear to confirm the intention to kill the process

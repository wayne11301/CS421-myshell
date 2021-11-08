### A Simple Shell

This is a customized shell interface program that accepts user commands and then executes each command in a separate process. 

In addition to normal inputs, this shell program provdies some extra command options:
1. yell: Transform all chars to uppercase.
2. ^Z: Display a history list of previous entered commands (up to 10), users may execute any of these history commands by entering 'r' or 'r '+index.
3. While exiting, the program will automatically print out these 7 informations {PID, PPID, %CPU, %MEM, ELAPSED, USER, COMMAND}.

### Sample Output
Please refer to the typescript file

### Walkthough GIF

<img src='test run.gif' title='test run' width='800' alt='test run' />

GIF created with [LiceCap](http://www.cockos.com/licecap/).

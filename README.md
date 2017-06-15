# Winbond-W25Q32JV
Basic mbed library for peeking and poking a Winbond W25Q32JV. Implemented on MBED L031K6 however should be easy enough to convert to other MBED devices or even any other microcontroller with a SPI bus.

## MBED Code
The MBED code opens a serial connection to your PC over USB. Use a program like RealTerm (Windows) or Minicom (Linux) to interact with it.

Code will ask for a 4 character command. The first character indicates the command to execute and the next 3 characters are the arguments. Not all commands use all 3 arguments  (some use no arguments).

c - reset chip, no arguments  
d - erase chip, no arguments  
i - read manufacturer and device ID, no arguments  
r - read page, arguments are the three bytes which make up the address of the location to read  
q - enable writing, no arguments  
b - load buffer page  
w - write contexts of buffer to memory location, arguments are the three bytes which make up the address of the location to write  
e - erase page, arguments are the three bytes which make up the address of the location to erase  
x - quit program, no arguments  

Write enable must be performed before each write or erase command.

## Python Code
TBC

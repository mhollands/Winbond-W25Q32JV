import serial
import sys
import time

if(len(sys.argv) != 3):
	print "Must enter device and filename as command arguments!"
	exit()

SERIALPORT = sys.argv[1];
INPUTFILE = sys.argv[2];
BAUDRATE = 115200;

#open serial port
ser = serial.Serial(SERIALPORT, BAUDRATE)
ser.flushOutput()
ser.flushInput()

while(1):
	print "Waiting for hello world"
	a = ser.readline()
	if("Hello World" in a):
		break
	time.sleep(0.1)

print "got Hello World"

time.sleep(1);

#reset the memory chip
ser.write('c')
time.sleep(0.1);
ser.write('c')
time.sleep(0.1);
ser.write('c')
time.sleep(0.1);
ser.write('c')
print ser.readline();
print ser.readline();
time.sleep(0.5)

#enable write
time.sleep(0.01);
ser.write('q')
time.sleep(0.01);
ser.write('q')
time.sleep(0.01);
ser.write('q')
time.sleep(0.01);
ser.write('q')
print ser.readline();
print ser.readline();
#erase chip
time.sleep(0.01);
ser.write('d')
time.sleep(0.01);
ser.write('d')
time.sleep(0.01);
ser.write('d')
time.sleep(0.01);
ser.write('d')
while(1):
	a = ser.readline();
	print a
	if("NOT BUSY" in a):
		break

reached_end = False;
addr3 = chr(0);
addr2 = chr(0);
with open(INPUTFILE, "rb") as f:
	while(reached_end == False):
		#begin buffer fill
		ser.write('b')
		ser.write('b')
		ser.write('b')
		ser.write('b')
		print ser.readline();
		print ser.readline();
		#load the first 256 bytes
		for x in range(0,256):
		    byte = f.read(1)
		    if(byte == ""):
		    	reached_end = True;
		    	byte = ' '
		    ser.write(byte);

		print ser.readline();

		#enable write
		ser.write('q')
		ser.write('q')
		ser.write('q')
		ser.write('q')
		print ser.readline();
		print ser.readline();

		#write
		ser.write('w')
		ser.write(addr3)
		ser.write(addr2)
		ser.write(chr(0))
		print ser.readline();
		#increment address
		if(ord(addr2) < 255):
			addr2 = chr(ord(addr2)+1)
		else:
			addr2 = chr(0);
			addr3 = chr(ord(addr3) + 1)

		#wait until not busy
		while(1):
			a = ser.readline();
			print a
			if("NOT BUSY" in a):
				break


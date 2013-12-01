#!/usr/bin/python
import string;
import thread;
import time;
import os;

xval = 0
yval = 0
zval = 0


# looks at y axis.
# low number to high number means moving left.
# high number to low number means moving right.
# +/-50 is the threshold, within 0.3 seconds, and we will block future finds for 0.7 secs.
def findSwipe():
	lastHigh = 10
	lastLow = 10
	lastFind = 10
	while True:
		time.sleep(0.1)
		print(yval)
		lastHigh = lastHigh + 1
		lastLow = lastLow + 1
		lastFind = lastFind + 1
		if (yval > 50):
			lastHigh = 0
		if (yval < -50):
			lastLow = 0
		if (lastFind < 7):
			continue # "debouncing"
		if ((lastHigh < 3) and (lastLow < 3)):
			# We have found a swipe.  Is it left or right?
			lastFind = 0
			if (lastHigh > lastLow):
				print("left")
				os.system("osascript -e 'tell application \"System Events\" to key code 123'")
			else:
				print("right")
				os.system("osascript -e 'tell application \"System Events\" to key code 124'")





thread.start_new_thread(findSwipe, ())


while True:
	l = raw_input('');
	pos = string.find(l,'Val = ')
	if (pos == -1):
		continue
	v = int(l[pos+6:]);
	if (v > 127):
		v = v - 256;
	#print(v);

	if (l[pos-1] == 'x'):
		xval = v
		#print(l[pos-1]);
	if (l[pos-1] == 'y'):
		yval = v
		#print(l[pos-1]);
	if (l[pos-1] == 'z'):
		zval = v
		#print(l[pos-1]);

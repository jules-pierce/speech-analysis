#!/usr/bin/env python
import os
import sys
import numpy as np
import scipy as sp
import matplotlib as mpl
import matplotlib.pyplot as plt

#############################
# Main program starts here
#############################
globalminx = 1e100
globalmaxx = -1e100
globalminy = 1e100
globalmaxy = -1e100

argc = len( sys.argv )

if( argc == 1 ) :
	print 'usage is: mygraph file1.txt file2.txt ...'
	sys.exit()

print 'Graphing %u files:' % (argc - 1)

plt.title( 'Data Graph' )
plt.xlabel( 'X values' )
plt.ylabel( 'Y values' )
plt.xticks()
plt.yticks()
plt.grid('on')
for i in range( 1, argc, 1 ) :
	print 'processing file %u' % i
	fd = open( sys.argv[i], 'r' )
	x = list()
	y = list()
	for data in fd :
		xt, yt = data.split()
		x.append(xt)
		y.append(yt)

	tmp_minx = float( min(x, key=float) )
	tmp_maxx = float( max(x, key=float) )
	tmp_miny = float( min(y, key=float)	)
	tmp_maxy = float( max(y, key=float) )
	print "min(x) = %8.3e max(x) = %8.3e min(y) = %8.3e max(y) = %8.3e" % \
	          (tmp_minx, tmp_maxx, tmp_miny, tmp_maxy)
	if tmp_minx < globalminx : globalminx = tmp_minx
	if tmp_maxx > globalmaxx : globalmaxx = tmp_maxx
	if tmp_miny < globalminy : globalminy = tmp_miny
	if tmp_maxy > globalmaxy : globalmaxy = tmp_maxy

	if( i == 1 ) : 
		cs = '-r.'
		l = 'first file'
	elif( i == 2 ) : 
		cs = '-g.'
		l = 'second file'
	elif( i == 3 ) : 
		cs = '-b.'
		l = 'third file'
	elif( i == 4 ) : 
		cs = '-c.'  # cyan
		l = 'fourth file'
	elif( i == 5 ) : 
		cs = '-m.'  # magenta
		l = 'fifth file'
	elif( i == 6 ) : 
		cs = '-y.'  # yellow
		l = 'sixth file'
	else :	
		cs = '-k.'  # black
		l = 'other file'
	plt.plot(x, y, cs, label=l)
	fd.close()

xmin = globalminx - .05*(globalmaxx - globalminx)
xmax = globalmaxx + .05*(globalmaxx - globalminx)
ymin = globalminy - .05*(globalmaxy - globalminy)
ymax = globalmaxy + .05*(globalmaxy - globalminy)
print "xaxismin, xaxismax, yaxismin, yaxismax = ", xmin, xmax, ymin, ymax

plt.xlim(xmin, xmax)
plt.ylim(ymin, ymax)
#plt.legend(loc='upper right')
plt.legend(loc='best')
plt.ion()
plt.show()

value = raw_input('Hit CR to continue\n')

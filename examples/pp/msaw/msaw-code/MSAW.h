//	%Z%   %M%	%I%	(c) CAP Volmac Inn.   %E%-%U%

#include <limits.h>

#define DEBUG_OUT "stderr"
#define GRAPH_OUT "MSAW.cmd"
#define EMPTY_INT INT_MIN

#define min(a,b)    (((a)<(b)) ? (a) : (b))
#define max(a,b)    (((a)>(b)) ? (a) : (b))
#define sign(a)     (((a)==0) ? 0 : ( ((a)>0) ? 1 : -1 ))

#define EPS 2   /* smallest width of a segment */


extern void Message(char, int = 0, int = 0, int = 0, int = 0, int = 0);


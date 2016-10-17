/* $Id: E-locker.pml,v 1.1 2006/01/18 04:55:34 vdmtools Exp $
 *
 * e-locker */

#define N 2
#define locking ls = true
#define unlocking ls = false
#define locked (ls == true)
#define unlocked (ls == false)
#define opening ds = true
#define closing ds = false
#define opened (ds == true)
#define closed (ds == false)
#define eq (Ks == K)
#define kfull (i == N)


mtype = {D, L, C, OPEN, CLOSE, lock, unlock};

chan Bch = [1] of { mtype, bit };
chan Dch = [0] of { mtype };
chan Lch = [0] of { mtype };

bool ls = true, ds = false;
short K, Ks;
byte i;
bit d,t;



inline new()
{ d_step{ K = 0; i = 0 }}

inline in(d)
{ d_step{ K = 2*K+d; i++ }}


proctype Button()
{
 end:
 bs1: new();
   if
   :: Bch ? D(d)  -> in(d); goto bs2
   :: Bch ? C(t)  -> goto bs1
   :: Bch ? L(t)  -> goto bs1
   fi;
 bs2: if
   :: kfull -> 
     if
     :: eq -> 
	       progress1: Lch ! unlock; goto bs3
     :: else    -> goto bs1
     fi
   :: else  -> 
     if
     :: Bch ? D(d) -> in(d); goto bs2
     :: Bch ? C(t) -> goto bs1
     :: Bch ? L(t) -> goto bs2
     fi
   fi;
 bs3: if
   :: Bch ? D(d) -> new(); in(d); goto bs4
   :: Bch ? L(t) -> 
     progress2: 
     if
     :: kfull -> 
	          Lch ! lock; Ks = K; goto bs1
     :: else  -> goto bs3
     fi
   :: Bch ? C(t) -> goto bs3
   fi;
 bs4: if
   :: kfull -> goto bs3
   :: else  -> if
     :: Bch ? D(d) -> in(d); goto bs4
     :: Bch ? C(t) -> new(); goto bs4
     :: Bch ? L(t) -> goto bs4
     fi
   fi
}

proctype Lock()
{
  ls1: if
    :: Lch ? unlock -> unlocking; goto ls2
    :: Lch ? lock   -> goto ls1
    fi;
  ls2: if
    :: Lch ? lock   -> locking; goto ls1
    :: Lch ? unlock -> goto ls2
    fi
}

proctype Door()
{
  ds1: if
    :: Dch ? OPEN  -> 
      if
      :: unlocked -> opening; goto ds2
      :: else     -> goto ds1
      fi
    :: Dch ? CLOSE -> goto ds1
    fi;
  ds2: if 
    :: Dch ? CLOSE  -> closing; goto ds1
    :: Dch ? OPEN   -> goto ds2
    fi
}

proctype user()
{ Bch ! D(1);
  Bch ! C(t);
  Bch ! D(1);
  Bch ! D(1);
  unlocked; Dch ! OPEN;
  Bch ! D(1);
  Bch ! C(t);
  Bch ! D(0);
  Bch ! D(0);
  Bch ! L(t);
  Dch ! CLOSE;
  Bch ! D(0);
  Bch ! D(0);
  unlocked; Dch !  OPEN
}

proctype watch()
{ do
  :: timeout -> printf("deadlock ?")
  od
}

proctype user0()
{ do
  :: Bch ! D(0)
  :: Bch ! D(1)
  :: Bch ! C(t)
  :: Bch ! L(t)
  :: Dch ! OPEN
  :: Dch ! CLOSE
  od
}


trace{
  do
  :: Lch ? unlock; Lch ? lock
  od
}

init
{ Ks = 3;
  atomic{
    run Button(); run Lock(); run Door(); run user(); 
    run watch()}
}

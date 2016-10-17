/* e-locker */
#define N 4
#define complete (K[0] != -1)
#define locking	ls = true
#define unlocking ls = false
#define locked	(ls == true)
#define unlocked (ls == false)
#define opening ds = true
#define closing ds = false
#define opened (ds == true)
#define closed (ds == false)
#define matched (r == true)

mtype = 
	{ D, L, C, OPEN, CLOSE, lock, unlock };

show chan Bch = [1] of {mtype, short };
show chan Dch = [0] of {mtype};
show chan Lch = [0] of {mtype};

show bool ls = true, ds = false, r = false;
show short K[N], Ks[N], d, w;
show byte i;

inline new()
{
	d_step {
		i = 0;
		do
		:: i < N	-> K[i] = -1; i++
		:: i == N	-> break
		od
	}
}

inline ass() /* new key assignf */
{
	d_step {
		i = 0;
		do
		:: i < N	-> Ks[i] = K[i]; i++
		:: i == N	-> break
		od
	}
}

inline in(d)
{
	d_step {
		assert (!complete);
		i = 0;
		do
		:: i < (N - 1)	-> K[i] = K[i+1]; i++
		:: i == (N - 1)	-> break
		od;
		K[N - 1] = d
	}
}

inline eq()
{
	bool rr;
	d_step {
		rr = true;
		i = 0;
		do
		:: i < N	-> rr = rr && (Ks[i] == K[i]); i++
		:: i == N	-> break
		od;
	r = rr
	}
}

proctype Button()
{
end:
s1: new();
	if
	:: Bch ? D(d)	-> in(d); goto s2
	:: Bch ? L(w)	-> goto s1
	:: Bch ? C(w)	-> goto s1
	fi;
s2: if
	:: complete	-> eq();
		if
		:: r == true	-> progress1: Lch ! unlock; goto s3
		:: r == false	-> goto s1
		fi
	:: else	-> if
		:: Bch ? D(d)	-> in(d); goto s2
		:: Bch ? C(w)	-> goto s1
		:: Bch ? L(w)	-> goto s2
		fi
	fi;
s3: if
	:: Bch ? D(d)	-> new(); in(d); goto s4
	:: Bch ? L(w)	-> progress2: Lch ! lock; ass(); goto s1
	:: Bch ? C(w)	-> goto s3
	fi;
s4: if
	:: complete	-> goto s3
	:: else	-> if
		:: Bch ? D(d)	-> in(d); goto s4
		:: Bch ? C(w)	-> new(); goto s3
		:: Bch ? L(w)	-> goto s4
		fi
	fi
}

proctype Lock()
{
ls1: if
	:: Lch ? unlock	-> unlocking; goto ls2
	fi;
ls2: if
	:: Lch ? lock	-> locking; goto ls1
	fi
}

proctype Door ()
{
ds1: if
	::Dch?OPEN -> 
		if 
		:: unlocked -> opening; goto ds2
		fi
	fi;
ds2: if
	:: Dch?CLOSE -> closing; goto ds1
	fi
}

proctype user()
{
Bch ! D(1);
Bch ! D(2);
Bch ! D(3);
Bch ! D(4);
if
::unlocked	-> Dch ! OPEN
fi;
Bch ! D(1);
Bch ! D(1);
Bch ! D(1);
Bch ! D(1);
Dch ! CLOSE;
if
::closed	-> 
	Bch ! L(w); printf("send L1");
fi;
Bch ! D(1); printf("send D");
Bch ! D(1);
Bch ! D(1);
Bch ! D(1);
if
::unlocked	-> Dch ! OPEN
fi;
Bch ! D(9);
if
::closed	-> Bch ! L(w); printf("send L")
fi;
Bch ! D(8);
Bch ! D(7);
Bch ! D(6);
}

init
{
	d_step {
		Ks[0] = 1;
		Ks[1] = 2;
		Ks[2] = 3;
		Ks[3] = 4;
	};
	atomic {
		run Button(); run Lock(); 
		run Door(); run user()
	}
}

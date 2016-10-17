#define complete true
#define locking	ls = true
#define unlocking ls = false
#define locked	(ls == true)
#define unlocked (ls == false)
#define opening ds = true
#define closing ds = false
#define opened (ds == true)
#define closed (ds == false)
#define matched (Ks == K)

mtype = { D, L, C, OPEN, CLOSE, lock, unlock };

show chan Bch = [1] of {mtype, short };
chan Dch = [0] of {mtype};
show chan Lch = [0] of {mtype};

bool ls = true, ds = false;
short K, Ks, d, w
byte i;

inline ass() /* new key assignf */
{ Ks = K }

inline in(d)
{ K = d }

proctype Button()
{
end:
s1: 
	if
	:: Bch ? D(d)	-> in(d); goto s2
	:: Bch ? L(w)	-> goto s1
	:: Bch ? C(w)	-> goto s1
	fi;
s2: 
	if
	:: matched	-> progress1: Lch ! unlock; goto s3
	:: ! matched	-> goto s1
	fi;
s3: if
	:: Bch ? D(d)	-> in(d); goto s3
	:: Bch ? L(w)	-> Lch ! lock; ass(); goto s1
	:: Bch ? C(w)	-> goto s3
	fi;
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
Bch ! D(1); printf("send D(1)");
if
::unlocked	-> Dch ! OPEN; printf("Open1")
fi;
Bch ! D(2); printf("send D(2)");
Dch ! CLOSE; printf("Close1");
if
::closed	-> Bch ! L(w); printf("Lock1")
fi;
Bch ! L(w); printf("Lock2");
Bch ! D(2); printf("send D(2)");
if
::unlocked	-> Dch ! OPEN;  printf("Open2")
fi;
}

init
{
	Ks= 1;
	atomic {
		run Button(); run Lock(); run Door(); run user()
	}
}

never {    /* !(locked -> <> unlocked) */
accept_init:
T0_init:
	if
	:: (! ((unlocked)) && (locked)) -> goto accept_S3
	fi;
accept_S3:
T0_S3:
	if
	:: (! ((unlocked))) -> goto accept_S3
	fi;
}
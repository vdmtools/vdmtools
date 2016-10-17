/* $Id: E-Locker-simple-0.pml,v 1.1 2006/01/18 04:55:34 vdmtools Exp $
 * 
 * Store E-Locker */
short key, digit
bool lockStatus, doorStatus

#define locking	lockStatus = true
#define unlocking lockStatus = false
#define locked	(lockStatus == true)
#define unlocked (lockStatus == false)
#define opening doorStatus = true
#define closing doorStatus = false
#define opened (doorStatus == true)
#define closed (doorStatus == false)
#define matched	(key == digit)

mtype = {D, OPEN, CLOSE, lock, unlock}

chan ButtonCh = [1] of {mtype, short};
chan LockCh = [0] of {mtype};
chan DoorCh = [0] of {mtype};

init
{
	key = 1;
	atomic {
		run User()
	}
}

proctype User()
{
ButtonCh ! D(1); printf("try unlock1");
if
::unlocked	-> DoorCh ! OPEN; printf("try door open1")
fi;
DoorCh ! CLOSE; printf("try close1");
if
::closed	-> printf("try locking");LockCh ! lock
fi;
ButtonCh ! D(1); printf("try unlock2");
if
::unlocked	-> DoorCh ! OPEN; printf("try door open2")
fi
}

active proctype Ctrl()
{
lockS:
if
:: ButtonCh ? D(digit)	-> 
	if 
	:: matched	-> printf("unlocking"); goto closeS
	:: else	-> goto lockS
	fi
fi;
closeS:
if
:: LockCh ? lock	-> 
	printf("locking"); goto lockS
:: DoorCh ? OPEN	-> 
	opening; printf("opening");goto openS;
fi;
openS:
if
:: DoorCh ? CLOSE	-> 
	closing; printf("closinglocked"); goto closeS
fi;
}

	/*
	 * Formula As Typed: locked -> <> unlocked
	 * The Never Claim Below Corresponds
	 * To The Negated Formula 
	 *               !(locked -> <> unlocked)
	 * (formalizing violations of the original)
	 */

never {    /* !(locked -> <> unlocked) */
accept_init:
T0_init:
	if
	:: (! ((unlocked)) && (locked)) -> 
		goto accept_S3
	fi;
accept_S3:
T0_S3:
	if
	:: (! ((unlocked))) -> goto accept_S3
	fi;
}

#define K_OPEN 1
#define K_CLOSED 0
#define K_UNLOCKED 0
#define K_LOCKED 1


#define KEYLENGTH 4
mtype = {NUM,CAN,LOCK};

chan keypad = [0] of { mtype, int};
chan handle = [0] of { bit }
bit lock_status = K_LOCKED; /* initial state locked, means locked */
bit door_status = K_CLOSED; /* initial state door closed  */

inline clear_buf(){
  d_step {
    buf_len = 0;
    buf[0] = -1;
    buf[1] = -1;
    buf[2] = -1;
    buf[3] = -1
  }
}

inline open_the_door(){
  lock_status == K_UNLOCKED;
  assert(lock_status == K_UNLOCKED);
  handle ! K_OPEN;
  door_status == K_OPEN;
  assert(door_status == K_OPEN);
}

/* random key in user */
proctype user1(){
  do
  :: keypad ! NUM(0);
  :: keypad ! NUM(1);
  :: keypad ! NUM(2);
  :: keypad ! NUM(3);
  :: keypad ! NUM(4);
  :: keypad ! NUM(5);
  :: keypad ! CAN(0);
  :: keypad ! LOCK(0);
  :: handle ! K_OPEN;
  :: handle ! K_CLOSED
  od
};
 
proctype user2() {
  /* open the door */
  keypad ! NUM(1);
  keypad ! NUM(2);
  keypad ! NUM(3);
  keypad ! NUM(4);
  open_the_door();
  
  /* then closed the door */
  handle ! K_CLOSED;
  door_status == K_CLOSED;
  /* set new key */
  keypad ! NUM(5);
  keypad ! NUM(6);
  keypad ! NUM(7);
  keypad ! NUM(8);
  keypad ! LOCK(0);
  lock_status == K_LOCKED;
  assert(lock_status == K_LOCKED);

 /* open the door with the new key */
  keypad ! NUM(5);
  keypad ! NUM(6);
  keypad ! NUM(7);
  keypad ! NUM(8);
  open_the_door();
}

proctype user3() {
  /* open the door */
  keypad ! NUM(5);
  keypad ! NUM(5);
  keypad ! NUM(5);
  keypad ! NUM(5);
  keypad ! LOCK(0);

  keypad ! NUM(5);
  keypad ! NUM(5);
  keypad ! NUM(5);
  keypad ! NUM(5);

  open_the_door();
}

proctype safe(){
  int key;
  int buf[KEYLENGTH];
  int buf_len = 0;
  int currentKey[4]; /* initial key value is "1234" */
  d_step {
    currentKey[0] = 1;
    currentKey[1] = 2;
    currentKey[2] = 3;
    currentKey[3] = 4;
  }
end0:
  do
    :: keypad ? NUM(key) -> 
       d_step {
	 printf("Number input:%d.\n",key);
	 if
	   :: door_status == K_CLOSED ->
	      /* the door is close, accumulate keys into buf */
	      /* display them on LED */
	      buf[0] = buf[1];
	      buf[1] = buf[2];
	      buf[2] = buf[3];
	      buf[3] = key;
	      if
		:: buf_len <= 3 -> buf_len++;
		:: else -> skip
	      fi;
	      if
		:: lock_status == K_LOCKED ->
		   /* the door is currently locked */
		   if
		     :: ((buf[0] == currentKey[0]) && (buf[1] == currentKey[1]) &&
			 (buf[2] == currentKey[2]) && (buf[3] == currentKey[3]))
			-> lock_status = K_UNLOCKED;
			printf("Unlocked.\n");
			clear_buf() /* light is off [open] */
		     :: else -> skip
		   fi
		:: else -> skip
	      fi
	   :: door_status == K_OPEN ->
	      /* if the door is open, ignore the key input */
	      printf("Door open and key input ignored.\n");
	      skip
	 fi
       }
       /* accumulate the data into the buffer */
    :: keypad ? CAN(_) ->
       /* clear the buffer */
       clear_buf()
    :: keypad ? LOCK(_) ->
       d_step {
	 printf("Lock Key pressed.\n");
	 if
	   :: lock_status == K_UNLOCKED ->
	      /* Lock the door and set new key */
	      if
		:: buf_len == 0 -> {
		  /* use current key and lock */
		  lock_status = K_LOCKED; 
		  printf("Locked with the current Key.\n")
		}
		:: buf_len == 4 /* set net key and lock */
		   -> {
		     currentKey[0] = buf[0];
		     currentKey[1] = buf[1];
		     currentKey[2] = buf[2];
		     currentKey[3] = buf[3];
		     lock_status = K_LOCKED;
		     clear_buf();
		     printf("Locked with the new key.\n")
		   }
		:: else -> {
		  printf("Error: insufficient new key. Try again!\n");
		  clear_buf();
		}
	      fi
	   :: else -> skip
	 fi
       }
    :: handle ? K_CLOSED ->
       /* handle closed */
       d_step {
	 if
	   :: door_status == K_OPEN -> {
	     door_status = K_CLOSED;
	     printf("Door closed.\n");
	   }
	   :: else -> skip
	 fi
	 
       }
    :: handle ? K_OPEN ->
       /* handle \opened
        check if unlocked
	if off, open the door, clear the buffer */
       d_step {
	 if
	   :: (lock_status == K_UNLOCKED) -> {
	     door_status = K_OPEN;
	     printf("Door opened.\n");
	   }
	   :: else -> skip
	 fi
       }
  od
}

init {
  run user3();
  run safe();
}

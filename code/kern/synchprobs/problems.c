/*
 * Copyright (c) 2001, 2002, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Driver code for whale mating problem
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

/*
 * 08 Feb 2012 : GWA : Driver code is in kern/synchprobs/driver.c. We will
 * replace that file. This file is yours to modify as you see fit.
 *
 * You should implement your solution to the whalemating problem below.
 */


// 13 Feb 2012 : GWA : Adding at the suggestion of Isaac Elbaz. These
// functions will allow you to do local initialization. They are called at
// the top of the corresponding driver code

struct semaphore *male_sem;
struct semaphore *female_sem;
struct lock *hold;
struct cv* mate_cv;
volatile int male_count;
volatile int female_count;

void whalemating_init() {
 hold=lock_create("My lock");
 male_sem=sem_create("Male Semaphore",0);
 female_sem=sem_create("Female Semaphore",0);
mate_cv=cv_create("mating cv");
male_count=0;
female_count=0;
  return;
}

// 20 Feb 2012 : GWA : Adding at the suggestion of Nikhil Londhe. We don't
// care if your problems leak memory, but if you do, use this to clean up.

void whalemating_cleanup() {

	sem_destroy(male_sem);
	sem_destroy(female_sem);
	lock_destroy(hold);
	cv_destroy(mate_cv);
  return;
}

void
male(void *p, unsigned long which)
{
	struct semaphore * whalematingMenuSemaphore = (struct semaphore *)p;
  (void)which;
  
  male_start();

  lock_acquire(hold);

  V(male_sem);
  male_count++;
  if(female_count!=0)
  cv_signal(mate_cv,hold);
  else
	  cv_wait(mate_cv,hold);
  lock_release(hold);

	// Implement this function 
  male_end();

  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // whalemating driver can return to the menu cleanly.
  V(whalematingMenuSemaphore);
  return;
}

void
female(void *p, unsigned long which)
{
	struct semaphore * whalematingMenuSemaphore = (struct semaphore *)p;
  (void)which;
  
  female_start();
  lock_acquire(hold);
   V(female_sem);
   female_count++;
   if(male_count!=0)
   cv_signal(mate_cv,hold);
   else
	   cv_wait(mate_cv,hold);
  lock_release(hold);
	// Implement this function 
  female_end();
  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // whalemating driver can return to the menu cleanly.
  V(whalematingMenuSemaphore);
  return;
}

void
matchmaker(void *p, unsigned long which)
{
	struct semaphore * whalematingMenuSemaphore = (struct semaphore *)p;
  (void)which;
  
  matchmaker_start();
  lock_acquire(hold);
 P(male_sem);
 male_count--;
 lock_release(hold);
 lock_acquire(hold);
  P(female_sem);
  female_count--;
  lock_release(hold);
	// Implement this function 
  matchmaker_end();
  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // whalemating driver can return to the menu cleanly.
  V(whalematingMenuSemaphore);
  return;
}




/*
 * You should implement your solution to the stoplight problem below. The
 * quadrant and direction mappings for reference: (although the problem is,
 * of course, stable under rotation)
 *
 *   | 0 |
 * --     --
 *    0 1
 * 3       1
 *    3 2
 * --     --
 *   | 2 | 
 *
 * As way to think about it, assuming cars drive on the right: a car entering
 * the intersection from direction X will enter intersection quadrant X
 * first.
 *
 * You will probably want to write some helper functions to assist
 * with the mappings. Modular arithmetic can help, e.g. a car passing
 * straight through the intersection entering from direction X will leave to
 * direction (X + 2) % 4 and pass through quadrants X and (X + 3) % 4.
 * Boo-yah.
 *
 * Your solutions below should call the inQuadrant() and leaveIntersection()
 * functions in drivers.c.
 */

// 13 Feb 2012 : GWA : Adding at the suggestion of Isaac Elbaz. These
// functions will allow you to do local initialization. They are called at
// the top of the corresponding driver code.
struct lock* intersect_lock;
struct cv* intersect_cv;
volatile int cur_poss[4];// an array to store the possible positions of the car
//currently

void stoplight_init() {
	intersect_cv=cv_create("Intersection Condition Variable");
	if(intersect_cv==NULL)
	{
		///return NULL;
		panic("condition variable not created");
	}
	intersect_lock=lock_create("A lock on the intersection");
	if(intersect_lock==NULL)
	{
		//return NULL;
		panic("Lock could not be created");
	}
	
	
  return;
}

// 20 Feb 2012 : GWA : Adding at the suggestion of Nikhil Londhe. We don't
// care if your problems leak memory, but if you do, use this to clean up.

void stoplight_cleanup() {
	cv_destroy(intersect_cv);
	lock_destroy(intersect_lock);
  return;
}

void
gostraight(void *p, unsigned long direction)
{
	struct semaphore * stoplightMenuSemaphore = (struct semaphore *)p;
  //(void)direction;
	int sec_quad=(direction+3)%4;
	lock_acquire(intersect_lock);
	// So while any of the two possible quadrants are already set, It means some 
	//thread is waiting...Hopefully
	while(cur_poss[direction]==1||cur_poss[sec_quad]==1)
		cv_wait(intersect_cv,intersect_lock);
	cur_poss[direction]=1;
	cur_poss[sec_quad]=1;
	inQuadrant(direction);
	lock_release(intersect_lock);
	lock_acquire(intersect_lock);
    cur_poss[direction]=0;
    //while(curr_p)
    //cv_broadcast(intersect_cv);
    inQuadrant(sec_quad);
    cv_broadcast(intersect_cv,intersect_lock);
    lock_release(intersect_lock);
    lock_acquire(intersect_lock);
    cur_poss[sec_quad]=0;
    leaveIntersection();
    cv_broadcast(intersect_cv,intersect_lock);
    lock_release(intersect_lock);
  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // stoplight driver can return to the menu cleanly.
  V(stoplightMenuSemaphore);
  return;
}

void
turnleft(void *p, unsigned long direction)
{
	struct semaphore * stoplightMenuSemaphore = (struct semaphore *)p;
  (void)direction;
  int sec_quad=(direction+3)%4;
  int third_quad=(direction+2)%4;
  lock_acquire(intersect_lock);
  while(cur_poss[direction]==1||cur_poss[sec_quad]==1||cur_poss[third_quad]==1)
	  cv_wait(intersect_cv,intersect_lock);
  cur_poss[direction]=1;
  cur_poss[sec_quad]=1;
  inQuadrant(direction);
  lock_release(intersect_lock);
  lock_acquire(intersect_lock);
  cur_poss[direction]=0;
  
  //while(cur_poss[third_quad]==1||cur_poss[sec_quad]==1)
  //cv_wait(intersect_cv,intersect_lock);
      cur_poss[third_quad]=1;
      //while(curr_p)
      //cv_broadcast(intersect_cv);
      inQuadrant(sec_quad);
      cv_broadcast(intersect_cv,intersect_lock);
      lock_release(intersect_lock);
        lock_acquire(intersect_lock);
            //currposs[direction]=0;
            cur_poss[sec_quad]=0;
            //while(curr_p)
            //cv_broadcast(intersect_cv);
            inQuadrant(third_quad);
            cv_broadcast(intersect_cv,intersect_lock);
                //cur_poss[third_quad]=0;
            lock_release(intersect_lock);
              lock_acquire(intersect_lock);
                leaveIntersection();
                cur_poss[third_quad]=0;
                cv_broadcast(intersect_cv,intersect_lock);
                lock_release(intersect_lock);
  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // stoplight driver can return to the menu cleanly.
  V(stoplightMenuSemaphore);
  return;
}

void
turnright(void *p, unsigned long direction)
{
	struct semaphore * stoplightMenuSemaphore = (struct semaphore *)p;
  (void)direction;
  // Easy coz the vehicle is in same lane!!
  lock_acquire(intersect_lock);
  while(cur_poss[direction]==1)
	  cv_wait(intersect_cv,intersect_lock);
  cur_poss[direction]=1;
  inQuadrant(direction);
  lock_release(intersect_lock);
    lock_acquire(intersect_lock);
  leaveIntersection();
  cur_poss[direction]=0;
  cv_broadcast(intersect_cv,intersect_lock);
  lock_release(intersect_lock);
  // 08 Feb 2012 : GWA : Please do not change this code. This is so that your
  // stoplight driver can return to the menu cleanly.
  V(stoplightMenuSemaphore);
  return;
}

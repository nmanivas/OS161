/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
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
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <lib.h>
#include <thread.h>
#include <current.h>
#include <addrspace.h>
#include <vm.h>
#include <vfs.h>
#include <syscall.h>
#include <test.h>
#include <file.h>
#include<synch.h>
#include<copyinout.h>
//extern pid_t pidcount;
//extern struct process* p_table[17];
/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 */

int
runprogram(char *progname,char** arguments)
{
	//int firstid;
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;
	int i;
	int numargs;
	size_t actual_size=0;
	//int argc1=(int)argc;
	//curthread->pid=PID_MIN;
	//curthread->proc=(struct process*)kmalloc(sizeof(struct process));
	//curthread->proc->pid=PID_MIN;

	//for(firstid=0;firstid<16;firstid++)
	//{
	    // if(p_table[firstid]==NULL)
	     //{
	    	//p_table[firstid]=(struct process*)kmalloc(sizeof(struct process));
	    	//p_table[firstid]->full=1;
	    	//pidcount++;
	    	//p_table[firstid]->pid=PID_MIN;
	    	//p_table[firstid]->ppid=1;
	    	//p_table[childid]->exitsem=sem_create("process",0);
	    	//p_table[firstid]->tlock=lock_create("My Lock");
	    	//p_table[firstid]->wcv=cv_create("My CV");
	    	//p_table[firstid]->self=curthread;
	    	//break;
	     //}
	//}
	//pidcount++;
	//curthread->proc->full=1;
	//curthread->proc->ppid=-1;
	//curthread->proc->self=curthread;
	//curthread->proc->tlock=lock_create("My Lock");
	//curthread->proc->wcv=cv_create("My CV");
	//p_table[PID_MIN]=curthread->proc;
	//p_table[c]->full=1;
	//pidcount++;
	//p_table[childid]->pid=pidcount;
	//p_table[childid]->ppid=curthread->pid;
	    	//p_table[childid]->exitsem=sem_create("process",0);
	//p_table[childid]->tlock=lock_create("My Lock");
	//p_table[childid]->wcv=cv_create("My CV");
	//p_table[childid]->self=NULL;

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, 0, &v);
	if (result) {
		return result;
	}

	/* We should be a new thread. */
	KASSERT(curthread->t_addrspace == NULL);
	curthread->t_filetable=NULL;
  if (curthread->t_filetable == NULL) {
		result = filetable_init("con:", "con:", "con:");
		if (result) {
			return result;
		}
	}

  /*char** kargv=(char**)kmalloc(sizeof(char*)*argc1);
  /for(i=0;i<argc1;i++)
  	{
  		actual_size=0;
  		kargv[i]=(char*)kmalloc(PATH_MAX);
  		result=copyinstr((userptr_t)argv[i],kargv[i],PATH_MAX,&actual_size);
  		if(result)
  		{
  			kfree(kargv);
  			//kfree(namedes);
  			return EFAULT;
  		}

  	}
  	actual_size=0;
  	*/

	/* Create a new address space. */
	curthread->t_addrspace = as_create();
	if (curthread->t_addrspace==NULL) {
		vfs_close(v);
		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_addrspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_addrspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);



	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_addrspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_addrspace */
		return result;
	}
	numargs=0;
		while(arguments[numargs]!=NULL)
		{
			//kprintf("%");
			numargs=numargs+1;
		}
		char** kargv=(char**)kmalloc(sizeof(char*)*numargs);
		//int i;
		//for(i=0;i<numargs;i++)
		//{
			//actual_size=0;
			kargv[0]=(char*)kmalloc(PATH_MAX);
			//result=copyinstr((userptr_t)arguments[i],kargv[i],PATH_MAX,&actual_size);
			memcpy((void*)kargv,(const void*)arguments,sizeof(arguments[0]));
			//if(result)
			//{
				//kfree(kargv);
				//kfree(namedes);
				//return EFAULT;
			//}

		//}
			//kprintf(%)
		actual_size=0;

	for(i=numargs-1;i>=0;i--)
			{
				actual_size=0;
				int length=strlen(kargv[i])+1;
				//int length=strlen(kargv[i]);
				int padder=length%4;
				if(padder!=0)
				length=length+padder;
				copyoutstr(kargv[i],(userptr_t)stackptr,length,&actual_size);
				//stackptr=stackptr+length;
				stackptr=stackptr-length;


			}


	/* Warp to user mode. */
	enter_new_process(numargs, (userptr_t)stackptr,
			  stackptr, entrypoint);
	
	/* enter_new_process does not return. */
	panic("enter_new_process returned\n");
	return EINVAL;
}


/*

 * proc_sys.c
 *
 *  Created on: Mar 7, 2014
 *      Author: trinity
 */

#include <types.h>
#include<mips/trapframe.h>
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
#include <synch.h>
#include<copyinout.h>
#include <spl.h>
#include<kern/wait.h>

extern struct process* p_table[17];

//struct cv* wcv;

extern pid_t pidcount;
int mywait(struct process* mypro) {
	//lock_acquire(mypro->tlock);
	while (!mypro->exited) {
		cv_wait(mypro->wcv, mypro->tlock);
	}
	//*status=mypro->exitcode;
	//lock_release(mypro->tlock);
	//int test = mypro->exitcode;
	//test++;
	return 0;
}

int sys_fork(struct trapframe* tf, unsigned long adrs, int *retval) {
	(void) adrs;
	int flag;
	pid_t childid;
	struct addrspace *adrcopy;
	struct trapframe* copyt = (struct trapframe*) kmalloc(sizeof(struct trapframe));
	if(copyt==NULL)
		return ENOMEM;
	bzero(copyt, sizeof(struct trapframe));

	//struct filetable* copyft;
	//kprintf("Parent ID:%d\n",curthread->proc->pid);

	memcpy(copyt, tf, sizeof(struct trapframe));
	flag = as_copy(curthread->t_addrspace, &adrcopy);
	if(flag)
		return flag;
	//if(curthread->pid==0)
	//{
	//curthread->pid=PID_MIN;
	//}
	if (flag) {
		kfree(copyt);
		return flag;
	}

	//we need to allocate pid for our new process

	//pid_t cpid;
	/*
	for (childid = 0; childid <= 16; childid++) {
		if (p_table[childid] == NULL ) {
			p_table[childid] = (struct process*) kmalloc(
					sizeof(struct process));
			if(p_table[childid]==NULL)
			{
				return ENOMEM;
			}
			p_table[childid]->full = 1;
			pidcount++;
			p_table[childid]->pid = pidcount;
			p_table[childid]->ppid = curthread->pid;
			//p_table[childid]->exitsem=sem_create("process",0);
			p_table[childid]->tlock = lock_create("My Lock");
			if(p_table[childid]==NULL)
			{
				return ENOMEM;
			}
			p_table[childid]->wcv = cv_create("My CV");
			if(p_table[childid]==NULL)
				return ENOMEM;
			p_table[childid]->self = NULL;
			break;
		}
	}
	if (childid > PID_MAX) {
		//retval = 1;
		return EMPROC;
	}
	*/
	//int i;

	//copyt->tf_a0 = (int) p_table[childid]->pid;
	//kprintf("Ret:%d\n",copyt->tf_a0);
	struct thread *new_proc;
	//curthread->t_addrspace=(struct addrspace*)adrcopy;
	//int s=splhigh();
	flag = thread_fork("newproc", enter_forked_process, (void *) copyt,
			(unsigned long) adrcopy, &new_proc);
	//kprintf("Out of thread fork:%d\n",flag);
	if (flag) {
		kfree(copyt);
		as_destroy(adrcopy);

		return flag;
	}


	for (childid = 0; childid <= 16; childid++) {
			if (p_table[childid] == NULL ) {
				p_table[childid] = (struct process*) kmalloc(
						sizeof(struct process));
				if(p_table[childid]==NULL)
				{
					return ENOMEM;
				}
				p_table[childid]->full = 1;
				pidcount++;
				p_table[childid]->pid = pidcount;
				p_table[childid]->ppid = curthread->pid;
				//p_table[childid]->exitsem=sem_create("process",0);
				p_table[childid]->tlock = lock_create("My Lock");
				if(p_table[childid]->tlock==NULL)
				{
					return ENOMEM;
				}
				p_table[childid]->wcv = cv_create("My CV");
				if(p_table[childid]->wcv==NULL)
					return ENOMEM;
				p_table[childid]->self = NULL;
				break;
			}
		}
		if (childid > PID_MAX) {
			//*retval = 1;
			return EMPROC;
		}

	//new_proc->pid=(pid_t)childid;
	//new_proc=p_table[childid];

	new_proc->pid = p_table[childid]->pid;
	new_proc->proc = p_table[childid];
	// new_proc->t_filetable = kmalloc(sizeof(struct filetable));
	//if (new_proc->t_filetable == NULL) {
	//return ENOMEM;
	//}
	p_table[childid]->self = new_proc;

	//new_proc->pid=p_table[childid]->pid;
	//new_proc->proc->pid=p_table[childid]->pid;
	//kprintf("Child id in fork: %d\n",new_proc->pid);

	//kprintf("Child id:%d",new_proc->pid);
	//new_proc->t_filetable->
	//for(i=0;i<128;i++)
	//{
	flag = filetable_copy(&new_proc->t_filetable);
	//}
	//splx(s);
	// kprintf("Fork over!");

	*retval = p_table[childid]->pid;
	//kprintf("Return val in fork:%d",*retval);
	return 0;

	//flag=
	//flag=thread_fork=
}

int sys_execv(char *progname, char** arguments) {

	int flag;
	int numargs;
	int addr;
	int i;
	// first we need to copy the program name to the kernel space
	// then we go for arguments. Path size unknown

	size_t actual_size = 0;
	char *namedes;

	if (progname == NULL ) {
		return EFAULT;
	}
	if (progname == "") {
		return EINVAL;
	}

	namedes = (char*) kmalloc(PATH_MAX);
	if(namedes==NULL)
	{
		return ENOMEM;
	}
	flag = copyinstr((const_userptr_t) progname, namedes, PATH_MAX,
			&actual_size);
	if (flag != 0) {
		kfree(namedes);
		return flag;
	}
	//namedes[actual_size]
	if (strlen(namedes) == 0) {
		//*retval=1;
		return EISDIR;
	}

	// now we open file using vfs_open. Same as runprogram
	struct vnode* vn;
	flag = vfs_open(namedes, O_RDONLY, 0, &vn);
	if (flag) {
		return flag;
	}
	actual_size = 0;
	// according to the blog, now cpy the arguments 1 by one into the kernel spca.

	//char** kargv=(char**)kmalloc(sizeof(char));
	// get the number of arguments
	if (arguments == NULL ) {
		kfree(namedes);
		return EFAULT;
	}
	flag = copyin((userptr_t) arguments, &addr, sizeof(int));
	if (flag) {
		kfree(namedes);
		return EFAULT;
	}

	//check=wthread->exitcode;
	//err=copyout(&check,(userptr_t)status,sizeof(check));
	//if(err)
	//{
	//	lock_release(p_table[i]->tlock);
	//return err;
	//}

	numargs = 0;
	while (arguments[numargs] != NULL ) {
		//kprintf("%");
		numargs = numargs + 1;
	}

	char** kargv = (char**) kmalloc(sizeof(char*) * numargs);
	//int i;
	for (i = 0; i < numargs; i++) {
		actual_size = 0;
		kargv[i] = (char*) kmalloc(PATH_MAX);
		if(kargv==NULL)
			return ENOMEM;
		flag = copyinstr((userptr_t) arguments[i], kargv[i], PATH_MAX,
				&actual_size);
		if (flag) {
			kfree(kargv);
			kfree(namedes);
			return EFAULT;
		}

	}
	actual_size = 0;

	curthread->t_addrspace = as_create();
	if (curthread->t_addrspace == NULL ) {
		vfs_close(vn);
		return ENOMEM;
	}

	vaddr_t entrypoint, stackptr;
	as_activate(curthread->t_addrspace);
	flag = load_elf(vn, &entrypoint);
	if (flag) {
		// thread_exit destroys curthread->t_addrspace
		vfs_close(vn);
		return flag;
	}

	vfs_close(vn);

	// now set up the user stack with the arguments
	flag = as_define_stack(curthread->t_addrspace, &stackptr);
	if (flag) {
		//thread_exit destroys curthread->t_addrspace
		return flag;
	}

	//i=0;
	vaddr_t stackptrv[numargs + 1];

	for (i = numargs - 1; i >= 0; i--) {
		int len = strlen(kargv[i]);
		int padder=0;
		len++;// to account for string terminator
		//if(len%4!=0)
		padder = len % 4;
		//stackptr=stackptr-(len+padder)
		stackptr =stackptr-( len + (4-padder));

		flag = copyoutstr((const char *) kargv[i], (userptr_t) stackptr, len,
				&actual_size);

		if (flag)
		{
			kfree(kargv);
			kfree(namedes);
			return flag;
		}

		stackptrv[i] = stackptr;
	}
	stackptrv[numargs] = 0;

	//This copies the actual stack addresses of the arguments
	//* into the stack. Hopefully.

	 for(i = numargs; i >= 0; i--)
	 {
	 stackptr -= sizeof(vaddr_t);
	 flag = copyout(&stackptrv[i], (userptr_t)stackptr, sizeof(vaddr_t));

	 if(flag)
	 {
	 kfree(kargv);
	 kfree(namedes);
	 return flag;
	 }
	 }

	enter_new_process(numargs, (userptr_t) stackptr, stackptr, entrypoint);
	//pt_getthread();

	//enter_new_process does not return.
	panic("enter_new_process returned\n");
	return EINVAL;

}

int sys_getpid(pid_t *retval) {
	//kprintf("In get pid:%d\n",curthread->pid);
	//kprintf("In get pid2:%d\n",curthread->proc->pid);

	*retval = curthread->proc->pid;
	return 0;
}

int sys_waitpid(pid_t pid, int* status, int options, pid_t *retval) {
	struct process* wthread = NULL;
	int check;
	int err;
	pid_t i;
	//kprintf("PID in wait pid:%d\n",pid);
	if (status == NULL )
		return EFAULT;
	if (options != 0)
		return EINVAL;
	if (pid == curthread->pid)
		return ECHILD;
	if (pid < PID_MIN || pid > PID_MAX)
		return ESRCH;
	if (status == NULL )
		return EFAULT;
	if (pid == curthread->proc->ppid)
		return ECHILD;
	//int *shit;
	//*shit=(int)&status;
	//kprintf("%d\n",*shit);
	//if(curthread->)
	//if(pid!=curthread->)
	//char* argv=(char*)(status);
	//int length=strlen(argv)+1;
	//int length=strlen(kargv[i])+1;
	//int padder=length%4;
	//kprintf("Hai:%d\n",padder);
	//if(padder!=0)
	//{
	//return EFAULT;
	//}

	//if()

	//char pathbuf[PATH_MAX];
	//int *buffer;
	//int result;

	//result = copyin((const_userptr_t)status, (void*)buffer, sizeof(int));
	//if (result) {
	//return result;
	//}
	//int addr=(int)&status;
	//kprintf("Status:%d\n",addr);
	//int *addr=&status;
	//if(addr%4!=0)
	//{
	//return EFAULT;
	//}
	//kprintf("Got Here\n");
	for (i = 0; i <= 16; i++) {
		if (p_table[i] != NULL ) {
			if (p_table[i]->pid == pid) {
				wthread = p_table[i];
				break;
			}
		}
	}
	if (wthread == NULL ) {
		return ESRCH;
	}
	if (curthread->proc->pid != wthread->ppid)
		return ECHILD;
	//panic("We are here");

	lock_acquire(p_table[i]->tlock);

	//while(!wthread->exited)
	//{
//
	//	cv_wait(p_table[i]->wcv,p_table[i]->tlock);
	//}
	mywait(wthread);

	//copyout((const void*)wthread->exitcode,(userptr_t)status,sizeof(int));
	//kprintf("We are here!");

	check = wthread->exitcode;
	err = copyout(&check, (userptr_t) status, sizeof(check));
	if (err) {
		lock_release(p_table[i]->tlock);
		return err;
	}
	//kprintf("Exit code After waiting:%d\n",*status);
	//kfree(p_table[i]->exitsem);
	//p_table[i]=NULL;

	//panic("After kfree");
	//p_table[i]=NULL;
	lock_release(p_table[i]->tlock);
	//filetable_destroy(p_table[i]->self->t_filetable);
	//lock_destroy(p_table[i]->tlock);
	//cv_destroy(p_table[i]->wcv);
	//kfree(p_table[i]);
	p_table[i] = NULL;

	*retval = pid;
	//panic("After dereferencing");
	return 0;

	//return -1;
	//return 0;
}

void sys_exit(int exitcode) {
	pid_t pid = curthread->proc->pid;
	pid_t i = PID_MIN;
	struct process* ethread=NULL;
//pid_t parent;
//if(pid!=2)
//{
	for (i = 0; i <= 16; i++) {
		if (p_table[i] != NULL ) {
			if (p_table[i]->pid == pid) {
				ethread = p_table[i];
				break;
			}
		}
	}

//parent=ethread->ppid;
	if (ethread != NULL ) {
		lock_acquire(ethread->tlock);
//kprintf("Exit code Before:%d\n",exitcode);

		ethread->exitcode = _MKWAIT_EXIT(exitcode);
//kprintf("Exitcode After:%d\n",ethread->exitcode);
		ethread->exited = 1;
//p_table[i]=ethread;
		cv_broadcast(ethread->wcv, ethread->tlock);
//kfree(curthread->t_filetable);
		lock_release(ethread->tlock);
	}
//}
	i = 0;
//
//kprintf("Before hanging");
//filetable_destroy(curthread->t_filetable);
//kfree(curthread->p);

	thread_exit();
}


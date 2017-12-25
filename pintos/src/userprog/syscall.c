#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

get_user (const uint8_t *uaddr) {
  // checks that a user pointer `uaddr` points below PHYS_BASE
  if (! ((void*)uaddr < PHYS_BASE)) {
    return -1;
  }

  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
      : "=&a" (result) : "m" (*uaddr));
  return result;
}

static int memread_user (void *src, void *dst, size_t bytes){
  int32_t value;
  size_t i;
  for(i=0; i<bytes; i++) {
    value = get_user(src + i);
    if(value == -1)
      printf("memread_user error");
    *(char*)(dst + i) = value & 0xff;
  }
  return (int)bytes;
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

sys_have_job(int lock_name, int time_amount){
  /*implement increasing lock time_amounts*/
}

static void
syscall_handler (struct intr_frame *f) 
{
  int syscall_number;

  memread_user(f->esp, &syscall_number, sizeof(syscall_number));

  if(syscall_number == HAVE_JOB){
    int lock_name, time_amount, return_code;

    memread_user(f->esp + 4, &lock_name, sizeof(lock_name));
    memread_user(f->esp + 8, &time_amount, sizeof(time_amount));

    return_code = sys_have_job(lock_name, time_amount);
    f->eax = return_code;
  }

  thread_exit ();
}
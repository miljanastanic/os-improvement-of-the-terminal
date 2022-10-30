// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
	char *whichDev[] = {"/dev/tty1", "/dev/tty2", "/dev/tty3", "/dev/tty4", "/dev/tty5", "/dev/tty6"};

	int pid1, pid2, pid3, pid4, pid5, pid6;
	int wpid1, wpid2, wpid3, wpid4, wpid5, wpid6;

	int pid[6] = {pid1, pid2, pid3, pid4, pid5, pid6};
	int wpid[6] = {wpid1, wpid2, wpid3, wpid4, wpid5, wpid6};

	int n = 6, wpidl;

	if(getpid() != 1){
		fprintf(2, "init: already running\n");
		exit();
	}

	if(open("/dev/console", O_RDWR) < 0){
		mknod("/dev/console", 1, 1);
		open("/dev/console", O_RDWR);
	}
	dup(0);  // stdout
	dup(0);  // stderr

	for(int i=0; i<n ;i++){

		pid[i] = fork();
		if(pid[i] < 0){
			printf("init: fork failed\n");
			exit();
		}
		if(pid[i] == 0){
			close(0);
			close(1);
			close(2);
			if(open(whichDev[i], O_RDWR) < 0){
				mknod(whichDev[i], 1, (i+1));
				open(whichDev[i], O_RDWR);
			}
			dup(0);  // stdout
			dup(0);  // stderr
			
			printf("init: starting sh on %s\n", whichDev[i]);
			exec("/bin/sh", argv);
			printf("init: exec sh failed\n");
			exit();
		}
	
	}
		
	
	for(;;){
		for(int i=0; i<n; i++){
			while((wpid[i]=wait()) >= 0 && wpid[i] != pid[i]) // 6 ifa za wpid npr wpid1 == pid1
				printf("zombie!\n");
		}
	}
	

}

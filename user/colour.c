#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	int fd, i;
	char *str1 = argv[1];
	char *stc = argv[2];

	char *str2 = "-fg"; // fg = 20
	char *str3 = "--foreground";

	char *str4 = "-bg"; //bg = 30
	char *str5 = "--background";

	char *str6 = "-h";
	char *str7 = "--help";

	char *str8 = "reset"; // reset = 40
	
	int fgcolors[8]  = { 0x0000, 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700 };
	int fglcolors[8] = { 0x0800, 0x0900, 0x0A00, 0x0B00, 0x0C00, 0x0D00, 0x0E00, 0x0F00 };

	int bgcolors[8]  = { 0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000 };
	int bglcolors[8] = { 0x8000, 0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000 };

	char *black = "black";
	char *blue = "blue";
	char *green = "green";
	char *aqua = "aqua";
	char *red = "red";
	char *purple = "purple";
	char *yellow = "yellow";
	char *white = "white";

	char *black1 = "Lblack";
	char *blue1 = "Lblue";
	char *green1 = "Lgreen";
	char *aqua1 = "Laqua";
	char *red1 = "Lred";
	char *purple1 = "Lpurple";
	char *yellow1 = "Lyellow";
	char *white1 = "Lwhite";

	if(!(strcmp(str1, str6)) || !(strcmp(str1, str7))){
		printf("Usage: colour [OPTION]...\n\nCommand line options:\n    -h, --help:        Show help prompt.\n    -bg, --background: Change terminal background color.\n");
		printf("    -fg, --foreground: Change terminal foreground color.\n    -reset:            Reset terminal color.\n");
		exit();
	}
	else if(!strcmp(str1, str8)){
		int reset = 40;
		colour(reset, 0);	
		exit();
	}
	else if(!(strcmp(str1, str2)) || !(strcmp(str1, str3))){
		if(!strcmp(stc, black)){
			colour(fgcolors[0], 20); 
			exit();
		}
		if(!strcmp(stc, blue)){
			colour(fgcolors[1], 20);
			exit();
		}
		if(!strcmp(stc, green)){
			colour(fgcolors[2], 20);
			exit();
		}
		if(!strcmp(stc, aqua)){
			colour(fgcolors[3], 20);
			exit();
		}
		if(!strcmp(stc, red)){
			colour(fgcolors[4], 20);
			exit();
		}
		if(!strcmp(stc, purple)){
			colour(fgcolors[5], 20);
			exit();
		}
		if(!strcmp(stc, yellow)){
			colour(fgcolors[6], 20);
			exit();
		}
		if(!strcmp(stc, white)){
			colour(fgcolors[7], 20);
			exit();
		}
		if(!strcmp(stc, black1)){
			colour(fglcolors[0], 20);
			exit();
		}
		if(!strcmp(stc, blue1)){
			colour(fglcolors[1], 20);
			exit();
		}
		if(!strcmp(stc, green1)){
			colour(fglcolors[2], 20);
			exit();
		}
		if(!strcmp(stc, aqua1)){
			colour(fglcolors[3], 20);
			exit();
		}
		if(!strcmp(stc, red1)){
			colour(fglcolors[4], 20);
			exit();
		}
		if(!strcmp(stc, purple1)){
			colour(fglcolors[5], 20);
			exit();
		}
		if(!strcmp(stc, yellow1)){
			colour(fglcolors[6], 20);
			exit();
		}
		if(!strcmp(stc, white1)){
			colour(fglcolors[7], 20);
			exit();
		}
	}
	else if(!(strcmp(str1, str4)) || !(strcmp(str1, str5))){
		if(!strcmp(stc, black)){
			colour(bgcolors[0], 30);
			exit();
		}
		if(!strcmp(stc, blue)){
			colour(bgcolors[1], 30);
			exit();
		}
		if(!strcmp(stc, green)){
			colour(bgcolors[2], 30);
			exit();
		}
		if(!strcmp(stc, aqua)){
			colour(bgcolors[3], 30);
			exit();
		}
		if(!strcmp(stc, red)){
			colour(bgcolors[4], 30);
			exit();
		}
		if(!strcmp(stc, purple)){
			colour(bgcolors[5], 30);
			exit();
		}
		if(!strcmp(stc, yellow)){
			colour(bgcolors[6], 30);
			exit();
		}
		if(!strcmp(stc, white)){
			colour(bgcolors[7], 30);
			exit();
		}
		if(!strcmp(stc, black1)){
			colour(bglcolors[0], 30);
			exit();
		}
		if(!strcmp(stc, blue1)){
			colour(bglcolors[1], 30);
			exit();
		}
		if(!strcmp(stc, green1)){
			colour(bglcolors[2], 30);
			exit();
		}
		if(!strcmp(stc, aqua1)){
			colour(bglcolors[3], 30);
			exit();
		}
		if(!strcmp(stc, red1)){
			colour(bglcolors[4], 30);
			exit();
		}
		if(!strcmp(stc, purple1)){
			colour(bglcolors[5], 30);
			exit();
		}
		if(!strcmp(stc, yellow1)){
			colour(bglcolors[6], 30);
			exit();
		}
		if(!strcmp(stc, white1)){
			colour(bglcolors[7], 30);
			exit();
		} 
	}else{
		if(str1[0] == '0' && str1[1] == 'x'){
			int n = atoi(str1);
			colour(n, 50);
			exit();
		}else{
			printf("Unesite heksadecimalnu vrednost boje");
		}
	exit();
	}
}

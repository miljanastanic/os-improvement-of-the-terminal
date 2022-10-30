// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "history.h"

#define KEY_UP          0xE2
#define KEY_DN          0xE3


char *history1[8]={history11, history12, history13, history14, history15, history16, history17, history18};
char *history2[8]={history21, history22, history23, history24, history25, history26, history27, history28};
char *history3[8]={history31, history32, history33, history34, history35, history36, history37, history38};
char *history4[8]={history41, history42, history43, history44, history45, history46, history47, history48};
char *history5[8]={history51, history52, history53, history54, history55, history56, history57, history58};
char *history6[8]={history61, history62, history63, history64, history65, history66, history67, history68};

char terminal1[25*80];
char terminal2[25*80];
char terminal3[25*80];
char terminal4[25*80];
char terminal5[25*80];
char terminal6[25*80];

char tty1[7] = {'[','t', 't', 'y', ' ', '1', ']', '\0'};
char tty2[7] = {'[','t', 't', 'y', ' ', '2', ']', '\0'};
char tty3[7] = {'[','t', 't', 'y', ' ', '3', ']', '\0'};
char tty4[7] = {'[','t', 't', 'y', ' ', '4', ']', '\0'};
char tty5[7] = {'[','t', 't', 'y', ' ', '5', ']', '\0'};
char tty6[7] = {'[','t', 't', 'y', ' ', '6', ']', '\0'};

int color[7] = { 0x0700, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600 };

int currColor1 = 0x0700; 
int currColor2 = 0x0200; 
int currColor3 = 0x0300;
int currColor4 = 0x0400; 
int currColor5 = 0x0500; 
int currColor6 = 0x0600;

static uchar *terminal[6] = {
		terminal1, terminal2, terminal3, terminal4, terminal5, terminal6
	};

int currTerminal = 1;
int currColor = 0x0700;
int currFlag = 0;

static void consputc(int);

static int panicked = 0;

static struct {
	struct spinlock lock;
	int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
	static char digits[] = "0123456789abcdef";
	char buf[16];
	int i;
	uint x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);

	if(sign)
		buf[i++] = '-';

	while(--i >= 0)
		consputc(buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
	int i, c, locking;
	uint *argp;
	char *s;

	locking = cons.locking;
	if(locking)
		acquire(&cons.lock);

	if (fmt == 0)
		panic("null fmt");

	argp = (uint*)(void*)(&fmt + 1);
	for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
		if(c != '%'){
			consputc(c);
			continue;
		}
		c = fmt[++i] & 0xff;
		if(c == 0)
			break;
		switch(c){
		case 'd':
			printint(*argp++, 10, 1);
			break;
		case 'x':
		case 'p':
			printint(*argp++, 16, 0);
			break;
		case 's':
			if((s = (char*)*argp++) == 0)
				s = "(null)";
			for(; *s; s++)
				consputc(*s);
			break;
		case '%':
			consputc('%');
			break;
		default:
			// Print unknown % sequence to draw attention.
			consputc('%');
			consputc(c);
			break;
		}
	}

	if(locking)
		release(&cons.lock);
}

void
panic(char *s)
{
	int i;
	uint pcs[10];

	cli();
	cons.locking = 0;
	// use lapiccpunum so that we can call panic from mycpu()
	cprintf("lapicid %d: panic: ", lapicid());
	cprintf(s);
	cprintf("\n");
	getcallerpcs(&s, pcs);
	for(i=0; i<10; i++)
		cprintf(" %p", pcs[i]);
	panicked = 1; // freeze other CPU
	for(;;)
		;
}

#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory


//.................sistemski.poziv.colour........................................

int
sys_colour(void)
{
	int colour;
	int flag; // flag = 20 FG, flag = 30 BG, reset = 40
	argint(0, &colour);
	argint(1, &flag);

	if(flag == 50){
		if(currTerminal == 1){
			currColor1 = colour;
			oboji(1);
		}
	}

	if(flag == 20){
		if(currTerminal == 1){
			currColor1 = ((currColor1 & 0xf000) | colour);
			oboji(1);
		}
		if(currTerminal == 2){
			currColor2 = ((currColor2 & 0xf000) | colour);
			oboji(2);
		}
		if(currTerminal == 3){
			currColor3 = ((currColor3 & 0xf000) | colour);
			oboji(3);
		}
		if(currTerminal == 4){
			currColor4 = ((currColor4 & 0xf000) | colour);
			oboji(4);
		}
		if(currTerminal == 5){
			currColor5 = ((currColor5 & 0xf000) | colour);
			oboji(5);
		}
		if(currTerminal == 6){
			currColor6 = ((currColor6 & 0xf000) | colour);
			oboji(6);
		}
	}
	if(flag == 30){
		if(currTerminal == 1){
			currColor1 = ((currColor1 & 0x0f00) | colour);
			oboji(1);
		}
		if(currTerminal == 2){
			currColor2 = ((currColor2 & 0x0f00) | colour);
			oboji(2);
		}
		if(currTerminal == 3){
			currColor3 = ((currColor3 & 0x0f00) | colour);
			oboji(3);
		}
		if(currTerminal == 4){
			currColor4 = ((currColor4 & 0x0f00) | colour);
			oboji(4);
		}
		if(currTerminal == 5){
			currColor5 = ((currColor5 & 0x0f00) | colour);
			oboji(5);
		}
		if(currTerminal == 6){
			currColor6 = ((currColor6 & 0x0f00) | colour);
			oboji(6);
		}
	}
	if(colour == 40 && flag == 0){
		if(currTerminal == 1){
			currColor1 = color[0];
			oboji(currTerminal);
		}
		if(currTerminal == 2){
			currColor2 = color[1];
			oboji(currTerminal);
		}
		if(currTerminal == 3){
			currColor3 = color[2];
			oboji(currTerminal);
		}
		if(currTerminal == 4){
			currColor4 = color[3];
			oboji(currTerminal);
		}
		if(currTerminal == 5){
			currColor5 = color[4];
			oboji(currTerminal);			
		}
		if(currTerminal == 6){
			currColor6 = color[5];
			oboji(currTerminal);
		}
	}
}

//..............................................................................
int brT1, brT2, brT3, brT4, brT5, brT6; 
int flag = 0; 
int posHistory1 = 0;
int count1 = 0;
int count2 = 0;
int pd;

static void
cgaputc(int c)
{
	int pos;
	char temp[50];
	int c1 = count1;

	// Cursor position: col + 80*row.
	outb(CRTPORT, 14);
	pos = inb(CRTPORT+1) << 8;
	outb(CRTPORT, 15);
	pos |= inb(CRTPORT+1);
	
	if(currFlag == 1){
		pos = 0;
		currFlag--;
	}
	
	posHistory1 = pos;
	int h = pos-(pos%80) - (80*2) + 1; //?????????????????????????????????????????????????
	if(c == '\n'){
		if(currTerminal == 1){
			for(int i = 0; i<50; i++){
				history1[c1][i] = crt[h++];
			}
			//count1+=1;
		}
		pos += 80 - pos%80;
	}else if(c == BACKSPACE){
		if(pos > 0) --pos;
			if(currTerminal == 1){
				if(brT1 > 0) --brT1;
			}
			if(currTerminal == 2){
				if(brT2 > 0) --brT2;
			}
			if(currTerminal == 3){
				if(brT3 > 0) --brT3;
			}
			if(currTerminal == 4){
				if(brT4 > 0) --brT4;
			}
			if(currTerminal == 5){
				if(brT5 > 0) --brT5;
			}
			if(currTerminal == 6){
				if(brT6 > 0) --brT6;
		}

	} else
		if(currTerminal == 1){
			crt[pos++] = (c&0xff) | currColor1;  // black on white //desava se na trenutnom terminalu!!!!!
		}
		if(currTerminal == 2){
			crt[pos++] = (c&0xff) | currColor2;
		}
		if(currTerminal == 3){
			crt[pos++] = (c&0xff) | currColor3;
		}
		if(currTerminal == 4){
			crt[pos++] = (c&0xff) | currColor4;
		}
		if(currTerminal == 5){
			crt[pos++] = (c&0xff) | currColor5;
		}
		if(currTerminal == 6){
			crt[pos++] = (c&0xff) | currColor6;
		}

		if(flag == 0){
			if(currTerminal == 1 ){
				terminal1[brT1++] = (c&0xff) | currColor1; //Korisnikovo pisnanje mora da se desava na trenutnom terminalu, ali takodje mora da se upisuje u pomocni niz terminala1
			}
			if(currTerminal == 2 ){
				terminal2[brT2++] = (c&0xff) | currColor2;
			}
			if(currTerminal == 3 ){
				terminal3[brT3++] = (c&0xff) | currColor3;
			}
			if(currTerminal == 4 ){
				terminal4[brT4++] = (c&0xff) | currColor4;
			}
			if(currTerminal == 5 ){
				terminal5[brT5++] = (c&0xff) | currColor5;
			}
			if(currTerminal == 6 ){
				terminal6[brT6++] = (c&0xff) | currColor6;
			}
		}

	if(pos < 0 || pos > 25*80)
		panic("pos under/overflow");

	if((pos/80) >= 24){  // Scroll up.
		memmove(crt, crt+80, sizeof(crt[0])*23*80);
		pos -= 80;
		memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
		for(int i = 0 ; i<80; i++){
			if(currTerminal == 1){
				crt[i+1840] = crt[i+1840] | currColor1;
			}
			if(currTerminal == 2){
				crt[i+1840] = crt[i+1840] | currColor2;
			}
			if(currTerminal == 3){
				crt[i+1840] = crt[i+1840] | currColor3;
			}
			if(currTerminal == 4){
				crt[i+1840] = crt[i+1840] | currColor4;
			}
			if(currTerminal == 5){
				crt[i+1840] = crt[i+1840] | currColor5;
			}
			if(currTerminal == 6){
				crt[i+1840] = crt[i+1840] | currColor6;
			}
			
		}
	}

	outb(CRTPORT, 14);
	outb(CRTPORT+1, pos>>8);
	outb(CRTPORT, 15);
	outb(CRTPORT+1, pos);

	if(currTerminal == 1){
		crt[pos] = ' ' | currColor1;
	}
	if(currTerminal == 2){
		crt[pos] = ' ' | currColor2;
	}
	if(currTerminal == 3){
		crt[pos] = ' ' | currColor3;
	}
	if(currTerminal == 4){
		crt[pos] = ' ' | currColor4;
	}
	if(currTerminal == 5){
		crt[pos] = ' ' | currColor5;
	}
	if(currTerminal == 6){
		crt[pos] = ' ' | currColor6;
	}

}

void
consputc(int c)
{
	if(panicked){
		cli();
		for(;;)
			;
	}

	if(c == BACKSPACE){
		uartputc('\b'); uartputc(' '); uartputc('\b');
	} else
		uartputc(c);
	cgaputc(c);
}

#define INPUT_BUF 128
struct {
	char buf[INPUT_BUF];
	uint r;  // Read index
	uint w;  // Write index
	uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x
#define ALTKBD(y) (y - '0')

void
consoleintr(int (*getc)(void))
{
	int c, doprocdump = 0;	
	int br1,br2,br3,br4,br5,br6;

	acquire(&cons.lock);
	while((c = getc()) >= 0){
		switch(c){
		case C('P'):  // Process listing.
			// procdump() locks cons.lock indirectly; invoke later
			doprocdump = 1;
			break;
		case C('U'):  // Kill line.
			while(input.e != input.w &&
			      input.buf[(input.e-1) % INPUT_BUF] != '\n'){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case C('H'): case '\x7f':  // Backspace
			if(input.e != input.w){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case ALTKBD('1'):
			br1=0;
			currFlag = 1;
			currTerminal = 1;

			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor1;
			}
			flag = 1;
			for(int i=0; i<brT1; i++){
				consputc(terminal1[i]);
			} 
			flag = 0;
			for(int i=1912; i<1920; i++){
				crt[i] = tty1[br1++] | currColor1;
			}
			break;
		case ALTKBD('2'):
			br2 = 0;
			currFlag = 1;
			currTerminal = 2;
			
			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor2;
			}
			flag = 1;
			for(int i=0; i<brT2; i++){
				consputc(terminal2[i]);
			} 
			for(int i=1912; i<1920; i++){
				crt[i] = tty2[br2++] | currColor2;
			}
			flag = 0;
			break;
		case ALTKBD('3'):
			br3 = 0;
			currFlag = 1;
			currTerminal = 3;

			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor3;
			}
			flag = 1;
			for(int i=0; i<brT3; i++){
				consputc(terminal3[i]);
			} 
			for(int i=1912; i<1920; i++){
				crt[i] = tty3[br3++] | currColor3;
			}
			flag = 0;
			break;
		case ALTKBD('4'):
			br4 = 0;
			currFlag = 1;
			currTerminal = 4;

			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor4;
			}
			flag = 1;
			for(int i=0; i<brT4; i++){
				consputc(terminal4[i]);
			} 
			for(int i=1912; i<1920; i++){
				crt[i] = tty4[br4++] | currColor4;
			}
			flag = 0;
			break;
		case ALTKBD('5'):
			br5 = 0;
			currFlag = 1;
			currTerminal = 5;

			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor5;
			}
			flag = 1;
			for(int i=0; i<brT5; i++){
				consputc(terminal5[i]);
			}
			for(int i=1912; i<1920; i++){
				crt[i] = tty5[br5++] | currColor5;
			}
			flag = 0;
			break;
		case ALTKBD('6'):
			br6 = 0;
			currFlag = 1;
			currTerminal = 6;

			for(int i=0; i<25*80; i++){
				crt[i] = 0 | currColor3;
			}
			flag = 1;
			for(int i=0; i<brT6; i++){
				consputc(terminal6[i]);
			} 
			for(int i=1912; i<1920; i++){
				crt[i] = tty6[br6++] | currColor6;
			}
			flag = 0;
			break;
		case KEY_UP:
			if(currTerminal == 1){
				for(int i=0; i<50; i++){
					crt[i] = history1[0][i] | currColor1;
				}
			}
			count2++;
			break;
		case KEY_DN:
			pd = count1;
			if(currTerminal == 1){
				for(int i=0; i<50; i++){
					crt[i] = history1[pd][i] | currColor1;
				}
			}
			pd--;
			break;

		default:
			if(c != 0 && input.e-input.r < INPUT_BUF){
				c = (c == '\r') ? '\n' : c;
				input.buf[input.e++ % INPUT_BUF] = c;
				consputc(c);
				if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
					input.w = input.e;
					wakeup(currTerminal);
				}
			}
			break;
		}
	}
	release(&cons.lock);
	if(doprocdump) {
		procdump();  // now call procdump() wo. cons.lock held
	}
}

void oboji(int currTerminal){
	
	for(int i=0; i<2000; i++){
		if(currTerminal == 1){
			crt[i]=(crt[i] & 0xff) | currColor1;
		}
		if(currTerminal == 2){
			crt[i]=(crt[i] & 0xff) | currColor2;
		}
		if(currTerminal == 3){
			crt[i]=(crt[i] & 0xff) | currColor3;
		}
		if(currTerminal == 4){
			crt[i]=(crt[i] & 0xff) | currColor4;
		}
		if(currTerminal == 5){
			crt[i]=(crt[i] & 0xff) | currColor5;
		}
		if(currTerminal == 6){
			crt[i]=(crt[i] & 0xff) | currColor6;
		}
	}
}
int
consoleread(struct inode *ip, char *dst, int n)
{
	uint target;
	int c;

	iunlock(ip);
	target = n;
	acquire(&cons.lock);
	while(n > 0){
		while(input.r == input.w){
			if(myproc()->killed){
				release(&cons.lock);
				ilock(ip);
				return -1;
			}
			sleep(ip->minor, &cons.lock);
		}
		c = input.buf[input.r++ % INPUT_BUF];
		if(c == C('D')){  // EOF
			if(n < target){
				// Save ^D for next time, to make sure
				// caller gets a 0-byte result.
				input.r--;
			}
			break;
		}
		*dst++ = c;
		--n;
		if(c == '\n')
			break;
	}
	release(&cons.lock);
	ilock(ip);

	return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
	int i;

	switch(ip->minor){	
		case 1: // mora takodje da ima 6 if-a gde ce se uporedjivati minor broj da bi znali koji proces treba da se izvrsava i za koji terminal zovemo consputc()
			if(!(currTerminal == 1)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal1[brT1++] = (buf[i] & 0xff) | currColor1;
				}
				release(&cons.lock);
				ilock(ip);
			}	
			break;
		case 2:
			if(!(currTerminal == 2)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal2[brT2++] = (buf[i] & 0xff) | currColor2;
				}
				release(&cons.lock);
				ilock(ip);
			}
			break;
		case 3:
			if(!(currTerminal == 3)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal3[brT3++] = (buf[i] & 0xff) | currColor3;
				}
				release(&cons.lock);
				ilock(ip);
			}
			break;
		case 4:
			if(!(currTerminal == 4)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal4[brT4++] = (buf[i] & 0xff) | currColor4;
				}
				release(&cons.lock);
				ilock(ip);
			}
			break;
		case 5:
			if(!(currTerminal == 5)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal5[brT5++] = (buf[i] & 0xff) | currColor5;
				}	
				release(&cons.lock);
				ilock(ip);
			}
			break;
		case 6:
			if(!(currTerminal == 6)){
				iunlock(ip);
				acquire(&cons.lock);
				for(i = 0; i < n; i++){
					terminal6[brT6++] = (buf[i] & 0xff) | currColor6;
				}
				release(&cons.lock);
				ilock(ip);
			}
			break;
	}
	
    	iunlock(ip);
    	acquire(&cons.lock);
    	if(ip->minor == currTerminal){
		for(i = 0; i < n; i++)
        		consputc(buf[i] & 0xff);
	}
   	release(&cons.lock);
    	ilock(ip);

	return n;
}

void
consoleinit(void)
{
	initlock(&cons.lock, "console");

	devsw[CONSOLE].write = consolewrite;
	devsw[CONSOLE].read = consoleread;
	cons.locking = 1;

	ioapicenable(IRQ_KBD, 0);
}


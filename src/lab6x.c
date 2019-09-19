#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>

const int BACKGROUND_ATTR   =   0; //00000000;
const int ERROR_ATTR        = 116; //01110100;
const int SUCCESS_ATTR      = 114; //01110010;
const int SIMPLE_ATTR       = 113; //01110001;
const int SIMPLE_INV_ATTR   =  23; //00010111;

const int MAX_CURSOR_VALUE = 80 * 25;
const int CURSOR_INC = 4;

typedef void interrupt far (*Interrupt)(void);

struct VIDEO {
	unsigned char symb;
	unsigned char attr;
};

void interrupt far keyboardHandler(void);

int running = 1;
int blinking = 0;
int commandSuccess = 0;
int commandProcessed = 0;
int cur_cursor_pos = 81;
int last_code = 0;

void blink(void);
int sendMask(int mask);
int sendByte(int byte);
void print_code(int hex_code, unsigned char attr);
unsigned char hex_to_char(int hex);
void clear_screen();
void inc_cursor();

int main() {
	Interrupt oldKeyboard;

	srand(time(NULL));
	clear_screen();
	puts("Press F to blink and ESC to exit");
	
	_disable();

	oldKeyboard = getvect(0x09);
	setvect(0x09, keyboardHandler);
		
	_enable();
	
	while (running) {
		if (blinking) {
			blink();
		}
	}
	
	_disable();

	setvect(0x09, oldKeyboard);
	_enable();
	
	return 0;	
}

void interrupt far keyboardHandler(void) {

	int code = inp(0x60);
	int attr;
	
	switch (code) {
	    case 0xFA: 
	        attr = SUCCESS_ATTR; 
	        break;
	    case 0xFE:
	    case 0xFF:
	        attr = ERROR_ATTR;
	        break;
	    default:
	        if (code == last_code + 0x80) attr = SIMPLE_INV_ATTR;
	        else attr = SIMPLE_ATTR;
	        break;
	}
	
	last_code = code;
	print_code(code, attr);
	
	switch (code) {
		
		// 'ESC'
		case 0x01: running = 0; break;
		
		// 'F'	
		case 0x21: blinking = !blinking; break;
	
	    // ERROR
		case 0xFE:
			commandSuccess = 0;
			commandProcessed = 1;
			break;
	    
	    // SUCCESS
		case 0xFA:
			commandSuccess = 1;
			commandProcessed = 1;
			break;
	}
	
	outp(0x20, 0x20);
}

void blink() {
	int mask = rand() % 0x08;
	if (!sendMask(mask)) {
		running = 0;
		return;
	}
	delay(100);
}

int sendMask(int mask) {
	return sendByte(0xED) ? sendByte(mask) : 0;
}

int sendByte(int byte) {
	int errorCount;
	for (
		errorCount = commandSuccess = 0;
		errorCount < 3 && !commandSuccess;
		++errorCount
	) {
		commandProcessed = 0;
		while (inp(0x64) & 0x02);           
		outp(0x60, byte);
		while(!commandProcessed);
	}
	return errorCount < 3;
}

unsigned char hex_to_char(int hex) {
    return (hex % 16 > 9) ? (hex % 16 - 10 + 'A') : (hex % 16 + '0');
}

void clear_screen() {

    int i;
    int j;
    
    struct VIDEO far* screen = (struct VIDEO far *) MK_FP(0xB800, NULL);
    struct VIDEO empty;
    
    empty.attr = BACKGROUND_ATTR;
    empty.symb = ' '; 
    
    for (i = 0; i < 25; i++) {
        for (j = 0; j < 80; j++) {
            *(screen++) = empty;
        }
    } 
}

void inc_cursor() {
    cur_cursor_pos += CURSOR_INC;
}

void print_code(int hex_code, unsigned char attr) {

    struct VIDEO far* screen = (struct VIDEO far *) MK_FP(0xB800, NULL);
    
    struct VIDEO first;
    struct VIDEO second;
    
    screen += cur_cursor_pos;
    
    first.attr = attr;
    second.attr = attr;
    
    hex_code %= 256;
    second.symb = hex_to_char(hex_code % 16);
    hex_code /= 16;
    first.symb = hex_to_char(hex_code % 16);
    
    *screen = first;
    screen++;
    *screen = second;
    
    if (cur_cursor_pos < MAX_CURSOR_VALUE) {
        inc_cursor();
    }
    else {
        clear_screen();
        cur_cursor_pos = 0;
    }
    
}


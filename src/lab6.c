#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <stdlib.h>

#define STDOUT 1

typedef void interrupt far (*Interrupt)(void);

void interrupt far keyboardHandler(void);

int running = 1;
int blinking = 0;
int commandSuccess = 0;
int commandProcessed = 0;

void blink(void);
int sendMask(int mask);

int main() {
	Interrupt oldKeyboard;

	srand(time(NULL));
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
	char str[5];

	sprintf(str, "%02X  ", code);
	
	write(STDOUT, str, 4);
	
	switch (code) {
		
		// 'ESC'
		case 0x01: 
		    running = 0; 
		    break;
		
		// 'F'	
		case 0x21: 
		    blinking = !blinking; 
		    break;
	
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


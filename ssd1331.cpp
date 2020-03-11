#include <SPI.h>
#include <Wire.h>
#include <stdio.h>
#include "ssd1331.h"

void command(uint8_t cmd){
	digitalWrite(PIN_DC, LOW);
	SPIWrite(&cmd, 1);
}

void SPIWrite(uint8_t *buffer, int bufferLength) {
	int i;
	for (i = 0; i < bufferLength; i++){
		SPI.transfer(buffer[i]);
		//Serial.println(buffer[i]);
	}
}

void displayBegin() {
	pinMode(PIN_RES, OUTPUT);
	pinMode(PIN_DC, OUTPUT);
	pinMode(PIN_CS, OUTPUT);

	SPI.begin();

	digitalWrite(PIN_RES, HIGH);
	delay(10);
	digitalWrite(PIN_RES, LOW);
	delay(10);
	digitalWrite(PIN_RES, HIGH);

	command(0xAE); //Display off.
	command(0x81); //Set contrast for color A.
	command(0xFF); //145
	command(0x82); //Set contrast for color B.
	command(0xFF); //80
	command(0x83); //Set contracr for color C.
	command(0xFF); //125
	command(0x87); //Master current control.
	command(0x06); //6
	command(0x8A); //Set pre-change speed for color A.
	command(0x64); //100
	command(0x8B); //Set pre-change speed for color B.
	command(0x78); //120
	command(0x8C); //Set pre-change speed for color C.
	command(0x64); //100
	command(0xA0); //Set remap and data format.
	command(0x72); //--
	command(0xA1); //Set display start line.
	command(0x0);
	command(0xA2); //Set display offset.
	command(0x0);
	command(0xA4); //Set display mode. [nor-A4, edn-A5, edf-A6]
	command(0xA8); //Set multiplex ratio.
	command(0x3F);
	command(0xAD); //Set master config.
	command(0x8E);
	command(0xB0); //Set Power Save Mode.
	command(0x00); //--
	command(0xB1); //Phase 1 and 2 period adjusment.
	command(0x31); //--
	command(0xB3); //Display clock divider/oscillator frequency. B3
	command(0xF0);
	command(0xBB); //Set pre-change level.
	command(0x3A);
	command(0xBE); //Set Voltage (vcomH).
	command(0x3E);
	command(0x2E); //Disable scrolling. [deactive scrolling]
	command(0xAF); //Set display on. [normal bright]
}

void drawPoint(int x, int y, unsigned short color) {
	unsigned char buffer[2];
	if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
	command(0x15);	   //Set column address
	command(x);		   //start address
	command(OLED_WIDTH - 1); //end address
	command(0x75);	   //Set row address
	command(y);		   //page start address
	command(OLED_HEIGHT - 1); //page end address
	digitalWrite(PIN_DC, HIGH);

	buffer[0] = (color & 0xFF00) >> 8;
	buffer[1] = color & 0xFF;
	SPIWrite(buffer, 2);
}

void clearDisplay() {
	int i, j;
	for (i = 0; i < OLED_WIDTH; i++)
		for (j = 0; j < OLED_WIDTH; j++)
			drawPoint(j, i, BLACK);
}

void setMonoBitmap(unsigned char x, unsigned char y,
				   const unsigned char *bmp, char cw, char ch, unsigned short color) {
	unsigned char i, j, bw = (cw + 7) / 8;
	for (j = 0; j < ch; j++)
		for (i = 0; i < cw; i++)
			if (pgm_read_byte(bmp + j * bw + i / 8) & (128 >> (i & 7)))
				drawPoint(x + i, y + j, color);
}

static void mchar(unsigned char x, unsigned char y, char acsii,
				  char size, char mode, unsigned short color) {
	unsigned char i, j, y0 = y;
	char temp;
	unsigned char ch = acsii - ' ';
	for (i = 0; i < size; i++) {
		if (size == 12) {
			if (mode)
				temp = pgm_read_byte(&Font1206[ch][i]);
			else
				temp = ~pgm_read_byte(&Font1206[ch][i]);
		} else {
			if (mode)
				temp = pgm_read_byte(&Font1608[ch][i]);
			else
				temp = ~pgm_read_byte(&Font1608[ch][i]);
		}

		for (j = 0; j < 8; j++) {
			if (temp & 0x80)
				drawPoint(x, y, color);
			else
				drawPoint(x, y, 0); //BLACK BACKGROUND
			temp <<= 1;
			y++;
			if ((y - y0) == size) {
				y = y0;
				x ++;
				break;
			}
		}
	}
}

void displayPrint(unsigned char x, unsigned char y, const char *pString, unsigned char Size, unsigned char Mode, unsigned short hwColor)
{
	while (*pString != '\0') {
		if (x > (OLED_WIDTH - Size / 2)) {
			x = 0;
			y += Size;
			if (y > (OLED_HEIGHT - Size)) {
				y = x = 0;
			}
		}

		mchar(x, y, *pString, Size, Mode, hwColor);
		x += Size / 2;
		pString ++;
	}
}

void displayPrint(unsigned char x, unsigned char y, int num, unsigned char Size, unsigned char Mode, unsigned short hwColor) {
	int n = log10(num) + 1;
	String b = "";
	for (int i = 0; i < n; i++, num /= 10) {
		b += num % 10;
	}
	String p = "";
	for (int i = b.length() - 1; i >= 0; i--) {
		p += b.charAt(i);
	}
	const char* o = p.c_str();
	displayPrint(x, y, o, Size, Mode, hwColor);
}

void plotLineLow(int x0, int y0, int x1, int y1, unsigned short hwColor) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}

	int D = 2 * dy - dx;
	int y = y0;

	for (int x = x0; x <= x1; x++) {
		//plot(x,y);
		drawPoint(x, y, hwColor);
		if (D > 0) {
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
}

void plotLineHigh(int x0, int y0, int x1, int y1, unsigned short hwColor) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	int D = 2 * dx - dy;
	int x = x0;

	for (int y = y0; y <= y1; y++) {
		//plot(x,y);
		drawPoint(x, y, hwColor);
		if (D > 0) {
			x = x + xi;
			D = D - 2 * dy;
		}
		D = D + 2 * dx;
	}
}

void plotLine(int x0, int y0, int x1, int y1, unsigned short hwColor) {
	if (abs(y1 - y0) < abs(x1 - x0))
		if (x0 > x1)
			plotLineLow(x1, y1, x0, y0, hwColor);
		else
			plotLineLow(x0, y0, x1, y1, hwColor);
	else if (y0 > y1)
		plotLineHigh(x1, y1, x0, y0, hwColor);
	else
		plotLineHigh(x0, y0, x1, y1, hwColor);
}

void drawCircle(int x0, int y0, int r, unsigned char color) {
	int f  = 1 - r;
	int dx = 1;
	int dy = -2 * r;
	int x  = 0;
	int y  = r;

	drawPoint(  x0, y0 + r, color);
	drawPoint(  x0, y0 - r, color);
	drawPoint(x0 + r,   y0, color);
	drawPoint(x0 - r,   y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			dy += 2;
			f += dy;
		}
		x++;
		dx += 2;
		f += dx;

		drawPoint(x0 + x, y0 + y, color);
		drawPoint(x0 - x, y0 + y, color);
		drawPoint(x0 + x, y0 - y, color);
		drawPoint(x0 - x, y0 - y, color);
		drawPoint(x0 + y, y0 + x, color);
		drawPoint(x0 - y, y0 + x, color);
		drawPoint(x0 + y, y0 - x, color);
		drawPoint(x0 - y, y0 - x, color);
	}	}

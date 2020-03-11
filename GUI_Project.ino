#include <SPI.h>
#include <Wire.h>
#include "ssd1331.h"
#include <string.h>
#include <EEPROM.h>

#define OLED_WIDTH 96
#define OLED_HEIGHT 64
#define PIN_RES 9
#define PIN_DC 8
#define PIN_CS 10
#define PIN_DIN 11
#define PIN_CLK 13

#define RPT_METER A1
int sensorValue = 0;

uint8_t oled_buf[OLED_WIDTH * OLED_HEIGHT / 8];

void scrollBar();
int text_align_center(int container_width, int ch_width, int str_length);

void setup() {
	Serial.begin(9600);
	displayBegin();
	clearDisplay();
	setMonoBitmap(0, 0, my_logo, 96, 64, PURPLE);
	delay(100);
	clearDisplay();
	

	displayPrint(text_align_center(96, 6, 6), 0, "Unlock", 12, true, GREEN);


	//plotLine(5, 0, 5, 10, BLUE);


	//scrollBar();
}

void loop() {
  
}

void scrollBar(){
	int x1=91;//max 95
	int y1=1;//max 63
	
	int x2=94;//max 95
	int y2=1;//max 63
	
	plotLine(91,1,94,1,BLACK);
	plotLine(91,2,94,2,BLACK);
	plotLine(91,3,94,3,BLACK);
	plotLine(91,4,94,4,BLACK);
	
	plotLine(90,0,95,0,GREEN);
	plotLine(90,0,90,63,GREEN);
	plotLine(95,0,95,63,GREEN);
	plotLine(90,63,95,63,GREEN);
}

void print_center_text(){

}


int text_align_center(int container_width, int ch_width, int str_length){
	return  (container_width / 2) - ((ch_width * str_length) /2);
}








	

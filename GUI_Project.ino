#include <SPI.h>
#include <Wire.h>
#include "ssd1331.h"
#include <string.h>
#include <EEPROM.h>

const int sensorPin = A0;
int sensorValue = 0;
int val = 0;
const int sensorErrorRate = 5; // For the hardware error, VCC and Resistor.
const int keyDelayRate = 250;  // ms (Milliseonds)

#define OLED_WIDTH 96
#define OLED_HEIGHT 64
#define PIN_RES 9
#define PIN_DC 8
#define PIN_CS 10
#define PIN_DIN 11
#define PIN_CLK 13

//#define RPT_METER A1
//int sensorValue = 0;

uint8_t oled_buf[OLED_WIDTH * OLED_HEIGHT / 8];

void scrollBar();
int text_align_center(int container_width, int ch_width, int str_length);

void setup()
{
	Serial.begin(9600);
	displayBegin();
	clearDisplay();
	setMonoBitmap(0, 0, my_logo, 96, 64, PURPLE);
	delay(100);
	clearDisplay();

	//displayPrint(text_align_center(96, 6, 6), 0, "Unlock", 12, true, GREEN);

	//plotLine(5, 0, 5, 10, BLUE);

	scrollBar();
}

int x = 0;
int y = 0;
int s = 8;
void loop()
{
	sensorValue = analogRead(sensorPin);

	int key = getKey(sensorValue);
	if (key != 0) //is pressing some key.
	{
		if (val != key) // is key change.
		{
			//Here is out put.
			//displayPrint(0, 0, "  ", 12, true, RED);
			//displayPrint(0, 0, key, 12, true, RED);
			Serial.print("Key: ");
			Serial.println(key);

			displayPrint(x, y, " ", 12, true, RED);

			switch (key)
			{
			case 1:
				break;
			case 2:
				y -= s;
				break;
			case 3:
				break;
			case 4:
				x -= s;
				;
				//left
				break;
			case 5:
				break;
			case 6:
				x += s;
				//right
				break;
			case 7:
				break;
			case 8:
				y += s;
				break;
			case 9:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			}
			displayPrint(x, y, "#", 12, true, WHITE);

			val = key;
		}

		delay(keyDelayRate);
		val = 0;
	}
}

void scrollBar()
{
	int x1 = 91; //max 95
	int y1 = 1;  //max 63

	int x2 = 94; //max 95
	int y2 = 1;  //max 63

	plotLine(91, 1, 94, 1, BLACK);
	plotLine(91, 2, 94, 2, BLACK);
	plotLine(91, 3, 94, 3, BLACK);
	plotLine(91, 4, 94, 4, BLACK);

	plotLine(90, 0, 95, 0, GREEN);
	plotLine(90, 0, 90, 63, GREEN);
	plotLine(95, 0, 95, 63, GREEN);
	plotLine(90, 63, 95, 63, GREEN);
}

void print_center_text()
{
}

int text_align_center(int container_width, int ch_width, int str_length)
{
	return (container_width / 2) - ((ch_width * str_length) / 2);
}

int getKey(int value) // 3x4 Analog Buttons Module.
{
	if (value == 1023)
	{
		return 1;
	}
	else if (value + sensorErrorRate > 932 && value - sensorErrorRate < 931)
	{
		return 2;
	}
	else if (value + sensorErrorRate > 854 && value - sensorErrorRate < 853)
	{
		return 3;
	}
	else if (value + sensorErrorRate > 789 && value - sensorErrorRate < 788)
	{
		return 4;
	}
	else if (value + sensorErrorRate > 733 && value - sensorErrorRate < 732)
	{
		return 5;
	}
	else if (value + sensorErrorRate > 684 && value - sensorErrorRate < 683)
	{
		return 6;
	}
	else if (value + sensorErrorRate > 643 && value - sensorErrorRate < 642)
	{
		return 7;
	}
	else if (value + sensorErrorRate > 605 && value - sensorErrorRate < 604)
	{
		return 8;
	}
	else if (value + sensorErrorRate > 571 && value - sensorErrorRate < 571)
	{
		return 9;
	}
	else if (value + sensorErrorRate > 541 && value - sensorErrorRate < 540)
	{
		return 10;
	}
	else if (value + sensorErrorRate > 513 && value - sensorErrorRate < 513)
	{
		return 11;
	}
	else if (value + sensorErrorRate > 489 && value - sensorErrorRate < 489)
	{
		return 12;
	}
	return 0;
}

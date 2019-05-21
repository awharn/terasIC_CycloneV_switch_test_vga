#include "address_map_arm.h"
void VGA_box(int, int, int, int, short);
void VGA_clr(void);
void VGA_text(char *);

int main(void)
{
	volatile int * switches = (int *) 0xFF200040; //Switches are at this address, can change at any moment
	float switch_value;
	float percent;
	float temp;
	int x1;
	int x2;
	int y1;
	int y2;
	short color;
	char text[7] = {'X'};
	text[3] = '.';
	text[5] = '%';
	text[6] = 0;

	int *p; //Clear the character buffer
    for (p = (int*)FPGA_CHAR_BASE; p < (int*)FPGA_CHAR_END; ++p)
	{
        *p = 0;
	}
	
	while (1)
	{
		//Get data and begin formatting it.
		switch_value = *switches; //Get the present switch value
		percent = ((switch_value / 1023) * 100) + 0.05; //Get the percent of 1023 and add 0.05 to do rounding.
		temp = percent;	//Create temporary variable that can be changed
		text[0] = (((int) (percent / 100)) + 48); //Returns first char
		temp = (int) percent % 100;
		text[1] = ((int) (temp / 10)) + 48; //Returns second char
		temp = (int) percent % 10;
		text[2] = ((int) temp) + 48; //Returns third char
		temp = (int) (percent * 10) % 10; //Get last digit
		text[4] = ((int) temp) + 48; //Returns fourth char

		color = 255; //Blue color

		if (text[0] == '0')
		{
			text[0] = ' ';
			if (text[1] == '0')
			{
				text[1] = ' ';
			}
		}		

		x1 = 60;
		x2 = ((int) (percent * 2)) + 59;
		y1 = 112;
		y2 = 126;
		VGA_clr();
		if (x2 >= x1)
		{
			VGA_box(x1, x2, y1, y2, color);
		}
		VGA_text(text); 
	}
}

void VGA_box(int x1, int x2, int y1, int y2, short pixel_color) //Display rectangle
{
	int pixel_ptr, row, column;
	for (row = y1; row <= y2; row++)
	{
		for (column = x1; column <= x2; ++column)
		{
			pixel_ptr = FPGA_ONCHIP_BASE + (row << 10) + (column << 1);
			*(short *)pixel_ptr = pixel_color;
		}
	}
}

void VGA_clr()
{
	VGA_box(0, 319, 0, 239, 0); //Black background
}

void VGA_text(char * text_ptr) //Display text
{
	int x = 39; //Make position static
	int y = 29;
	int offset;
	volatile char * character_buffer = (char *) FPGA_CHAR_BASE;

	offset = (y << 7) + x;
	while ( *(text_ptr))
	{
		*(character_buffer + offset) = *(text_ptr);
		++text_ptr;
		++offset;
	}
}

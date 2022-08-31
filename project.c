/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define PS2_BASE			  0xFF200100

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0x0

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_LETTERS 5

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
volatile int* PS2_ptr = (int*)PS2_BASE;


// ***************** function prototypes ******************* //
void clear_screen(void);
void swap_two_ints(int* first_num, int* second_num);
void draw_line(int x_coord1, int y_coord1, int x_coord2, int y_coord2, short int color);
void plot_pixel(int x, int y, short int line_color);
void plot_letter(int x, int y, short int line_color, char letter);
void plot_pixel_big(int x, int y, short int line_color);
void getFromKeyboard(unsigned char *keyboardValue);
void wait_for_vsync(void);
// ********************************************************** //

// **************** LETTERS STRUCTURE *************************//
struct LetterInfo
{
	char letter;
	
	int x_pos;
	int y_pos;
	
	int dx;
	int dy;	
};
 
// array of the structs

struct LetterInfo array[NUM_LETTERS];

// ********************************************************* //

int main(void)
{
    volatile int* pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    
	// random data to start
	for(int i = 0; i < NUM_LETTERS; i++){
		
		// letters
		array[i].letter = (rand() % 3) + 97; // ascii from a to z lowercase
		
		// initial position
		array[i].x_pos = rand() % 300 + 5;
		array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
		
		// initial motion
		array[i].dx= 0;
		array[i].dy = rand() % 2 + 1;
		
	}

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

	bool playGame = true;
	volatile int* SW_PTR = (int*)SW_BASE;
	int switchContents;
	
	
	
	// the game is played here
	while(1)
	{
		// check for reset
		switchContents = *SW_PTR;
		if(switchContents == 4) // key 2 for resetting the game
		{
			playGame = true;
			
			// random data to reset all
			for(int i = 0; i < NUM_LETTERS; i++){
		
				// letters
				array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
		
				// initial position
				array[i].x_pos = rand() % 300 + 5;
				array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
			
				// initial motion
				array[i].dx= 0;
				array[i].dy = rand() % 2 + 1;
			}
		}
		
		
		while (playGame)
		{	
			// check for a pause or a reset
			switchContents = *SW_PTR; // switchContents is contents of switch
			
			while(switchContents == 2) // infinite while loop when game is paused, key 1
			{
				switchContents = *SW_PTR; // switchContents is contents of switch
			}
			switchContents = *SW_PTR;
			
			if(switchContents == 4) // key 2 for resetting the game
			{
				playGame = true;
			
				// random data to reset all
				for(int i = 0; i < NUM_LETTERS; i++){
		
				// letters
				array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
		
				// initial position
				array[i].x_pos = rand() % 300 + 5;
				array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
			
				// initial motion
				array[i].dx= 0;
				array[i].dy = rand() % 2 + 1;
				}
			}
			
			clear_screen();
			
			// drawing the threshold game line
			draw_line(0, 230, 319, 230, RED);
						
			unsigned char keyboardValue = 0;
			
			// code for updating the locations of boxes
			for(int i = 0; i < NUM_LETTERS; i++)
			{
			
				getFromKeyboard(&keyboardValue);

				if((keyboardValue == 0x1c && array[i].letter == 'a') || (keyboardValue == 0x32 && array[i].letter == 'b') || (keyboardValue == 0x21 && array[i].letter == 'c') || (keyboardValue == 0x23 && array[i].letter == 'd') || (keyboardValue == 0x24 && array[i].letter == 'e') || (keyboardValue == 0x2b && array[i].letter == 'f') || (keyboardValue == 0x34 && array[i].letter == 'g') || (keyboardValue == 0x33 && array[i].letter == 'h') || (keyboardValue == 0x43 && array[i].letter == 'i') || (keyboardValue == 0x3b && array[i].letter == 'j') || (keyboardValue == 0x42 && array[i].letter == 'k') || (keyboardValue == 0x4b && array[i].letter == 'l') || (keyboardValue == 0x3a && array[i].letter == 'm') || (keyboardValue == 0x31 && array[i].letter == 'n') || (keyboardValue == 0x44 && array[i].letter == 'o') || (keyboardValue == 0x4d && array[i].letter == 'p') || (keyboardValue == 0x15 && array[i].letter == 'q') || (keyboardValue == 0x2d && array[i].letter == 'r') || (keyboardValue == 0x1b && array[i].letter == 's') || (keyboardValue == 0x2c && array[i].letter == 't') || (keyboardValue == 0x3c && array[i].letter == 'u') || (keyboardValue == 0x2a && array[i].letter == 'v') || (keyboardValue == 0x1d && array[i].letter == 'w') || (keyboardValue == 0x22 && array[i].letter == 'x') || (keyboardValue == 0x35 && array[i].letter == 'y') || (keyboardValue == 0x1a && array[i].letter == 'z'))
				{
					
					// the key for a specifc letter is pressed so can now reset this letter to something new
					// letters
					array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
			
					// initial position
					array[i].x_pos = rand() % 300 + 5;
					array[i].y_pos = rand() % 10 + 5; // actual size is to 239, but starting letters only in top half of screen
			
					// initial motion
					array[i].dx= 0;
					array[i].dy = rand() % 2 + 1;
					
					//draw new letter
					plot_letter(array[i].x_pos, array[i].y_pos, WHITE, array[i].letter);
					
				}
				else
				{
					// hitting boundaries, need to change directions
					if(array[i].y_pos > 229)
					playGame = false;
				
					if(playGame)
					{
						// increment for new line position
						array[i].y_pos = array[i].y_pos + array[i].dy;
						array[i].x_pos = array[i].x_pos + array[i].dx;
					
						//draw new letter
						plot_letter(array[i].x_pos, array[i].y_pos, WHITE, array[i].letter);	
					}
				}
				
			}
			
			if(playGame)
			{
				wait_for_vsync(); // swap front and back buffers on VGA vertical sync		
				pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			}
		}
		
		if(!playGame)
		{
			// the game over screen
			
			clear_screen();
			// game over letters
			
			// G 
			plot_pixel_big(70, 20, WHITE);
			plot_pixel_big(60, 20, WHITE);
			plot_pixel_big(50, 30, WHITE);
			plot_pixel_big(50, 40, WHITE);
			plot_pixel_big(50, 50, WHITE);
			plot_pixel_big(60, 60, WHITE);
			plot_pixel_big(70, 60, WHITE);
			plot_pixel_big(80, 50, WHITE);
			plot_pixel_big(80, 40, WHITE);
			plot_pixel_big(70, 40, WHITE);

			// A
			plot_pixel_big(110, 60, WHITE);
			plot_pixel_big(110, 50, WHITE);
			plot_pixel_big(110, 40, WHITE);
			plot_pixel_big(110, 30, WHITE);
			plot_pixel_big(120, 20, WHITE);
			plot_pixel_big(120, 40, WHITE);
			plot_pixel_big(130, 20, WHITE);
			plot_pixel_big(130, 40, WHITE);
			plot_pixel_big(140, 30, WHITE);
			plot_pixel_big(140, 40, WHITE);
			plot_pixel_big(140, 50, WHITE);
			plot_pixel_big(140, 60, WHITE);
			
			// M
			plot_pixel_big(170, 60, WHITE);
			plot_pixel_big(170, 50, WHITE);
			plot_pixel_big(170, 40, WHITE);
			plot_pixel_big(170, 30, WHITE);
			plot_pixel_big(180, 20, WHITE);
			plot_pixel_big(190, 30, WHITE);
			plot_pixel_big(200, 20, WHITE);
			plot_pixel_big(210, 30, WHITE);
			plot_pixel_big(210, 40, WHITE);
			plot_pixel_big(210, 50, WHITE);
			plot_pixel_big(210, 60, WHITE);
			
			// E
			plot_pixel_big(240, 60, WHITE);
			plot_pixel_big(240, 50, WHITE);
			plot_pixel_big(240, 40, WHITE);
			plot_pixel_big(240, 30, WHITE);
			plot_pixel_big(240, 20, WHITE);
			plot_pixel_big(250, 20, WHITE);
			plot_pixel_big(260, 20, WHITE);
			plot_pixel_big(250, 40, WHITE);
			plot_pixel_big(250, 60, WHITE);
			plot_pixel_big(260, 60, WHITE);
		
			// O
			plot_pixel_big(50, 100, WHITE);
			plot_pixel_big(50, 110, WHITE);
			plot_pixel_big(50, 120, WHITE);
			plot_pixel_big(60, 90, WHITE);
			plot_pixel_big(60, 130, WHITE);
			plot_pixel_big(70, 90, WHITE);
			plot_pixel_big(70, 130, WHITE);
			plot_pixel_big(80, 100, WHITE);
			plot_pixel_big(80, 110, WHITE);
			plot_pixel_big(80, 120, WHITE);
			
			// V
			plot_pixel_big(110, 90, WHITE);
			plot_pixel_big(110, 100, WHITE);
			plot_pixel_big(110, 110, WHITE);
			plot_pixel_big(110, 120, WHITE);
			plot_pixel_big(120, 130, WHITE);
			plot_pixel_big(130, 130, WHITE);
			plot_pixel_big(140, 90, WHITE);
			plot_pixel_big(140, 100, WHITE);
			plot_pixel_big(140, 110, WHITE);
			plot_pixel_big(140, 120, WHITE);
			
			// E
			plot_pixel_big(170, 90, WHITE);
			plot_pixel_big(170, 100, WHITE);
			plot_pixel_big(170, 110, WHITE);
			plot_pixel_big(170, 120, WHITE);
			plot_pixel_big(170, 130, WHITE);
			plot_pixel_big(180, 90, WHITE);
			plot_pixel_big(190, 90, WHITE);
			plot_pixel_big(180, 110, WHITE);
			plot_pixel_big(180, 130, WHITE);
			plot_pixel_big(190, 130, WHITE);
			
			// R
			plot_pixel_big(220, 90, WHITE);
			plot_pixel_big(220, 100, WHITE);
			plot_pixel_big(220, 110, WHITE);
			plot_pixel_big(220, 120, WHITE);
			plot_pixel_big(220, 130, WHITE);
			plot_pixel_big(230, 90, WHITE);
			plot_pixel_big(240, 90, WHITE);
			plot_pixel_big(250, 100, WHITE);
			plot_pixel_big(230, 110, WHITE);
			plot_pixel_big(240, 110, WHITE);
			plot_pixel_big(250, 120, WHITE);
			plot_pixel_big(250, 130, WHITE);
			
			// !
			plot_pixel_big(280, 90, WHITE);
			plot_pixel_big(280, 100, WHITE);
			plot_pixel_big(280, 110, WHITE);
			plot_pixel_big(280, 130, WHITE);
			
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync		
			pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			
		}
		
		while(!playGame)
		{
			//end of game loop, checking for a reset...
			switchContents = *SW_PTR;
			if(switchContents == 4) // key 2 for resetting the game
			{
				playGame = true;
			
				// random data to reset all
				for(int i = 0; i < NUM_LETTERS; i++){
			
				// letters
				array[i].letter = (rand() % 26) + 97; // ascii from a to z lowercase
		
				// initial position
				array[i].x_pos = rand() % 300 + 5;
				array[i].y_pos = 8; // actual size is to 239, but starting letters only in top half of screen
			
				// initial motion
				array[i].dx= 0;
				array[i].dy = rand() % 2 + 1;
				}
			}
	    }
	}
}



/******************************SUBROUTINES****************************************************/

void clear_screen(void)
{
	// go through each pixel in vga display and put black
	// nested for loop for x and y coordinates of screen (320 x 240)
	for(int x_coord = 0; x_coord < 320; x_coord++){
		for(int y_coord = 0; y_coord < 240; y_coord++){
			plot_pixel(x_coord, y_coord, BLACK);
		}
	}
}

// enhanced for bigger pixels and letters
void plot_letter(int x, int y, short int line_color, char letter)
{
    if(letter == 'a')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'b')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'c')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'd')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'e')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'f')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'g')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 4) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'h')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'i')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'j')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
	}
	else if(letter == 'k')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'l')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 'm')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 4) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 4) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 4) << 1)) = line_color;
	}
	else if(letter == 'n')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
	}
	else if(letter == 'o')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'p')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'q')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'r')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
	}
	else if(letter == 's')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		
	}
	else if(letter == 't')
	{
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		
	}
	else if(letter == 'u')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'v')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	else if(letter == 'w')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 3) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 3) << 1)) = line_color;
		
	}
	else if(letter == 'x')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;		
	}
	else if(letter == 'y')
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x - 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x - 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
	}
	else //for z
	{
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 1) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 2) << 10) + ((x + 2) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 1) << 1)) = line_color;
		*(short int*)(pixel_buffer_start + ((y - 3) << 10) + ((x + 2) << 1)) = line_color;
		
	}
	
}

// regular plot pixel
void plot_pixel(int x, int y, short int line_color)
{
    *(short int*)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// big plot pixel
void plot_pixel_big(int x, int y, short int line_color)
{
    for(int x_pos = 0; x_pos < 10; x_pos++)
	{
		for(int y_pos = 0; y_pos < 10; y_pos++)
		{
			*(short int*)(pixel_buffer_start + ((y + y_pos) << 10) + ((x + x_pos) << 1)) = line_color;
		}
	}
}

void wait_for_vsync(void)
{
	volatile int* pixel_ctrl_ptr = (int*) 0xFF203020; // pixel controller
	register int status;
	
	*pixel_ctrl_ptr = 1; // start synchronization
	
	status = *(pixel_ctrl_ptr + 3);
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}
}

void draw_line(int x_coord1, int y_coord1, int x_coord2, int y_coord2, short int colour)
{
    bool steep_slope = abs(y_coord2 - y_coord1) > abs(x_coord2 - x_coord1);

    if(steep_slope == 1){
        swap_two_ints(&x_coord1, &y_coord1);
        swap_two_ints(&x_coord2, &y_coord2);
    }

    if(x_coord1 > x_coord2) {
        swap_two_ints(&x_coord1, &x_coord2);
        swap_two_ints(&y_coord1, &y_coord2);
    }

    int delta_x = x_coord2 - x_coord1;
    int delta_y = abs(y_coord2 - y_coord1);
    int error = (delta_x/2)*(-1);
	
    int y_step;
	if(y_coord1 < y_coord2)
		y_step = 1;
	else
		y_step = -1;
	
	// now we will be using variables x and y to draw, start with
	// x = initial x and y = inital y (coord1)
	// the idea is using breshenam algorithm
	int y = y_coord1;
    for(int x = x_coord1; x <= x_coord2; x++){
        if(steep_slope == 1)
			plot_pixel(y, x, colour);
        else
			plot_pixel(x, y, colour);

        error = error + delta_y;

        if(error > 0)
		{
            y = y + y_step;
            error = error - delta_x;
        }
    }
}

void swap_two_ints(int* first_num, int* second_num){
    int temp_var;
	temp_var = *second_num;
    *second_num = *first_num;
    *first_num = temp_var;
}

void getFromKeyboard(unsigned char *keyboardValue){
	volatile int * PS2_ptr = (int *) 0xFF200100;
	int data = *PS2_ptr;
	*keyboardValue = data & 0xFF;
	
	while(data & 0x8000){
		data = *PS2_ptr;	
	}	
}


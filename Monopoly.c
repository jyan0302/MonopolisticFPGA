//320*240
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();


int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; 
    pixel_buffer_start = *(pixel_ctrl_ptr+1);
    clear_screen();
    wait_for_vsync();
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    //colors
    int black = 0x0;
    int blue = 0x001F;
    int red = 0xF800;
    int green = 0x07E0;
    int pink = 0xF81F;
    int white = 0xFFFF;
    int lightblue = 0x2353;

    int colors[6] = {blue, red, green, pink, white, lightblue};
    int cur_color = 0;
    int prev_color = 0;
    //game loop
    while (true){
        while (true){
            while (true){
                cur_color = colors[rand() % 6];
                if (cur_color != prev_color){
                    break;
                }
            }
            prev_color = cur_color;
            for (int y = 0; y < 240; y++){
                for (int x = 0; x < 320; x++){
                    plot_pixel(x, y, cur_color);
                }
                //draw
                wait_for_vsync(); // swap front and back buffers on VGA vertical sync
                pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
            }//draw loop
        }//play loop
    }//game loop
}//main


void plot_pixel(int x, int y, short int line_color){
    if ((x>319)||(x<0)||(y>239)||(y<0)){
        return;
    }
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    register int status;
    *pixel_ctrl_ptr = 1;        //tells buffers to switch
    status = *(pixel_ctrl_ptr+3); //checks when print has ended
    while ((status&0x01)!=0){   //while print has not ended, loop
        status = *(pixel_ctrl_ptr+3);
    }
}

//prints black pixels on the entire screen
void clear_screen(){
    //screen is 320*240 pixels
    for (int x=0; x<320; x++){
        //x is columns, goes left to right
        for (int y=0; y<240; y++){
            plot_pixel(x,y,0);
        }
    }
}
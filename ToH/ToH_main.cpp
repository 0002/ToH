#include "ToH_stack.h"

using namespace std;

volatile int timer = 0; // Timer to control speed

void increment() // Function to modify timer
{
    ++timer;
}

void buildbg(BITMAP* buffer) // Prints the background: wallpaper, base, pegs, speed slider, and close button
{
    // Load bitmaps which don't move around
    BITMAP* bg = load_bitmap("bg.bmp", NULL);
    BITMAP* base = load_bitmap("base.bmp", NULL);
    BITMAP* peg = load_bitmap("peg.bmp", NULL);

    BITMAP* spdbar = load_bitmap("hbar.bmp", NULL);
    BITMAP* close = load_bitmap("close.bmp", NULL);

    blit(bg, buffer, 0, 0, 0, 0, bg->w, bg->h);
    masked_blit(base, buffer, 0, 0, 0, screenheight-base->h, base->w, base->h);
    for(int c = 0; c < 3; c++)
    {
        masked_blit(peg, buffer, 0, 0, int(c*screenwidth/3. + screenwidth/6. - 12), 40, peg->w, peg->h - 36);
    }

    draw_sprite(buffer, close, 507, 0);
    draw_sprite(buffer, spdbar, 298, 280);

    // Cleanup
    destroy_bitmap(bg);
    destroy_bitmap(base);
    destroy_bitmap(peg);
    destroy_bitmap(spdbar);
    destroy_bitmap(close);
}

void drawspdslider(float& speed, BITMAP* buffer, int& slider_x, bool& xspec) // Draws the speed slider
{
    BITMAP* spdslider = load_bitmap("purpleblock.bmp", NULL);
    if((xspec == 1 || (mouse_y > 282 && mouse_y < 294)) && (mouse_b&1 && mouse_x > 308 && mouse_x < 504))
    {
        xspec = 1;
        speed = (mouse_x - 304)/20.;
        slider_x = mouse_x;
    }
    if(!mouse_b&1 && xspec == 1) xspec = 0;

    textprintf_ex(buffer, font, 164, 280, makecol(255, 255, 255), -1, "Speed: %0.2f", speed);
    masked_stretch_blit(spdslider, buffer, 0, 0, spdslider->w, spdslider->h, slider_x - 6, 276, 24, 18);
    destroy_bitmap(spdslider);
}

// Main recursive function; the 12 parameters are required to prevent global variables
void solve(STACK& start, STACK& target, STACK& interm, int msize, int size, int& moves, int& slider_x, bool& xspec, BITMAP* sourceone, BITMAP* sourcetwo, BITMAP* buffer, float& speed)
{
    if(msize > 0)
    {
        solve(start, interm, target, msize - 1, size, moves, slider_x, xspec, sourceone, sourcetwo, buffer, speed);
        int temp = start.pop();
        if(temp%2 == 0) target.push(temp, size, sourceone);
        else target.push(temp, size, sourcetwo);

        timer = 0; // Reset timer

        moves++;


        int max = 0;

        do // This ensures that the buffer is constantly updated, instead of just at a certain interval
        {
            buildbg(buffer);

            for(int c = 1; c < 4; c++)
            {
                if(start.rtnnumber() == c) start.ptobuffer(buffer);
                if(target.rtnnumber() == c) target.ptobuffer(buffer);
                if(interm.rtnnumber() == c) interm.ptobuffer(buffer);
            }

            while(!mouse_b&1 && mouse_x > 507 && mouse_y < 33)
                if(mouse_b&1) exit(0);


            drawspdslider(speed, buffer, slider_x, xspec);
            max = int(pow(2, float(size))) - 1;
            textprintf_ex(buffer, font, 4, 4, makecol(255, 255, 255), -1, "Moves: %d of %d (%0.2f%% complete)", moves, max, (float)moves*100/max);
            textprintf_ex(buffer, font, 4, 12, makecol(255, 255, 255), -1, "Mouse over close button to pause");
            show_mouse(screen);
            blit(buffer, screen, 0, 0, 0, 0, screenwidth, screenheight);
            clear_bitmap(buffer);
        }
        while(timer < (120 - 12*speed)); // do-while loop makes the contents run at least once

        solve(interm, target, start, msize - 1, size, moves, slider_x, xspec, sourceone, sourcetwo, buffer, speed);
    }
}

int main(int argc, char* argv[])
{
    /// Initialization
    allegro_init();
    install_keyboard();
    install_mouse();
    install_timer();

    LOCK_VARIABLE(timer);
    LOCK_FUNCTION(increment);
    install_int_ex(increment, BPS_TO_TIMER(120));

    set_color_depth(desktop_color_depth());
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, screenwidth, screenheight, 0, 0);

    // Bitmaps
    BITMAP* buffer = create_bitmap(screenwidth, screenheight);
    BITMAP* sourceone = load_bitmap("redblock.bmp", NULL);
    BITMAP* sourcetwo = load_bitmap("blueblock.bmp", NULL);

    if(!(sourceone && sourcetwo)) return 1;

    STACK stack1, stack2, stack3; // Stacks
    stack1.modnumber(1);
    stack2.modnumber(2);
    stack3.modnumber(3);

    int size = 7; // Number of starting disks; passed to many functions
    float speed = 5.35; // Represents the speed; can theoretically go from 0 (delay 1s) to 10 (delay 0s)

    BITMAP* cursor = load_bitmap("cursor.bmp", NULL);
    BITMAP* sizeslider = load_bitmap("greenblock.bmp", NULL);
    BITMAP* sizebar = load_bitmap("vbar.bmp", NULL);
    BITMAP* spdslider = load_bitmap("purpleblock.bmp", NULL);
    BITMAP* text = load_bitmap("intro.bmp", NULL);

    int slider_y = 164, slider_x = 412; // Positions of the two sliders
    bool yspec = 0, xspec = 0; // Helper variables for checking mouse locations

    /// Startscreen
    do
    {
        buildbg(buffer);

        // Controls for the size slider. The helper variables enable the cursor to affect the slider even if it strays off the bar.
        if((yspec == 1 || (mouse_x > 171 && mouse_x < 189)) && (xspec == 0 && mouse_b&1 && mouse_y > 46 && mouse_y < 242))
        {
            yspec = 1;
            size = (mouse_y - 46)/18 + 1;
            slider_y = mouse_y;
        }
        if(!mouse_b&1 && yspec == 1) yspec = 0;

        // Controls for the speed slider -- these are separate as they're also needed in the recursive part of the function
        drawspdslider(speed, buffer, slider_x, xspec);

        // Demo stack for display while the user chooses a size; serves no other purpose
        STACK demo;
        demo.modnumber(1); // Put in position 1 (starting pos.)
        demo.initialize(size, sourceone, sourcetwo);
        demo.ptobuffer(buffer);

        // Printing to buffer
        draw_sprite(buffer, sizebar, 174, 36);
        masked_stretch_blit(sizeslider, buffer, 0, 0, sizeslider->w, sizeslider->h, 171, slider_y - 6, 18, 24);
        textprintf_ex(buffer, font, 64, 280, makecol(255, 255, 255), -1, "Size: %d", size);
        masked_stretch_blit(text, buffer, 0, 0, text->w, text->h, 248, 8, 248, 256);
        draw_sprite(buffer, cursor, mouse_x, mouse_y);

        textprintf_ex(buffer, font, 4, 16, makecol(255, 255, 255), -1, "Press enter to begin!", size);

        draw_sprite(screen, buffer, 0, 0);
        clear_bitmap(buffer);

        // Checks if the close button is clicked
        while(!mouse_b&1 && mouse_x > 507 && mouse_y < 33)
            if(mouse_b&1) return 0;

    } while(!key[KEY_ENTER]);

    buildbg(buffer);
    destroy_bitmap(cursor);

    draw_sprite(screen, buffer, 0, 0);

    stack1.initialize(size, sourceone, sourcetwo);
    stack1.ptobuffer(buffer);
    blit(buffer, screen, 0, 0, 0, 0, screenwidth, screenheight);
    clear_bitmap(buffer);

    /// Solving
    int moves = 0; // Move counter
    solve(stack1, stack3, stack2, size, size, moves, slider_x, xspec, sourceone, sourcetwo, buffer, speed);

    /// Endscreen
    draw_sprite(screen, buffer, 0, 0);
    BITMAP* complete = load_bitmap("complete.bmp", NULL);
    while(!key[KEY_ESC])
    {
        masked_stretch_blit(complete, buffer, 0, 0, complete->w, complete->h, 50, 120, 287, 72);
        textprintf_ex(buffer, font, 118, 192, makecol(255, 255, 255), -1, "Press esc to quit.");
        stack3.ptobuffer(buffer);
        show_mouse(buffer);
        draw_sprite(screen, buffer, 0, 0);
        clear_bitmap(buffer);
    }

    return 0;

    // Cleanup
    destroy_bitmap(buffer);
    destroy_bitmap(sourceone);
    destroy_bitmap(sourcetwo);
    destroy_bitmap(complete);
    destroy_bitmap(cursor);
    destroy_bitmap(sizeslider);
    destroy_bitmap(sizebar);
    destroy_bitmap(spdslider);
    destroy_bitmap(text);
}

END_OF_MAIN();

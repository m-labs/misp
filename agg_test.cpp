#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <hw/csr.h>
#include <hw/flags.h>
#include "agg.h"

enum
{
    width  = 1024,
    height = 768
};


double random(double min, double max)
{
    int r = (rand() << 15) | rand();
    return ((r & 0xFFFFFFF) / double(0xFFFFFFF + 1)) * (max - min) + min;
}


void draw_ellipse(agg::rasterizer& ras,
                  double x,  double y,
                  double rx, double ry)
{
    int i;
    ras.move_to_d(x + rx, y);

    // Here we have a fixed number of approximation steps, namely 360
    // while in reality it's supposed to be smarter.
    for(i = 1; i < 360; i++)
    {
        double a = double(i) * 3.1415926 / 180.0;
        ras.line_to_d(x + cos(a) * rx, y + sin(a) * ry);
    }
}


void draw_line(agg::rasterizer& ras,
               double x1, double y1, 
               double x2, double y2,
               double width)
{

    double dx = x2 - x1;
    double dy = y2 - y1;
    double d = sqrt(dx*dx + dy*dy);
    
    dx = width * (y2 - y1) / d;
    dy = width * (x2 - x1) / d;

    ras.move_to_d(x1 - dx,  y1 + dy);
    ras.line_to_d(x2 - dx,  y2 + dy);
    ras.line_to_d(x2 + dx,  y2 - dy);
    ras.line_to_d(x1 + dx,  y1 - dy);
}

enum {
    VGA_MODE_640_480,
    VGA_MODE_800_600,
    VGA_MODE_1024_768,
    VGA_MODE_1920_1080
};

static void vga_clkgen_write(int cmd, int data)
{
    int word;

    word = (data << 2) | cmd;
    crg_cmd_data_write(word);
    crg_send_cmd_data_write(1);
    while(crg_status_read() & CLKGEN_STATUS_BUSY);
}

/* http://web.mit.edu/6.111/www/s2004/NEWKIT/vga.shtml */
static void vga_set_mode(int mode)
{
    int vga_hres, vga_vres;
    int clock_m, clock_d;

    switch(mode) {
        default:
        case VGA_MODE_640_480: // Pixel clock: 25MHz
            vga_hres = 640;
            vga_vres = 480;
            clock_m = 2;
            clock_d = 4;
            fb_hres_write(640);
            fb_hsync_start_write(656);
            fb_hsync_end_write(752);
            fb_hscan_write(800);
            fb_vres_write(480);
            fb_vsync_start_write(492);
            fb_vsync_end_write(494);
            fb_vscan_write(525);
            break;
        case VGA_MODE_800_600: // Pixel clock: 50MHz
            vga_hres = 800;
            vga_vres = 600;
            clock_m = 2;
            clock_d = 2;
            fb_hres_write(800);
            fb_hsync_start_write(848);
            fb_hsync_end_write(976);
            fb_hscan_write(1040);
            fb_vres_write(600);
            fb_vsync_start_write(636);
            fb_vsync_end_write(642);
            fb_vscan_write(665);
            break;
        case VGA_MODE_1024_768: // Pixel clock: 65MHz
            vga_hres = 1024;
            vga_vres = 768;
            clock_m = 13;
            clock_d = 10;
            fb_hres_write(1024);
            fb_hsync_start_write(1048);
            fb_hsync_end_write(1184);
            fb_hscan_write(1344);
            fb_vres_write(768);
            fb_vsync_start_write(772);
            fb_vsync_end_write(778);
            fb_vscan_write(807);
            break;
        case VGA_MODE_1920_1080: // Pixel clock: 148MHz
            vga_hres = 1920;
            vga_vres = 1080;
            clock_m = 74;
            clock_d = 25;
            fb_hres_write(1920);
            fb_hsync_start_write(2008);
            fb_hsync_end_write(2052);
            fb_hscan_write(2200);
            fb_vres_write(1080);
            fb_vsync_start_write(1084);
            fb_vsync_end_write(1089);
            fb_vscan_write(1125);
            break;
    }
    fb_length_write(vga_hres*vga_vres*4);

    vga_clkgen_write(0x1, clock_d-1);
    vga_clkgen_write(0x3, clock_m-1);
    crg_send_go_write(1);
    printf("waiting for PROGDONE...");
    while(!(crg_status_read() & CLKGEN_STATUS_PROGDONE));
    printf("ok\n");
    printf("waiting for LOCKED...");
    while(!(crg_status_read() & CLKGEN_STATUS_LOCKED));
    printf("ok\n");

    printf("VGA: mode set to %dx%d\n", vga_hres, vga_vres);
}

static void start_fb(unsigned char *addr)
{
    vga_set_mode(VGA_MODE_1024_768);
    fb_base_write((unsigned int)addr);
    fb_enable_write(1);
}

extern "C" void agg_test(void);
void agg_test(void)
{
    // Allocate the framebuffer
    unsigned char* buf = new unsigned char[width * height * 4];

    start_fb(buf);

    // Create the rendering buffer 
    agg::rendering_buffer rbuf(buf, width, height, width * 4);

    // Create the renderer and the rasterizer
    agg::renderer<agg::span_rgb101010> ren(rbuf);
    agg::rasterizer ras;

    // Setup the rasterizer
    ras.gamma(1.3);
    ras.filling_rule(agg::fill_even_odd);

    ren.clear(agg::rgba8(255, 255, 255));

    int i;

    // Draw random polygons
    for(i = 0; i < 10; i++)
    {
        int n = rand() % 6 + 3;

        // Make the polygon. One can call move_to() more than once. 
        // In this case the rasterizer behaves like Win32 API PolyPolygon().
        ras.move_to_d(random(-30, rbuf.width() + 30), 
                      random(-30, rbuf.height() + 30));

        int j;
        for(j = 1; j < n; j++)
        {
            ras.line_to_d(random(-30, rbuf.width() + 30), 
                          random(-30, rbuf.height() + 30));
        }

        // Render
        ras.render(ren, agg::rgba8(rand() & 0xFF, 
                                   rand() & 0xFF, 
                                   rand() & 0xFF, 
                                   rand() & 0xFF));
    }

    // Draw random ellipses
    for(i = 0; i < 50; i++)
    {
        draw_ellipse(ras, 
                     random(-30, rbuf.width()  + 30), 
                     random(-30, rbuf.height() + 30),
                     random(3, 50), 
                     random(3, 50));
        ras.render(ren, agg::rgba8(rand() & 0x7F, 
                                   rand() & 0x7F, 
                                   rand() & 0x7F,
                                  (rand() & 0x7F) + 100));
    }

    // Draw random straight lines
    for(i = 0; i < 20; i++)
    {
        draw_line(ras, 
                  random(-30, rbuf.width()  + 30), 
                  random(-30, rbuf.height() + 30),
                  random(-30, rbuf.width()  + 30), 
                  random(-30, rbuf.height() + 30),
                  random(0.1, 10));

        ras.render(ren, agg::rgba8(rand() & 0x7F, 
                                   rand() & 0x7F, 
                                   rand() & 0x7F));
    }

    delete [] buf;
}

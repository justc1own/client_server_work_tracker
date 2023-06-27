#include "screenshot.hpp"
#include <iostream>

CustomImg::CustomImg() : array(nullptr), width(0), height(0) {}

CustomImg::CustomImg(unsigned char *array_) : CustomImg() {
    array = array_;
}

void CustomImg::makeImg() {
   Display *display = XOpenDisplay(NULL);
   Window root = DefaultRootWindow(display);

   XWindowAttributes gwa;

   XGetWindowAttributes(display, root, &gwa);
   width = gwa.width;
   height = gwa.height;
   //std::cout << "width=" << width << " height=" << height << std::endl; 


   XImage *image = XGetImage(display,root, 0,0 , width,height,AllPlanes, ZPixmap);

   array = new unsigned char[width * height * 3];

   unsigned long red_mask = image->red_mask;
   unsigned long green_mask = image->green_mask;
   unsigned long blue_mask = image->blue_mask;

   for (int x = 0; x < width; x++)
      for (int y = 0; y < height ; y++)
      {
         unsigned long pixel = XGetPixel(image,x,y);

         unsigned char blue = pixel & blue_mask;
         unsigned char green = (pixel & green_mask) >> 8;
         unsigned char red = (pixel & red_mask) >> 16;

         array[(x + width * y) * 3] = red;
         array[(x + width * y) * 3+1] = green;
         array[(x + width * y) * 3+2] = blue;
      }

   //CImg<unsigned char> pic(array,width,height,1,3);
   //this->img = pic;
}
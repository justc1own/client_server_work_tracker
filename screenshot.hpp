#include <X11/Xlib.h>
#include <X11/X.h>

#include <cstdio>
#include <CImg.h>
#include <iostream>
#include <ostream>

using namespace cimg_library;

class CustomImg{
public:
    CustomImg();
    CustomImg(unsigned char *array);
    //CImg<unsigned char> img;

    void makeImg();

    unsigned char *array;
    int width, height;
    //std::ostream &operator<<(std::ostream &os);
    
private:
};
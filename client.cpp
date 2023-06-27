#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include <memory>
#include "screenshot.hpp"

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;

    tcp::socket s(io_context);
    boost::asio::connect(s, tcp::resolver(io_context).resolve("localhost", "12345"));

    tcp::iostream conn(std::move(s));
    //
    std::cout << "Connected " << conn.socket().local_endpoint() << " ---> " 
                        << conn.socket().remote_endpoint() << "\n";
    //

    std::string input;
    while(conn) {
        
        //std::cout << "debug out\n";
        //std::this_thread::sleep_for(std::chrono::milliseconds(900));
        if(conn >> input) {
            if(input == "prtScr") {
                std::unique_ptr<CustomImg> ptr = std::make_unique<CustomImg>();
                ptr->makeImg();
                
                conn << 1 << "\n"; //validInput check in server
                unsigned char *pic = ptr->array;
                int width = ptr->width;
                int height = ptr->height;
                conn << width << " " << height << "\n";
                for(int x = 0; x < width; x++) {
                    for(int y = 0; y < height; y++) {
                        conn << pic[width*y + x] << " ";
                    }
                }
                conn << "\n";

            } else {
                conn << 0;
            }
        }
    }
}
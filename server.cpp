#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <iostream>
#include <utility>
#include <condition_variable>
#include <string>
#include <vector>
#include <fstream>
#include <set>

#include <memory>
#include "screenshot.hpp"


using boost::asio::ip::tcp;
using namespace cimg_library;

std::string getTime(time_t rawtime, struct tm * timeinfo) {
    time( &rawtime );                         // текущая дата, выраженная в секундах
    timeinfo = localtime ( &rawtime );        // текущая дата, представленная в нормальной форме

    std::string res = asctime(timeinfo);
    return res;
}

int main() {
    time_t rawtime;
    struct tm * timeinfo;


    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
    std::cout << "Listening at " << acceptor.local_endpoint() << "\n";

    std::mutex m;
    std::condition_variable cond;
    //bool input_available = false;
    std::string input;
    std::vector<tcp::endpoint> connections;
    std::mutex connect;
    std::set<int> requests;
    std::mutex req;
    
    std::thread([&cond, &m, &connections, &connect, &requests, &req] () mutable {
        while(true) {
            std::string termCommand;
            std::unique_lock<std::mutex> l{m};
            
            if(!(std::cin >> termCommand)) {
                cond.wait(l);
            }
            if(termCommand == "info") {
                std::cout << "Information about connections:\n";
                std::unique_lock<std::mutex> c{connect};
                int idx = 1;
                for(const auto &con: connections) {
                    std::cout << con << " has idx = " << idx++ << "\n";
                }
            } else if(termCommand == "prtScr") {
                std::unique_lock<std::mutex> r{req};
                int idx;
                std::cin >> idx;
                requests.insert(idx);
            } else {
                std::cout << "Unknown command: " << termCommand << "\n";
            }

            l.unlock();
        }
    }).detach();

    while(true) {
        tcp::socket s = acceptor.accept();
        
        std::thread([s = std::move(s), &m, &cond, &input, &connections, &connect, &requests, &req, &rawtime, &timeinfo] () mutable {
            tcp::iostream client(std::move(s));
            int client_idx = -1;
            //
            std::cout << "Accepted connection " << client.socket().remote_endpoint()
                    << " --> " << client.socket().local_endpoint() << "\n";
            
            //
            {   
                std::unique_lock<std::mutex> f{connect};
                connections.emplace_back(client.socket().remote_endpoint());
                client_idx = connections.size();
            }


            while(client) {
                {
                    std::unique_lock<std::mutex> r{req};
                    if(requests.count(client_idx)) {
                        client << "prtScr\n";
                        requests.erase(client_idx);
                    }
                }
                {
                    std::unique_lock<std::mutex> l{m};
                    //std::unique_ptr<CustomImg> ptr = std::make_unique<CustomImg>();
                    bool validInput = false;
                    if(!(client >> validInput)) {
                        cond.wait(l);
                    }

                    if(validInput == false) {
                        l.unlock();
                        break;
                    }
                    int width, height;
                    client >> width >> height;
                    std::unique_ptr<unsigned char []> array = std::make_unique<unsigned char[]>(width * height * 3);
                    
                    for(int x = 0; x < width; x++) {
                        for(int y = 0; y < height; y++) {
                            unsigned char c;
                            client >> c;
                            array[width * y + x] = c;
                        }
                    }

                    CImg<unsigned char> img(&array, width, height, 1, 3);
                    std::string savePlace = std::to_string(client_idx) + getTime(rawtime, timeinfo) + ".png";
                    img.save_png("savePlace.png");
                    //std::cout << "Get input=" << input_snapshot << " from " << client.socket().remote_endpoint() << "\n"; 
                    //client << "Get input\n";
                    l.unlock();
                }
            }
        }).detach();
    }
}
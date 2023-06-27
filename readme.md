Сборка через терминал
Для client.cpp: g++ client.cpp -std=c++17 -o client -pthread
Для server.cpp: g++ server.cpp -std=c++17 -o server -pthread -lpng -lX11
Для screenshot.cpp: g++ screenshot.cpp -std=c++17 -lX11 -lpng -o screenshot
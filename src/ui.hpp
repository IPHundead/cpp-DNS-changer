#include <iostream>
#include "dns_changer.hpp"
#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>
#endif

class ui {
private:
    #if (defined (_WIN32) || defined (_WIN64))
    Windows os;
    #elif (defined (LINUX) || defined (__linux__))
    Linux os;
    #endif
    void gotoxy(int x, int y);

public:
    void run(const std::vector<DNSServer>& DNSServers);
};

void ui::gotoxy(int x, int y) {
    std::cout<<"\x1B["<<y<<";"<<x<<"H";
}

void ui::run(const std::vector<DNSServer>& DNSServers) {
}
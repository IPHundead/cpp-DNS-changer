#include <iostream>
#include "dns_changer.hpp"
#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>
#include <conio.h
#elif (defined (LINUX) || defined (__linux__))
#include <termios.h>
#endif

class ui {
private:
    #if (defined (_WIN32) || defined (_WIN64))
    Windows os;
    #elif (defined (LINUX) || defined (__linux__))
    Linux os;
    enum keyASCII
    {
        UP_ARROW_KEY = 65,
        DOWN_ARROW_KEY = 66,
        RIGHT_ARROW_KEY = 67,
        LEFT_ARROW_KEY = 68,
        ENTER_KEY = 10
    };
    #endif

    void gotoxy(int x, int y);
    char getch_();
    int DNSSelected = 0;

public:
    void run(const std::vector<DNSServer>& DNSServers);
};

void ui::gotoxy(int x, int y) {
    std::cout<<"\x1B["<<y<<";"<<x<<"H";
}

void ui::run(const std::vector<DNSServer>& DNSServers) {
    while (true) {
        char ch = getch_();
        switch (ch)
        {
        #if (defined (_WIN32) || defined (_WIN64))
        #elif (defined (LINUX) || defined (__linux__))
        case 27: switch(ch = getch_()) { case 91: switch(ch = getch_()) {
            case LEFT_ARROW_KEY:
                std::cout<<"Left ";
                    break;
            case RIGHT_ARROW_KEY:
                std::cout<<"Right ";
                break;
            case UP_ARROW_KEY:
                std::cout<<"Up ";
                break;
            case DOWN_ARROW_KEY:
                std::cout<<"Down ";
                break;
        } break; } break;
        #endif

        case 'r':
            std::cout<<"Restarting network... ";
            break;

        case 'f':
            std::cout<<"Flushing system DNS... ";
            break;

        case ENTER_KEY:
            std::cout<<"Setting DNS... ";
            break;

        case 'e':
            exit(0);

        default:
            break;
        }
    }
}

char ui::getch_()
{
    char ch;
    #if (defined (_WIN32) || defined (_WIN64))
    ch = getch();
    #elif (defined (LINUX) || defined (__linux__))
    static struct termios old, current;
    tcgetattr(0, &old);
    current = old;
    current.c_lflag &= ~ICANON;
    current.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &current);
    ch = getchar();
    tcsetattr(0, TCSANOW, &old);
    #endif
    return ch;
}
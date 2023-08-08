#include <iostream>
#include "dns_changer.hpp"
#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>
#include <conio.h>
#elif (defined (LINUX) || defined (__linux__))
#include <termios.h>
#endif

class ui {
private:
    #if (defined (_WIN32) || defined (_WIN64))
    Windows os;
    enum keyASCII {
        UP_ARROW_KEY = 72,
        DOWN_ARROW_KEY = 80,
        ENTER_KEY = 13
    };
    #elif (defined (LINUX) || defined (__linux__))
    Linux os;
    enum keyASCII {
        UP_ARROW_KEY = 65,
        DOWN_ARROW_KEY = 66,
        ENTER_KEY = 10
    };
    #endif    

    void gotoxy(int x, int y);
    char getch_();
    void displayDNSServers(const std::vector<DNSServer>& DNSServers);
    void displayStatus(std::string message);
    void displayCurrentDNS();
    int DNSServerSelected = 0;

public:
    void run(const std::vector<DNSServer>& DNSServers);
};

void ui::gotoxy(int x, int y) {
    std::cout<<"\x1B["<<y<<";"<<x<<"H";
}

void ui::run(const std::vector<DNSServer>& DNSServers) {
    os.clearTerminal();
    displayCurrentDNS();
    while (true) {
        displayStatus("Done");
        displayDNSServers(DNSServers);
        char ch = getch_();

        switch (ch) {
        #if (defined (_WIN32) || defined (_WIN64))
        case -32: switch(ch = getch_()) {
            case UP_ARROW_KEY:
                std::cout<<"Up ";
                break;
            case DOWN_ARROW_KEY:
                std::cout<<"Down ";
                break;
        } break;

        #elif (defined (LINUX) || defined (__linux__))
        case 27: switch(ch = getch_()) { case 91: switch(ch = getch_()) {
            case UP_ARROW_KEY:
                DNSServerSelected = DNSServerSelected > 0 ? DNSServerSelected - 1 : DNSServerSelected;
                break;
            case DOWN_ARROW_KEY:
                DNSServerSelected = DNSServerSelected < DNSServers.size() - 1 ? DNSServerSelected + 1 : DNSServerSelected;
                break;
        } break; } break;
        #endif

        case 'r':
            displayStatus("Restarting network...");
            os.restartNetwork();
            displayCurrentDNS();
            break;

        case 'f':
            displayStatus("Clearing DNS...");
            os.clearDNS();
            displayCurrentDNS();
            break;

        case ENTER_KEY:
            displayStatus("Setting DNS...");
            os.setDNS(DNSServers[DNSServerSelected]);
            displayCurrentDNS();
            break;

        case 'e':
            exit(0);

        default:
            break;
        }
    }
}

char ui::getch_() {
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

void ui::displayDNSServers(const std::vector<DNSServer>& DNSServers) {
    gotoxy(0, 4);
    for (auto i{0}; i<DNSServers.size(); i++)
        if (i == DNSServerSelected)
            std::cout<<"\033[1;31m>> "<<DNSServers[i].name<<": "<<DNSServers[i].IPs[0]<<", "<<DNSServers[i].IPs[1]<<" <<\033[0m"<<std::endl;
        else
            std::cout<<"   "<<DNSServers[i].name<<": "<<DNSServers[i].IPs[0]<<", "<<DNSServers[i].IPs[1]<<"   "<<std::endl;
}

void ui::displayStatus(std::string message)
{
    gotoxy(0, 0);
    std::cout<<"Status: ("<<message<<")                 ";
}

void ui::displayCurrentDNS()
{
    displayStatus("Getting system DNS...");
    gotoxy(0, 2);
    std::cout<<"System DNS: "<<os.getDNSServers()<<"                            ";
}
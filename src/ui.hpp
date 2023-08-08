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
    enum keyboardASCIIs {
        UP_ARROW_KEY = 72,
        DOWN_ARROW_KEY = 80,
        ENTER_KEY = 13
    };
    #elif (defined (LINUX) || defined (__linux__))
    Linux os;
    enum keyboardASCIIs {
        UP_ARROW_KEY = 65,
        DOWN_ARROW_KEY = 66,
        ENTER_KEY = 10
    };
    #endif

    void gotoxy(short x, short y);
    char getch_();
    void displayHeader();
    void displayDNSServers(const std::vector<DNSServer>& DNSServers);
    void displayStatus(std::string message);
    void displayCurrentDNS();
    int findLargestDNSServerNameSize();
    int largestDNSServerNameSize;
    int DNSServerSelected = 0;
    std::vector<DNSServer>* DNSServers;

public:
    ui(std::vector<DNSServer>* DNSServers)
    {
        this->DNSServers = DNSServers;
        largestDNSServerNameSize = findLargestDNSServerNameSize();
    }
    void run();
};

void ui::gotoxy(short x, short y) {
    #if (defined (_WIN32) || defined (_WIN64))
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);
    COORD c {x, y};
    SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), c);
    #elif (defined (LINUX) || defined (__linux__))
    std::cout<<"\x1B["<<y<<";"<<x<<"H";
    #endif
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

void ui::displayHeader()
{
    gotoxy(0, 0);
    std::cout<<"(e)xit, (r)estart network, (f)lush DNS";
}

void ui::displayDNSServers(const std::vector<DNSServer>& DNSServers) {
    for (int i{0}; i<DNSServers.size(); i++)
    {
        gotoxy(0, i + 5);
        std::cout<<(i == DNSServerSelected ? "\033[0;32m>> " : "   ")<<DNSServers[i].name;
        gotoxy(largestDNSServerNameSize + 6, i + 5);
        std::cout<<"[" << DNSServers[i].IPs[0]<<"]";
        gotoxy(largestDNSServerNameSize + 23, i + 5);
        std::cout<<"[" << DNSServers[i].IPs[1]<<(i == DNSServerSelected ? "]\033[0m" : "]")<<std::endl;
    }
}

void ui::displayStatus(std::string message)
{
    gotoxy(0, 2);
    std::cout<<"Status: ("<<message<<")                 ";
}

void ui::displayCurrentDNS()
{
    displayStatus("Getting system DNS...");
    gotoxy(0, 3);
    std::cout<<"System DNS: "<<os.getDNSServers()<<"                            ";
}

void ui::run() {
    os.clearTerminal();
    displayCurrentDNS();
    while (true) {
        displayHeader();
        displayStatus("Done");
        displayDNSServers(*DNSServers);

        char ch = getch_();
        switch (ch) {
        #if (defined (_WIN32) || defined (_WIN64))
        case -32: switch(ch = getch_()) {
            case UP_ARROW_KEY:
                DNSServerSelected = DNSServerSelected > 0 ? DNSServerSelected - 1 : DNSServerSelected;
                break;
            case DOWN_ARROW_KEY:
                DNSServerSelected = DNSServerSelected < DNSServers.size() - 1 ? DNSServerSelected + 1 : DNSServerSelected;
                break;
        }
        #elif (defined (LINUX) || defined (__linux__))
        case 27: switch(ch = getch_()) { case 91: switch(ch = getch_()) {
            case UP_ARROW_KEY:
                DNSServerSelected = DNSServerSelected > 0 ? DNSServerSelected - 1 : DNSServerSelected;
                break;
            case DOWN_ARROW_KEY:
                DNSServerSelected = DNSServerSelected < (*DNSServers).size() - 1 ? DNSServerSelected + 1 : DNSServerSelected;
                break;
        } break; }
        #endif
		break;

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
            os.setDNS((*DNSServers)[DNSServerSelected]);
            displayCurrentDNS();
            break;

        case 'e':
            exit(0);

        default:
            break;
        }
    }
}

int ui::findLargestDNSServerNameSize()
{
    int largestDNSServerNameSize {0};
    for (const auto& DNSServer : *DNSServers)
        if (DNSServer.name.size() > largestDNSServerNameSize)
            largestDNSServerNameSize = DNSServer.name.size();
    return largestDNSServerNameSize;
}
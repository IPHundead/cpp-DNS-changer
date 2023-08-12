#include "dns_changer.hpp"
#include <fstream>
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

    void displayHelp();
    void displayManual();
    void displayStatus(std::string message);
    void displayCurrentDNS();
    void displayDNSServersTableHeader();
    void displayDNSServersTableTail();
    void displayDNSServersTable();

    bool changeDNSToDefault();
    bool saveDefaultDNS(DNSServer* server);
    bool isUserChoosingDefaultDNS = false;

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
    COORD c {(short)(x != 0 ? x - 1 : x), (short)(y != 0 ? y - 1 : y)};
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

void ui::displayHelp()
{
    gotoxy(0, 0);
    std::cout<<"(e)xit, (r)estart network, (f)lush DNS, (c)hange DNS to default, (s)et the default DNS, (m)anual.";
}

void ui::displayManual()
{
    os.clearTerminal();
    // Complete this here
    std::cout<<"To back to the main page, press any key...";
    getch_();
    os.clearTerminal();
}

void ui::displayStatus(std::string message)
{
    gotoxy(0, 2);
    std::cout<<"Status: ("<<message<<")                                       ";
}

void ui::displayCurrentDNS()
{
    displayStatus("Getting system DNS...");
    gotoxy(0, 3);
    std::cout<<"System DNS: "<<os.getDNSServers()<<"                            ";
}

void ui::displayDNSServersTableHeader()
{
    gotoxy(0, 5);
    std::cout<<"+";
    for (int i=0; i<largestDNSServerNameSize + 40; i++)
        std::cout<<"=";
    std::cout<<"+"<<std::endl<<"|";
    for (int i=0; i<largestDNSServerNameSize + 40; i++)
        std::cout<<((i == largestDNSServerNameSize + 4 || i == largestDNSServerNameSize + 22) ? "|" : " ");
    std::cout<<"|";
    gotoxy(largestDNSServerNameSize + 10, 6);
    std::cout<<"Primary  IP";
    gotoxy(largestDNSServerNameSize + 27, 6);
    std::cout<<"Secondary  IP";
    gotoxy(largestDNSServerNameSize / 2 - 2, 6);
    std::cout<<"Server Name"<<std::endl<<"+";
    for (int i=0; i<largestDNSServerNameSize + 40; i++)
        std::cout<<((i == largestDNSServerNameSize + 4 || i == largestDNSServerNameSize + 22) ? "|" : "=");
    std::cout<<"+"<<std::endl;
}

void ui::displayDNSServersTableTail()
{
    for (int i=0; i<largestDNSServerNameSize + 42; i++)
        std::cout<<((i == 0 || i == largestDNSServerNameSize + 41) ? "+" : "=");
    std::cout<<std::endl;
}

void ui::displayDNSServersTable() {
    displayDNSServersTableHeader();
    for (int i{0}; i<(*DNSServers).size(); i++)
    {
        std::cout<<(i == DNSServerSelected ? "| \033[0;32m> " : "|   ")<<(*DNSServers)[i].name<<"\033[0m";
        gotoxy(largestDNSServerNameSize + 6, i + 8);
        std::cout<<(i == DNSServerSelected ? "|\033[0;32m " : "| ");
        gotoxy(largestDNSServerNameSize + 8 + ((16 - (*DNSServers)[i].IPs[0].size()) / 2), i + 8);
        std::cout<<(*DNSServers)[i].IPs[0]<<"\033[0m";
        gotoxy(largestDNSServerNameSize + 24, i + 8);
        std::cout<<(i == DNSServerSelected ? "|\033[0;32m " : "| ");
        gotoxy(largestDNSServerNameSize + 26 + ((16 - (*DNSServers)[i].IPs[1].size()) / 2), i + 8);
        std::cout<<(*DNSServers)[i].IPs[1]<<"\033[0m";
        gotoxy(largestDNSServerNameSize + 42, i + 8);
        std::cout<<"|"<<std::endl;
    }
    displayDNSServersTableTail();
}

void ui::run() {
    bool issue = false;
    os.clearTerminal();
    displayCurrentDNS();
    while (true) {
        displayHelp();
        displayStatus(issue == true ? "Failed" : isUserChoosingDefaultDNS ? "Choose your default DNS." : "Done");
        issue = false;
        displayDNSServersTable();

        char ch = getch_();
        switch (ch) {
        #if (defined (_WIN32) || defined (_WIN64))
        case -32: switch(ch = getch_()) {
            case UP_ARROW_KEY:
                DNSServerSelected = DNSServerSelected > 0 ? DNSServerSelected - 1 : DNSServerSelected;
                break;
            case DOWN_ARROW_KEY:
                DNSServerSelected = DNSServerSelected < (*DNSServers).size() - 1 ? DNSServerSelected + 1 : DNSServerSelected;
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
            isUserChoosingDefaultDNS = false;
            displayStatus("Restarting network...");
            issue = os.restartNetwork();
            displayCurrentDNS();
            break;

        case 'f':
            isUserChoosingDefaultDNS = false;
            displayStatus("Clearing DNS...");
            issue = os.clearDNS();
            displayCurrentDNS();
            break;

        case ENTER_KEY:
            if (isUserChoosingDefaultDNS)
            {
                displayStatus("Saving default DNS...");
                issue = saveDefaultDNS(&(*DNSServers)[DNSServerSelected]);
            } else {
                displayStatus("Setting DNS...");
                issue = os.setDNS((*DNSServers)[DNSServerSelected]);
            }
            displayCurrentDNS();
            isUserChoosingDefaultDNS = false;
            break;

        case 'c':
            isUserChoosingDefaultDNS = false;
            displayStatus("Setting default DNS...");
            issue = changeDNSToDefault();
            displayCurrentDNS();
            break;

        case 's':
            isUserChoosingDefaultDNS = true;
            break;

        case 'm':
            displayManual();
            displayHelp();
            displayStatus("Done");
            displayCurrentDNS();
            displayDNSServersTable();
            break;

        case 'e':
            exit(0);

        default:
            isUserChoosingDefaultDNS = false;
            break;
        }
    }
}

int ui::findLargestDNSServerNameSize()
{
    int largestDNSServerNameSize {0};
    for (const auto& DNSServer : *DNSServers)
        largestDNSServerNameSize = std::max((int)DNSServer.name.size(), largestDNSServerNameSize);
    return largestDNSServerNameSize;
}

bool ui::changeDNSToDefault()
{
    std::ifstream file("default_DNS.txt");
    bool issue = file.is_open() == false;

    if (!issue)
    {
        DNSServer DefaultDNSServer;
        std::string line;

        while(std::getline(file, line))
            if (DefaultDNSServer.IPs[0].empty())
                DefaultDNSServer.IPs[0] = line;
            else
                DefaultDNSServer.IPs[1] = line;

        issue = os.setDNS(DefaultDNSServer) || issue;
        file.close();
    }

    return issue;
}

bool ui::saveDefaultDNS(DNSServer* server)
{
    std::ofstream defaultDNSFile ("default_DNS.txt");
    bool issue = defaultDNSFile.is_open() == false;
    if (!issue)
    {
        defaultDNSFile<<server->IPs[0]<<"\n";
        defaultDNSFile<<server->IPs[1];
        defaultDNSFile.close();
    }
    return issue;
}
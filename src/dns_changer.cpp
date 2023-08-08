#include <iostream>
#include "dns_changer.hpp"

bool isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void showHelp() {
    std::cout<<"\tEnter 'list' or 'l' to show the DNS servers list."<<std::endl;
    std::cout<<"\tEnter the DNS server number in the DNS server list to change the OS DNS server."<<std::endl;
    std::cout<<"\tEnter '0' to clear the OS DNS server."<<std::endl;
    std::cout<<"\tEnter 'restart' or 'r' to restart the internet connection."<<std::endl;
    std::cout<<"\tEnter 'dns' or 'd' to get a list of the current DNS servers of OS."<<std::endl;
    std::cout<<"\tEnter 'clear' or 'c' to clear the terminal screen."<<std::endl;
    std::cout<<"\tEnter 'exit' or 'e' to exit the program."<<std::endl;
}

void showDNSServersList(std::vector<DNSServer> DNSServers) {
    for (size_t i=0; i<DNSServers.size(); i++)
        std::cout<<"\tName: "<<DNSServers[i].name<<", Number: "<<i+1<<", DNS: ("<<DNSServers[i].IPs[0]<<", "<<DNSServers[i].IPs[1]<<")"<<std::endl;
}

int main()
{
#ifdef _WIN32
    Windows os;
#elif _WIN64
    Windows os;
#elif __linux__
    Linux os;
#else
    return 1;
#endif

    std::vector<DNSServer> DNSServers {
        {"shecan", "178.22.122.100", "185.51.200.2"},
        {"electro", "78.157.42.100", "78.157.42.101"},
        {"radar game", "10.202.10.10", "10.202.10.11"}
    };

    std::cout<<"DNS Changer"<<std::endl;
    std::cout<<"Enter 'help' or 'h' to get more information."<<std::endl;
    while (true) {
        std::string command;
        std::cout<<"~> ";
        std::getline(std::cin, command);

        if (command == "exit" || command == "e")
            break;

        else if (command == "help" || command == "h")
            showHelp();

        else if (command == "list" || command == "l")
            showDNSServersList(DNSServers);

        else if (command == "clear" || command == "c")
            os.clearTerminal();

        else if (command == "restart" || command == "r")
            os.restartNetwork();

        else if (command == "dns" || command == "d")
            std::cout<<os.getDNSServers()<<std::endl;

        else if (isNumber(command)) {
            long DNSServerNumber {std::stol(command) - 1};
            if (DNSServerNumber == -1)
                os.clearDNS();
            else if (DNSServerNumber < 0 || DNSServerNumber >= DNSServers.size())
                std::cerr<<"Invalid server number."<<std::endl;
            else
                os.setDNS(DNSServers[DNSServerNumber]);
        } else
            std::cerr<<"Command not found!"<<std::endl;
    }

    return 0;
}
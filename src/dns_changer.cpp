#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

class DNSServer {
public:
	std::string name;
	std::array<std::string, 2> IPs;

    DNSServer(const std::string& name, const std::string& primaryIP, const std::string& secondaryIP)
        : name(name), IPs {primaryIP, secondaryIP} {}
};

class OS {
public:
    virtual ~OS() = default;
    virtual void setDNS(const DNSServer& server) = 0;
    virtual void clearDNS() = 0;
    virtual void clearTerminal() = 0;
    virtual void restartNetwork() = 0;
    virtual std::string getDNSServers() = 0;
};

class Windows : public OS {
public:
    void clearDNS() override {
        system("netsh interface ipv4 set dns \"Wi-Fi\" dhcp");
    }

    void setDNS(const DNSServer& DNSServer) override {
        std::string DNSSetterCommand("netsh interface ipv4 add dnsservers \"Wi-Fi\"");
        for (const auto& ip : DNSServer.IPs)
            DNSSetterCommand += " " + ip;
        system(DNSSetterCommand.c_str());
    }

    void clearTerminal() override {
        system("cls");
    }

    void restartNetwork() override {
        system("netsh interface set interface \"Wi-Fi\" admin=disable");
        system("netsh interface set interface \"Wi-Fi\" admin=enable");
    }

    std::string getDNSServers() override {
        FILE* pipe = popen("powershell -command \"(Get-WmiObject Win32_NetworkAdapterConfiguration | Where-Object { $_.DNSServerSearchOrder -ne $null }).DNSServerSearchOrder -join ', '\"", "r");
        if (!pipe) {
            std::cerr<<"popen failed."<<std::endl;
            return "";
        }

        std::array<char, 256> buffer;
        std::string DNSServers;
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
            DNSServers += buffer.data();
        pclose(pipe);

        DNSServers.erase(std::remove_if(DNSServers.begin(), DNSServers.end(), [](char c) { return c == '\n' || c == '\r'; }), DNSServers.end());
        return DNSServers;
    }
};

class Linux : public OS {
public:
    void clearDNS() override {
        system("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\" \"$(ip -o -4 route show to default | awk '{print $3}' | tail -n 1)\"");
    }

    void setDNS(const DNSServer& DNSServer) override {
        std::string DNSSetterCommand("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" " + DNSServer.IPs[0] + " " + DNSServer.IPs[1]);
        system(DNSSetterCommand.c_str());
    }

    void clearTerminal() override {
        system("clear");
    }

    void restartNetwork() override {
        system("systemctl restart NetworkManager");
    }

    std::string getDNSServers() override {
        std::string DNSServers;
        FILE* pipe = popen("resolvectl status | awk '/DNS Servers:/ { for (i = 3; i <= NF; ++i) { dns_servers = dns_servers $i \", \"; } } END { sub(/, $/, \"\", dns_servers); print dns_servers }'", "r");
        if (pipe != nullptr) {
            std::array<char, 256> buffer;
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
                DNSServers += buffer.data();
            pclose(pipe);
        }

        DNSServers.erase(std::remove_if(DNSServers.begin(), DNSServers.end(), [](char c) { return c == '\n' || c == '\r'; }), DNSServers.end());
        return DNSServers;
    }
};

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
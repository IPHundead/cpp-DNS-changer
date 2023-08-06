#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

class DNSServer {
public:
	std::string name;
	std::array<std::string, 2> ip;

    DNSServer(const std::string& name, const std::string& ip1, const std::string& ip2)
        : name(name), ip{ ip1, ip2 } {}
};

class OS {
public:
    virtual ~OS() = default;
    virtual void setDNS(const DNSServer& server) = 0;
    virtual void clearDNS() = 0;
    virtual void clearTerminal() = 0;
    virtual void restartNetwork() = 0;
    virtual std::string getDNS() = 0;
};

class Windows : public OS {
public:
    void clearDNS() override {
        system("netsh interface ipv4 set dns \"Wi-Fi\" dhcp");
    }

    void setDNS(const DNSServer& server) override {
        clearDNS();
        std::string DNSSetterCommand("netsh interface ipv4 add dnsservers \"Wi-Fi\" ");
        for (char i{0}; i<2; i++) {
            std::string DNSSetterCommand(DNSSetterCommand + " " + server.ip[i] + " index=" + std::to_string(i + 1));
            system(DNSSetterCommand.c_str());
        }
    }

    void clearTerminal() override {
        system("cls");
    }

    void restartNetwork() override {
        system("netsh interface set interface \"Wi-Fi\" admin=disable");
        system("netsh interface set interface \"Wi-Fi\" admin=enable");
    }

    std::string getDNS() override {
        FILE* pipe = popen("powershell -command \"(Get-WmiObject Win32_NetworkAdapterConfiguration | Where-Object { $_.DNSServerSearchOrder -ne $null }).DNSServerSearchOrder -join ', '\"", "r");
        if (!pipe) {
            std::cerr<<"popen failed."<<std::endl;
            return "";
        }

        std::array<char, 128> buffer;
        std::string result;
        while (!feof(pipe)) {
            if (fgets(buffer.data(), 128, pipe) != nullptr)
                result += buffer.data();
        }
        pclose(pipe);

        result.erase(std::remove_if(result.begin(), result.end(), [](char c) { return c == '\n' || c == '\r'; }), result.end());
        return result;
    }
};

class Linux : public OS {
public:
    void clearDNS() override {
        system("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\" \"$(ip -o -4 route show to default | awk '{print $3}' | tail -n 1)\"");
    }

    void setDNS(const DNSServer& server) override {
        const std::string dns_setter_command("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" " + server.ip[0] + " " + server.ip[1]);
        system(dns_setter_command.c_str());
    }

    void clearTerminal() override {
        system("clear");
    }

    void restartNetwork() override {
        system("systemctl restart NetworkManager");
    }

    std::string getDNS() override {
        std::string dns_servers;
        FILE* pipe = popen("resolvectl status | awk '/DNS Servers:/ { for (i = 3; i <= NF; ++i) { dns_servers = dns_servers $i \", \"; } } END { sub(/, $/, \"\", dns_servers); print dns_servers }'", "r");
        if (pipe != nullptr) {
            char buffer[128];
            while (fgets(buffer, 128, pipe) != nullptr)
                dns_servers += buffer;
            pclose(pipe);
        }

        if (!dns_servers.empty() && dns_servers.back() == '\n')
            dns_servers.pop_back();

        return dns_servers;
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
        std::cout<<"\tName: "<<DNSServers[i].name<<", Number: "<<i+1<<", DNS: ("<<DNSServers[i].ip[0]<<", "<<DNSServers[i].ip[1]<<")"<<std::endl;
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

        else if (command == "clear" || command == "c")
            os.clearTerminal();

        else if (command == "list" || command == "l")
            showDNSServersList(DNSServers);

        else if (command == "restart" || command == "r")
            os.restartNetwork();

        else if (command == "dns" || command == "d")
            std::cout<<os.getDNS()<<std::endl;

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
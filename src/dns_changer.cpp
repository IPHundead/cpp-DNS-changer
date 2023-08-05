#include <iostream>
#include <vector>
#include <array>

class dns_server
{
public:
	std::string name;
	std::string ip[2];

    void create(const std::string& name, const std::string& ip1, const std::string& ip2)
    {
        ip[0] = ip1;
        ip[1] = ip2;
        this->name = name;
    }
};

class OS
{
public:
    virtual void set_dns(dns_server server) = 0;
    virtual void clear_dns() = 0;
    virtual void clear_terminal() = 0;
    virtual void restart_network() = 0;
};

class Windows : public OS
{
public:
    void clear_dns()
    {
        if (system("netsh interface ipv4 set dns \"Wi-Fi\" dhcp") == 0)
            std::cout<<"\r\033[1ADNS cleared successfully."<<std::endl;
        else
            std::cerr<<"\r\033[1AAn error occurred in clearing DNS."<<std::endl;
    }

    void set_dns(dns_server server)
    {
        clear_dns();
        const std::string add_dns_cmd("netsh interface ipv4 add dnsservers \"Wi-Fi\"");
        for (char i = 0; i < 2; i++)
        {
            std::string dns_setter_cmd(add_dns_cmd + " " + server.ip[i] + " index=" + std::to_string(i + 1));
            if (system(dns_setter_cmd.c_str()) == 1)
            {
                std::cerr<<"\r\033[1AAn error occurred in setting DNS."<<std::endl;
                return;
            }
        }
        std::cout<<"\r\033[2ADNS has been set successfully!"<<std::endl;
    }

    void clear_terminal()
    {
        system("cls");
    }

    void restart_network()
    {
        if (system("netsh interface set interface \"Wi-Fi\" admin=disable") == 0)
            std::cout<<"\r\033[1AThe Internet was turned off successfully."<<std::endl;
        else
            std::cerr<<"\r\033[1AAn error occurred in turning off the internet."<<std::endl;

        if (system("netsh interface set interface \"Wi-Fi\" admin=enable") == 0)
            std::cout<<"\r\033[1AThe Internet was turned on successfully."<<std::endl;
        else
            std::cerr<<"\r\033[1AAn error occurred in turning on the internet."<<std::endl;
    }
};

class Linux : public OS
{
public:
    void clear_dns()
    {
        if (system("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\" \"$(ip -o -4 route show to default | awk '{print $3}' | tail -n 1)\"") == 0)
            std::cout<<"DNS cleared successfully."<<std::endl;
        else
            std::cerr<<"An error occurred in clearing DNS."<<std::endl;
    }

    void set_dns(dns_server server)
    {
        const std::string dns_setter_cmd("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" " + server.ip[0] + " " + server.ip[1]);
        if (system(dns_setter_cmd.c_str()) == 0)
            std::cout<<"DNS has been set successfully!"<<std::endl;
        else
            std::cerr<<"An error occurred in setting DNS."<<std::endl;
    }

    void clear_terminal()
    {
        system("clear");
    }
};

bool is_number(const std::string& str)
{
    std::string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) ++it;
    return !str.empty() && it == str.end();
}

void show_help()
{
    std::cout<<"\tEnter 'list' or 'l' to show the DNS servers list."<<std::endl;
    std::cout<<"\tEnter the DNS server number in the DNS server list to change the OS DNS server."<<std::endl;
    std::cout<<"\tEnter '0' to clear the OS DNS server."<<std::endl;
    std::cout<<"\tEnter 'restart' or 'r' to restart the internet connection."<<std::endl;
    std::cout<<"\tEnter 'clear' or 'c' to clear the terminal screen."<<std::endl;
    std::cout<<"\tEnter 'exit' or 'e' to exit the program."<<std::endl;
}

void show_server_list(std::vector<dns_server> dns_servers)
{
    for (auto i{0}; i<dns_servers.size(); i++)
        std::cout<<"\tName: "<<dns_servers[i].name<<", Number: "<<i+1<<", DNS: ("<<dns_servers[i].ip[0]<<", "<<dns_servers[i].ip[1]<<")"<<std::endl;
}

int main()
{
    #ifdef _WIN32
        Windows os;
    #elif _WIN64
        Windows os;
    #elif __linux__
        Linux os;
    #elif __APPLE__ || __MACH__
        return 0;
    #elif __FreeBSD__
        return 0;
    #elif __unix || __unix__
        return 0;
    #else
        return 0;
    #endif

    std::vector<dns_server> dns_servers(3);
    dns_servers[0].create("shecan", "178.22.122.100", "185.51.200.2");
    dns_servers[1].create("electro", "78.157.42.100", "78.157.42.101");
    dns_servers[2].create("radar game", "10.202.10.10", "10.202.10.11");

    std::cout<<"DNS Changer"<<std::endl;
    std::cout<<"Enter 'help' or 'h' to get more information."<<std::endl;
    while (true)
    {
        std::cout<<"cmd: ";
        std::string cmd;
        std::cin>>cmd;

        if (cmd == "exit" || cmd == "e")
            break;
        else if (cmd == "help" || cmd == "h")
            show_help();
        else if (cmd == "clear" || cmd == "c")
            os.clear_terminal();
        else if (cmd == "list" || cmd == "l")
            show_server_list(dns_servers);
        else if (cmd == "restart" || cmd == "r")
            os.restart_network();
        else if (is_number(cmd)) {
            if (cmd == "0")
                os.clear_dns();
            else
                os.set_dns(dns_servers[std::stoi(cmd) - 1]);
        } else
            std::cerr<<"Command not found!"<<std::endl;
    }

    return 0;
}

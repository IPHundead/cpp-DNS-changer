#include <iostream>
#include <vector>

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
};

class Windows : public OS
{
public:
    void clear_dns()
    {
        system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
    }

    void set_dns(dns_server server)
    {
        clear_dns();

        const std::string add_dns_cmd("netsh interface ipv4 add dnsservers \"Wi-Fi\"");
        for (char i = 0; i < 2; i++)
        {
            std::string dns_setter_cmd(add_dns_cmd + " " + server.ip[i] + " index=" + std::to_string(i + 1));
            system(dns_setter_cmd.c_str());
        }
    }
};

class Linux : public OS
{
public:
    void clear_dns()
    {
        system("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\" \"$(ip -o -4 route show to default | awk '{print $3}' | tail -n 1)\"");
    }

    void set_dns(dns_server server)
    {
        const std::string dns_setter_cmd("resolvectl dns \"$(ip -o -4 route show to default | awk '{print $5}')\" " + server.ip[0] + " " + server.ip[1]);
        system(dns_setter_cmd.c_str());
    }
};

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

    while (true)
    {
        std::cout << "Enter 0 to clear DNS server, or choose your DNS server (1-3), or -1 to exit: ";
        int dns_server_number;
        std::cin >> dns_server_number;

        if (dns_server_number == -1)
            break;
        else if (dns_server_number != 0 && dns_server_number >= 1 && dns_server_number <= 3)
            os.set_dns(dns_servers[dns_server_number - 1]);
        else
            os.clear_dns();
    }

    return 0;
}
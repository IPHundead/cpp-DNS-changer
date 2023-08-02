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

void clear_dns()
{
    system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
}

void set_dns(dns_server server)
{
    clear_dns();

    const std::string mycmd="netsh interface ipv4 add dnsservers \"Wi-Fi\"";
    for (std::string ip : server.ip)
    {
        std::string dnsSetterCommand (mycmd + " " + ip + " index=1");
        system(dnsSetterCommand.c_str());
    }
}

int main()
{
	std::vector<dns_server> servers(3);

	servers[0].create("shecan", "178.22.122.100", "185.51.200.2");
	servers[1].create("electro", "78.157.42.100", "78.157.42.101");
	servers[2].create("radar game", "10.202.10.10", "10.202.10.11");

	while (true)
	{
		system("cls");
		std::cout<<"0 to clear dns server, or choose your dns server:"<<std::endl;

    	int n;
		std::cin>>n;

        if (n != 0)
        {
			n--;
			set_dns(servers[n]);
        } else
			clear_dns();
	}

    return 0;
}

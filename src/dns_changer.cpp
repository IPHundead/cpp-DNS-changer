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

    const std::string add_dns_cmd ("netsh interface ipv4 add dnsservers \"Wi-Fi\"");
    for (int i{0}; i<2; i++)
    {
        std::string dns_setter_cmd (add_dns_cmd + " " + server.ip[i] + " index=" + std::to_string(i+1));
        system(dns_setter_cmd.c_str());
    }
}

int main()
{
	std::vector<dns_server> dns_servers(3);
	dns_servers[0].create("shecan", "178.22.122.100", "185.51.200.2");
	dns_servers[1].create("electro", "78.157.42.100", "78.157.42.101");
	dns_servers[2].create("radar game", "10.202.10.10", "10.202.10.11");

	while (true)
	{
		system("cls");

		std::cout<<"0 to clear dns server, or choose your dns server:"<<std::endl;
    	int dns_server_number;
		std::cin>>dns_server_number;

        if (dns_server_number != 0)
			set_dns(dns_servers[dns_server_number-1]);
        else
			clear_dns();
	}

    return 0;
}
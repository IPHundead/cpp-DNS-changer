#include "ui.hpp"

int main()
{
    std::vector<DNSServer> DNSServers {
    	{"Google", "8.8.8.8", "8.8.4.4"},
    	{"CloudFlare", "1.1.1.1", "1.0.0.1"},
        {"Shecan", "178.22.122.100", "185.51.200.2"},
        {"Electro", "78.157.42.100", "78.157.42.101"},
        {"RadarGame", "10.202.10.10", "10.202.10.11"},
        {"OpenDNS", "208.67.222.222", "208.67.220.220"}
    };

    ui DNSChangerUI(&DNSServers);
    DNSChangerUI.run();
}
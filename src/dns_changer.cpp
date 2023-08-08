#include "ui.hpp"

int main()
{
    std::vector<DNSServer> DNSServers {
        {"shecan", "178.22.122.100", "185.51.200.2"},
        {"electro", "78.157.42.100", "78.157.42.101"},
        {"radar game", "10.202.10.10", "10.202.10.11"}
    };

    ui DNSChangerUI;
    DNSChangerUI.run(DNSServers);

    return 0;
}
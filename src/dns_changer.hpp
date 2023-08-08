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
        clearDNS();
        for (char i=0; i<2; i++)
        {
            std::string DNSSetterCommand = "netsh interface ipv4 add dnsservers \"Wi-Fi\" " + DNSServer.IPs[i] + " index=" + std::to_string(i+1);
            system(DNSSetterCommand.c_str());
            std::cout<<"\033[1A";
        }
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
        system("nmcli con mod \"$(nmcli -t -f NAME c show --active | head -n1)\" ipv4.ignore-auto-dns yes");
        system("nmcli con mod \"$(nmcli -t -f NAME c show --active | head -n1)\" ipv4.dns \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\"");
    }

    void setDNS(const DNSServer& DNSServer) override {
        std::string connectionName = "$(nmcli -t -f NAME c show --active | head -n1)";
        system(("nmcli con mod " + connectionName + " ipv4.dns \"" + DNSServer.IPs[0] + " " + DNSServer.IPs[1] + "\"").c_str());
        system(("nmcli con mod " + connectionName + " ipv4.ignore-auto-dns yes").c_str());
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

#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

struct DNSServer {
	std::string name;
	std::array<std::string, 2> IPs;
    DNSServer() {}
    DNSServer(const std::string& name, const std::string& primaryIP, const std::string& secondaryIP)
        : name(name), IPs {primaryIP, secondaryIP} {}
};

class OS {
public:
    virtual ~OS() = default;
    virtual bool setDNS(const DNSServer& server) = 0;
    virtual bool clearDNS() = 0;
    virtual bool restartNetwork() = 0;
    virtual void clearTerminal() = 0;
    virtual std::string getDNSServers() = 0;
};

class Windows : public OS {
public:
    bool clearDNS() override {
        bool issue = system("netsh interface ipv4 set dns \"Wi-Fi\" dhcp > NUL") == true;
        issue = system("ipconfig /flushdns > NUL") == true || issue;
        return issue;
    }

    bool setDNS(const DNSServer& DNSServer) override {
        bool issue = clearDNS();
        for (char i=0; i<2; i++) {
            std::string DNSSetterCommand = "netsh interface ipv4 add dnsservers \"Wi-Fi\" " + DNSServer.IPs[i] + " index=" + std::to_string(i + 1) + " > NUL";
            issue = std::system(DNSSetterCommand.c_str()) == true || issue;
        }
        return issue;
    }

    bool restartNetwork() override {
        bool issue = system("netsh interface set interface \"Wi-Fi\" admin=disable > NUL");
        system("netsh interface set interface \"Wi-Fi\" admin=enable > NUL") == true || issue;
        return issue;
    }

    void clearTerminal() override {
        system("cls");
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
    bool clearDNS() override {
        bool issue = system("nmcli con mod \"$(nmcli -t -f NAME c show --active | head -n1)\" ipv4.ignore-auto-dns yes > /dev/null");
        issue = system("nmcli con mod \"$(nmcli -t -f NAME c show --active | head -n1)\" ipv4.dns \"$(ip -o -4 route show to default | awk '{print $3}' | head -n 1)\" > /dev/null") || issue;
        return issue;
    }

    bool setDNS(const DNSServer& DNSServer) override {
        std::string connectionName = "$(nmcli -t -f NAME c show --active | head -n1) > /dev/null";
        bool issue = system(("nmcli con mod " + connectionName + " ipv4.dns \"" + DNSServer.IPs[0] + " " + DNSServer.IPs[1] + "\" > /dev/null").c_str());
        issue = system(("nmcli con mod " + connectionName + " ipv4.ignore-auto-dns yes > /dev/null").c_str()) || issue;
        return issue;
    }

    bool restartNetwork() override {
        bool issue = system("sudo systemctl restart NetworkManager > /dev/null");
        return issue;
    }

    void clearTerminal() override {
        system("clear");
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
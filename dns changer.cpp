#include <windows.h>
#include <iostream>
using namespace std;
int main()
{
	
	
	
	int n;	
	
	while(true)
	{
		system("CLS");
		
		cout << "choose your profile.\n 1-shecan\n 2-electro\n 3-radargame\n 4-nodns";
		cout << endl;
		cin >> n;
		
		switch(n)
		{
			case 1:
				system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 178.22.122.100 index=1");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 185.51.200.2 index=2");
				break;
			case 2:
				system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 78.157.42.101 index=1");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 78.157.42.100 index=2");
				break;
			case 3:
				system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 10.202.10.10 index=1");
				system("netsh interface ipv4 add dnsservers \"Wi-Fi\" 10.202.10.11 index=2");
				break;	
			case 4:
				system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
				break;
			default:
				cout << "something has gone horribly wrong :|";
				break;
		}
		
	}

}
#include <windows.h>
#include <iostream>
using namespace std;

const string mycmd="netsh interface ipv4 add dnsservers \"Wi-Fi\"";

struct dnsserver{
	string name;
	string ip[2];
};

void cleardns()
{
	system("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all");
}

void changedns(string dns1, string dns2)
{
	string tempx;
	
	cleardns();
	
	tempx=mycmd+" "+dns1+" index=1";
	system(tempx.c_str());
	cout << "\n" << tempx;
	
	tempx=mycmd+" "+dns2+" index=2";
	system(tempx.c_str());
	cout << endl << tempx;
}

int main()
{
	dnsserver server[30];

	string temp;
	int n;	
	
	server[0].name="shecan";
	server[0].ip[0]="178.22.122.100";
	server[0].ip[1]="185.51.200.2";
	
	server[1].name="electro";
	server[1].ip[0]="78.157.42.100";
	server[1].ip[1]="78.157.42.101";
	
	server[2].name="radar game";
	server[2].ip[0]="10.202.10.10";
	server[2].ip[1]="10.202.10.11";
	
	

	while(true)
	{
		system("CLS");
		
		cout << "0 to clear dns server, or choose your dns server:\n"; 
		
		cin >> n;
		
		if (n==0)
		{
			cleardns();	
		}
		else
		{
			n=n-1;
			changedns(server[n].ip[0], server[n].ip[1]);
		}
	}

}
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <thread>
#include <future>
#include <errno.h>
#include "../Serial/Serial.h"

using namespace std;

#define COM_PORT "/dev/ttyUSB0"
#define COM_BUF_SIZE 512
#define PORT 12255

volatile bool serial_wait = true, serial_ready = false;

string buffer;

void serial_proc()
{
	cout << "Serial proc start" << endl;
	Serial my_serial(B115200, COM_PORT, COM_BUF_SIZE);
	if (!my_serial.connect())
	{
		cout << "COM port dont connect" << endl;
		return;
	}
	string temp;
	temp.reserve(COM_BUF_SIZE);
	while(true)
	{
		while(serial_wait);
		serial_ready = false;
		my_serial.read_str(temp);
		buffer.append(temp.c_str(), temp.length());
		serial_ready = true;
	}
}

void fileread_proc()
{
	cout << "Filereader proc start" << endl;
	while(true)
	{
		
	}
}

void client_proc(int client_ds)
{
	cout << "Client work!" << endl;
	serial_wait = false;
	usleep(50);
	while(true)
	{
		serial_wait = true;
		while(!serial_ready);
		if(buffer.empty()) 
		{
			serial_wait = false;
			continue;
		}
		write(client_ds, buffer.c_str(), buffer.length());
		buffer.clear();
		serial_wait = false;
	}
}

bool server_main_proc()
{
	cout << "Start server" << endl;
	int server_ds;
	int err;

	if(-1 == (server_ds = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) 
	{
		cerr << "Socket can not created!\n";
		return false;
	}
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons (PORT);
	addr.sin_addr.s_addr = INADDR_ANY ;

	err = bind(server_ds, (sockaddr*) &addr, sizeof(addr));
	if(err < 0) 
	{
		close(server_ds);
		cout << strerror(errno) << endl;
		return false;
	}

	if(listen(server_ds, 64) < 0) 
	{
		close(server_ds);
		cerr << "Listen error!" << endl;
		return false;
	}

	int client_ds;
	if (true) 
	{
		client_ds = accept(server_ds, NULL, NULL);
		thread client(&client_proc, client_ds);
		client.join();
	}
}

int main()
{
	cout << "Start work" << endl;
	thread t(&serial_proc);
	if(!server_main_proc())
		return 1;
	t.join();
}

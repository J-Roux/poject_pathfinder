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
volatile bool frame_wait = true, frame_ready = false;

string serial_buffer;
string frame_buffer;

void serial_proc()
{
	cout << "Serial proc start" << endl;
	Serial my_serial(B115200, COM_PORT, COM_BUF_SIZE);
	if (!my_serial.connect())
	{
		cout << "COM port dont connect" << endl;
		return;
	}
	
	while(true)
	{
		while(serial_wait);
		serial_ready = false;
		my_serial.read_str(serial_buffer);
		if(serial_buffer.empty())
		{
			serial_ready = true;
			continue;
		}
		serial_ready = true;
	}
}

void fileread_proc()
{
	cout << "Filereader proc start" << endl;
	int frame_ds;
	frame_ds = open("Frame_x.raw",  O_RDONLY);
	uint8_t temp[4194304];
	while(true)
	{
		while(frame_wait);
		frame_ready = false;		
		read(frame_ds, temp, 4194304);
		frame_buffer.assign((char*)temp);
		frame_ready = true;		
	}
}

void client_proc(int client_ds)
{
	cout << "Client work!" << endl;
	serial_wait = frame_wait= false;	
	usleep(500);
	string client_buffer;
	while(true)
	{
		serial_wait = frame_wait = true;
		while(!serial_ready || !frame_ready);
		if(serial_buffer.empty()) 
		{
			serial_wait = frame_wait = false;
			continue;
		}
		//client_buffer.append(frame_buffer);
		client_buffer.append(serial_buffer);		
		write(client_ds, client_buffer.c_str(), client_buffer.length());
		client_buffer.clear();
		serial_wait = frame_wait = false;
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
	client_ds = accept(server_ds, NULL, NULL);
	thread client(&client_proc, client_ds);
	client.join();
}

int main()
{
	cout << "Start work" << endl;
	thread filereader(&fileread_proc);
	thread ser(&serial_proc);
	if(!server_main_proc())
		return 1;
	ser.join();
	filereader.join();
}

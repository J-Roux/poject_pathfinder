#include <iostream>
#include <cstring>

using namespace std;

#include "Serial.h"

#define COM_PORT "/dev/ttyUSB0"
#define BUF_SIZE 512

int main()
{
	Serial test_serial(B38400, COM_PORT, BUF_SIZE);
	if (test_serial.connect())
		cout << "Good" << endl;
	else
	{
		cout << "Very bad!!" << endl;
		return 1;
	}
	/*string send("Print test echo"), echo;
	echo.reserve(BUF_SIZE);
	int i = 0;
	while (true)
	{
		cout << "Mess " << ++i << " " << send.c_str() << endl;
		test_serial.println(send);
		while (!test_serial.read_str(echo));
		cout << "Echo: " << echo.c_str() << endl;
	}*/
	string buffer;
	buffer.reserve(BUF_SIZE);
	while (true)
	{
		while (!test_serial.read_str(buffer));
		cout << "Data: " << buffer.c_str() << endl;
		sleep(50);
	}
	return 0;
}

/* Serial class. Designed to work with the COM-port emulation.
	It includes methods to read/write data from/in port.

	author: Max K. (StarGrif)*/

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#if (_WIN64 || _WIN32)
#define WIN 1
#elif defined(__linux) || defined(__unix) 
#define LINUX 1
#endif

#include <iostream>
#include <cstring>
#include <cstdint>

#ifdef WIN
#include <windows.h>

#elif LINUX

#include <unistd.h>  
#include <fcntl.h>   
#include <errno.h>   
#include <termios.h> 
#include <sys/types.h>
#include <sys/stat.h>

#define HANDLE int

#endif

#ifdef WIN
#define sleep(millisec) Sleep(millisec)
#elif LINUX
#define sleep(millisec) usleep(millisec * 1000)
#endif 


#ifdef WIN
enum BaudRate_Win :uint32_t
{
	B2400 = 2400,
	B4800 = 4800,
	B9600 = 9600,
	B19200 = 19200,
	B38400 = 38400,
	B57600 = 57600,
	B115200 = 115200,
	B230400 = 230400
};

typedef BaudRate_Win speed_t;
#endif

enum Serial_error :uint8_t
{
	NOTHING_ERROR,
	FILE_NOT_FOUND,
	SOMETHING_ERROR
};

struct Serial_settings
{
	speed_t baud_rate;
	std::string port;
	uint32_t buffer_length;
	// ...

	void clear()
	{
		this->baud_rate = B9600;
		this->port.clear();
		this->buffer_length = 0;
	}

	friend bool operator ==(const Serial_settings &A, const Serial_settings &B);
	friend bool operator !=(const Serial_settings &A, const Serial_settings &B);
};

class Serial
{
private:

	Serial_settings settings;

	HANDLE dSerial;		// descriptor COM-port

	bool connected;		// state connection        
	//COMSTAT status;		// something information about connect       
	Serial_error error;		// last error

public:

	/*** Constructors ***/
	Serial() : connected(false), dSerial(0), error(NOTHING_ERROR)
	{
		this->settings.clear();
	}

	Serial(Serial_settings settings) : settings(settings), connected(false), dSerial(0), error(NOTHING_ERROR) {}

	Serial(speed_t baud_rate, std::string port, uint16_t buffer_length) :
		settings({ baud_rate, port, buffer_length }), connected(false), dSerial(0), error(NOTHING_ERROR) {}
	/********************/

	bool connect();
	bool connect(Serial_settings new_settings);

	bool disconnect();
	bool is_connected();		// return state connection

	int32_t bytes_available();

	//Serial_settings get_curr_settings();

	int32_t read_raw(void * buffer, uint32_t count_bytes);		// read data from COM-port
	int32_t read_str(std::string &str);

	bool write_raw(void * buffer, uint32_t count_bytes);									// write data in COM-port
	bool println(std::string str);


	~Serial();
};

#endif 

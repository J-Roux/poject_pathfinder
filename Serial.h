/* Serial class. Âesigned to work with the COM-port emulation.
	It includes methods to read/write data from/in port.

	author: Max K. (StarGrif)*/

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <iostream>
#include <cstring>
#include <cstdint>
#include <windows.h>

enum BaudRate :uint32_t
{
	BR2400 = 2400,
	BR4800 = 4800,
	BR9600 = 9600,
	BR14400 = 14400,
	BR19200 = 19200,
	BR28800 = 28800,
	BR38400 = 38400,
	BR57600 = 57600,
	BR76800 = 76800,
	BR115200 = 115200,
	BR230400 = 230400,
	BR250000 = 250000
};

enum Serial_error :uint8_t
{
	NOTHING_ERROR,
	FILE_NOT_FOUND,
	SOMETHING_ERROR
};

struct Serial_settings
{
	BaudRate baud_rate;
	std::string port;
	uint32_t buffer_length;
	// ...

	void clear()
	{
		this->baud_rate = BR9600;
		this->port.clear();
		this->buffer_length = NULL;
	}

	friend bool operator ==(const Serial_settings &A, const Serial_settings &B);
	friend bool operator !=(const Serial_settings &A, const Serial_settings &B);
};

class Serial
{
private:

	Serial_settings settings;
	HANDLE hSerial;		// descriptor COM-port        
	bool connected;		// state connection        
	//COMSTAT status;		// something information about connect       
	Serial_error error;		// last error

public:

	/*** Constructors ***/
	Serial::Serial() : connected(false), hSerial(NULL), error(NOTHING_ERROR)
	{
		this->settings.clear();
	}

	Serial(Serial_settings settings) : settings(settings), connected(false), hSerial(NULL), error(NOTHING_ERROR) {}

	Serial(BaudRate baud_rate, std::string port, uint16_t buffer_length) :
		settings({ baud_rate, port, buffer_length }), connected(false), hSerial(NULL), error(NOTHING_ERROR) {}
	/********************/

	bool connect();
	bool connect(Serial_settings new_settings);

	bool disconnect();
	bool is_connected();		// return state connection

	int32_t bytes_available();

	//Serial_settings get_curr_settings();

	int32_t read(void * buffer, uint32_t count_bytes);		// read data from COM-port
	int32_t read_str(std::string &str);

	bool write(void * buffer, uint32_t count_bytes);									// write data in COM-port
	bool println(std::string str);


	~Serial();
};

#endif 
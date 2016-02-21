#include "Serial.h"
#include <tuple>

#define ARDUINO_WAIT_TIME 1500

bool operator ==(const Serial_settings &A, const Serial_settings &B)
{
	return tie(A.baud_rate, A.buffer_length, A.port) == tie(B.baud_rate, B.buffer_length, B.port);
}

bool operator !=(const Serial_settings &A, const Serial_settings &B)
{
	return !(A == B);
}

bool Serial::connect()
{
	Serial_settings zero_settings;
	zero_settings.clear();
	if (this->settings != zero_settings)
	{
		if (this->connected)
			this->disconnect();

		this->connected = false;

		this->hSerial = CreateFileA(this->settings.port.c_str(), GENERIC_READ | GENERIC_WRITE,
			false, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (this->hSerial == INVALID_HANDLE_VALUE)		// check connection
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				std::cerr << "Handle was not attached. "
					<< this->settings.port.c_str() << " not available." << std::endl;
				this->error = Serial_error::FILE_NOT_FOUND;
			}
			else
				std::cerr << "Error with connection." << std::endl;
			this->hSerial = NULL;
			this->settings.clear();
			this->error = Serial_error::SOMETHING_ERROR;
		}
		else        // set parameters
		{
			SetupComm(this->hSerial, settings.buffer_length, settings.buffer_length);

			DCB dcbSerialParams = { NULL };
			if (!GetCommState(this->hSerial, &dcbSerialParams))
				std::cerr << "Failed to get current serial parameters!" << std::endl;
			else
			{
				// Arduino parameters
				dcbSerialParams.BaudRate = settings.baud_rate;
				dcbSerialParams.ByteSize = 8;
				dcbSerialParams.StopBits = ONESTOPBIT;
				dcbSerialParams.Parity = NOPARITY;

				if (!SetCommState(hSerial, &dcbSerialParams))
					std::cerr << "Could not set Serial Port parameters." << std::endl;
				else
				{
					this->connected = true;
					Sleep(ARDUINO_WAIT_TIME);
					return true;
				}
			}
		}
	}
	else
		std::cerr << "Serial is not initialized." << std::endl;
	return false;
}

bool Serial::connect(Serial_settings new_settings)
{
	this->settings = new_settings;
	return this->connect();
}

bool Serial::disconnect()
{
	if (this->connected)
	{
		this->connected = false;
		CloseHandle(this->hSerial);
	}
	this->settings.clear();
	return true;
}

int32_t Serial::bytes_available()
{
	COMSTAT status;
	DWORD curr_error;
	if (ClearCommError(this->hSerial, &curr_error, &status))
	{
		return (uint32_t)status.cbInQue;
	}
	else
		std::cerr << "Error with getting status com-port" << std::endl;
	return -1;
}

int32_t Serial::read(void * buffer, uint32_t count_bytes)			// read data
{
	if (count_bytes > this->settings.buffer_length)
		count_bytes = this->settings.buffer_length;
	int32_t bytes = this->bytes_available();
	if (bytes != -1)
	{
		if (bytes > 0)
		{
			if (bytes < count_bytes)
				count_bytes = bytes;
			DWORD bytes_read = 0;
			if (ReadFile(this->hSerial, buffer, count_bytes, &bytes_read, NULL))
				return bytes_read;
		}
		else
			return NULL;
	}
	std::cerr << "Error with readind data." << std::endl;
	return -1;
}

int32_t Serial::read_str(std::string &str)
{
	size_t count_bytes = this->settings.buffer_length;
	int8_t * temp_buffer = new int8_t[count_bytes];
	memset(temp_buffer, 0, count_bytes);
	count_bytes = this->read(temp_buffer, count_bytes);
	if (count_bytes != -1)
	{
		if (count_bytes == NULL)
			return NULL;
		str.assign((char *)temp_buffer);
		return count_bytes;
	}
	return -1;
}

bool Serial::write(void * buffer, uint32_t count_bytes)
{
	while (count_bytes)
	{
		uint32_t bytes_to_write;
		if (count_bytes > this->settings.buffer_length)
		{
			bytes_to_write = this->settings.buffer_length;
			count_bytes -= this->settings.buffer_length;
		}
		else
		{
			bytes_to_write = count_bytes;
			count_bytes = 0;
		}
		if (!WriteFile(this->hSerial, buffer, bytes_to_write, NULL, NULL))
		{
			//ClearCommError(this->hSerial, &this->error, &this->status);			// save error and exit
			std::cerr << "Port is not avaliable for write." << std::endl;
			return false;
		}
	}
	return true;
}

bool Serial::println(std::string str)
{
	return this->write((void *)str.c_str(), str.length());
}

bool Serial::is_connected()
{
	return this->connected;
}

Serial::~Serial()
{
	if (this->connected)
		CloseHandle(this->hSerial);
}
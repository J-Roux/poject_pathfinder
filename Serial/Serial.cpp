#include "Serial.h"
#include <tuple>

#define ARDUINO_WAIT_TIME 1500

#ifdef WIN
#define closeSerial(descriptor) CloseHandle(descriptor)
#elif LINUX
#define closeSerial(descriptor) close(descriptor);
#endif 

bool operator ==(const Serial_settings &A, const Serial_settings &B)
{
	return tie(A.baud_rate, A.buffer_length, A.port) == tie(B.baud_rate, B.buffer_length, B.port);
}

bool operator !=(const Serial_settings &A, const Serial_settings &B)
{
	return !(A == B);
}

#ifdef WIN
bool Serial::connect()
{
	Serial_settings zero_settings;
	zero_settings.clear();
	if (this->settings != zero_settings)
	{
		if (this->connected)
			this->disconnect();

		this->connected = false;

		this->dSerial = CreateFileA(this->settings.port.c_str(), GENERIC_READ | GENERIC_WRITE,
			false, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (this->dSerial == INVALID_HANDLE_VALUE)		// check connection
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
			{
				std::cerr << "Descriptor was not attached. "
					<< this->settings.port.c_str() << " not available." << std::endl;
				this->error = Serial_error::FILE_NOT_FOUND;
			}
			else
				std::cerr << "Error with connection." << std::endl;
			this->error = Serial_error::SOMETHING_ERROR;
		}
		else        // set parameters
		{
			SetupComm(this->dSerial, settings.buffer_length, settings.buffer_length);

			DCB dcbSerialParams = { NULL };
			if (!GetCommState(this->dSerial, &dcbSerialParams))
				std::cerr << "Failed to get current serial parameters!" << std::endl;
			else
			{
				// Arduino parameters
				dcbSerialParams.BaudRate = settings.baud_rate;
				dcbSerialParams.ByteSize = 8;
				dcbSerialParams.StopBits = ONESTOPBIT;
				dcbSerialParams.Parity = NOPARITY;

				if (!SetCommState(dSerial, &dcbSerialParams))
				{
					std::cerr << "Could not set Serial Port parameters." << std::endl;
					this->error = Serial_error::SOMETHING_ERROR;
				}
				else
				{
					this->connected = true;
					sleep(ARDUINO_WAIT_TIME);
					return true;
				}
			}
		}
	}
	else
		std::cerr << "Serial is not initialized." << std::endl;
	this->dSerial = 0;
	this->settings.clear();
	return false;
}
#elif LINUX
bool Serial::connect()
{
	Serial_settings zero_settings;
	zero_settings.clear();
	if (this->settings != zero_settings)
	{
		this->connected = false;

		this->dSerial = open(this->settings.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

		if (this->dSerial == -1)
		{
			std::cerr << "Descriptor was not attached. "
				<< this->settings.port.c_str() << " not available." << std::endl;
			this->error = Serial_error::FILE_NOT_FOUND;
		}
		else
		{
			struct termios options;
			tcgetattr(this->dSerial, &options);

			cfsetispeed(&options, this->settings.baud_rate);
			cfsetospeed(&options, this->settings.baud_rate);

			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;

			if (tcsetattr(this->dSerial, TCSANOW, &options) == -1)
			{
				std::cerr << "Could not set Serial Port parameters." << std::endl;
				this->error = Serial_error::SOMETHING_ERROR;
			}
			else
			{
				this->connected = true;
				sleep(ARDUINO_WAIT_TIME);
				return true;
			}
		}
	}
	else
		std::cerr << "Serial is not initialized." << std::endl;
	this->dSerial = 0;
	this->settings.clear();
	return false;
}
#endif 

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
		closeSerial(this->dSerial);
	}
	this->settings.clear();
	return true;
}

int32_t Serial::bytes_available()
{
#ifdef WIN
	COMSTAT status;
	DWORD curr_error;
	if (ClearCommError(this->dSerial, &curr_error, &status))
	{
		return (uint32_t)status.cbInQue;
	}
	else
		std::cerr << "Error with getting status com-port" << std::endl;

	return -1;
#elif LINUX
	return 0;
#endif
}

int32_t Serial::read_raw(void * buffer, uint32_t count_bytes)			// read data
{
	if (count_bytes > this->settings.buffer_length)
		count_bytes = this->settings.buffer_length;
#ifdef WIN
	int32_t bytes = this->bytes_available();
	if (bytes != -1)
	{
		if (bytes > 0)
		{
			if (bytes < count_bytes)
				count_bytes = bytes;
			DWORD bytes_read = 0;
			if (ReadFile(this->dSerial, buffer, count_bytes, &bytes_read, NULL))
				return bytes_read;

		}
		else
			return 0;
	}
	std::cerr << "Error with readind data." << std::endl;
	return -1;
#elif LINUX
	return (int32_t)read(this->dSerial, buffer, count_bytes);
#endif	
}

int32_t Serial::read_str(std::string &str)
{
	size_t count_bytes = this->settings.buffer_length;
	int8_t * temp_buffer = new int8_t[count_bytes];
	memset(temp_buffer, 0, count_bytes);
	count_bytes = this->read_raw(temp_buffer, count_bytes);
	if (count_bytes != -1)
	{
		if (count_bytes == 0)
			return 0;
		str.assign((char *)temp_buffer);
		return count_bytes;
	}
	return -1;
}

bool Serial::write_raw(void * buffer, uint32_t count_bytes)
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
		int status;
#ifdef WIN
		status = WriteFile(this->dSerial, buffer, bytes_to_write, NULL, NULL);
#elif LINUX
		status = write(this->dSerial, buffer, bytes_to_write);
#endif 
		if (!status || (status == -1))
		{
			std::cerr << "Port is not avaliable for write." << std::endl;
			return false;
		}
	}
	return true;
}

bool Serial::println(std::string str)
{
	str.push_back('\n');
	return this->write_raw((void *)str.c_str(), str.length());
}

bool Serial::is_connected()
{
	return this->connected;
}

Serial::~Serial()
{
	if (this->connected)
		closeSerial(this->dSerial);
}

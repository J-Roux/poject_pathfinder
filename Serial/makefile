test: serial.o test_main.o 
	clear
	g++-4.7 -o test.exe Serial.o Test_main.o
serial.o: Serial.cpp Serial.h 
	g++-4.7 -Wall -std=c++11 -c Serial.cpp
test_main.o: Test_main.cpp Serial.h
	g++-4.7 -Wall -std=c++11 -c Test_main.cpp
clean:
	rm Test_main.o Serial.o test.exe

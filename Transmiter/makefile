transmiter: serial.o transmiter_main.o 
	clear
	g++ -pthread -o transmiter.exe Serial.o transmiter_main.o 
serial.o: ../Serial/Serial.cpp ../Serial/Serial.h 
	g++  -std=c++11 -c ../Serial/Serial.cpp 
transmiter_main.o: transmiter_main.cpp ../Serial/Serial.h
	g++  -pthread -std=c++11 -c transmiter_main.cpp 
clean:
	rm transmiter_main.o Serial.o transmiter.exe

all:
	g++ -Iv8/include hw.cpp -o hw v8/libv8.a -lpthread -lzmq

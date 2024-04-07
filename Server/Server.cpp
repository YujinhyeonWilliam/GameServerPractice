#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <windows.h>


volatile bool ready = false;

void Thread_1()
{
	while(false){}
	cout << " 탈출! " << endl;
}

int main()
{ 
	thread t1(Thread_1);
	this_thread::sleep_for(1s);

	ready = true;
	t1.join();
}

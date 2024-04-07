#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>

atomic<int> sum = 0;

void Add()
{
	for (int i = 0; i < 1'000'000; i++)
	{
		sum++;
	}
}

void Sub()
{
	for (int i = 0; i < 1'000'000; i++)
	{
		sum--;
	}
}

int main()
{ 
	sum.store(20); 

	int temp = sum.exchange(10);
	int prev = sum.fetch_add(30);

	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}

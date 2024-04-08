#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>
#include <queue>

mutex m;
queue<int> q;
HANDLE hEvent;
condition_variable cv;
void Producer()
{
	while (true)
	{
		unique_lock<mutex> lock(m);
		q.push(100);

		cv.notify_one();
	}
}

void Consumer()
{
	while (true)
	{
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; });

		int data = q.front();
		q.pop();
		cout << data << endl;
	}
}

int main()
{
	hEvent = ::CreateEvent(NULL/*보안 속성*/, FALSE/*수동 리셋*/, FALSE/*초기 상태*/, NULL/*이름*/);

	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	::CloseHandle(hEvent);
}

#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>

class User
{

};

class UserManager
{
public:
	User* GetUser(int id)
	{
		unique_lock<mutex> guard(_lock);

		auto findIt = _users.find(id);
		if (findIt == _users.end())
			return nullptr;

		return findIt->second;
	}

private:
	map<int, User*> _users;
	mutex _lock;
};

mutex m1;
mutex m2;

void Thread_1()
{
	for (int i = 0; i < 10'000; i++)
	{
		lock_guard<mutex> lockGuard1(m1);
		lock_guard<mutex> lockGuard2(m2);
	}
}

void Thread_2()
{
	for (int i = 0; i < 10'000; i++)
	{
		lock_guard<mutex> lockGuard2(m2); 
		lock_guard<mutex> lockGuard1(m1);
	}
}

int main()
{
	thread t1(Thread_1);
	thread t2(Thread_2);

	t1.join();
	t2.join();

	cout << "Jobs Done" << endl;
}

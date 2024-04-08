#include <iostream>
using namespace std;
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>
#include <queue>

class Knight : public enable_shared_from_this<Knight>
{
public:
	void Test()
	{
		move(shared_from_this());
	}

	void Move(shared_ptr<Knight> k)
	{

	}
};

using KnightRef = shared_ptr<Knight>;

int main()
{
	KnightRef knight(new Knight());
	knight->Test();
}

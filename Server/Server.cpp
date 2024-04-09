#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <map>
#include <queue>
using namespace std;
#include "ThreadManager.h"


// Overlapped IO (비동기 + 논블로킹)

const int32 BUF_SIZE = 1000;
struct Session
{
	WSAOVERLAPPED overlapped = {};
	//... Overlapped IO 에선 overlapped 변수가 맨 위이어야 함 
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUF_SIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data Recv Len Callback = " << recvLen << endl;

	// TODO 
	Session* session = (Session*)(overlapped);
	cout << session->recvBuffer << endl;
}

int main()
{
	SocketUtils::Init();

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	// 논블로킹 소켓화
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SocketUtils::SetReuseAddress(listenSocket, true);

	if (SocketUtils::BindAnyAddress(listenSocket, 7777) == false)
		return 0;

	if (SocketUtils::Listen(listenSocket) == false)
		return 0;

	// overlapped 함수를 건다 (WSARecv, WSASend)
	// - overlapped 함수 성공했는가?
	// -> 성공 했으면 결과 얻어서 처리
	// -> 실패했으면 사유 확인

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket;

		while(true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			
			// 문제
			return 0;
		}

		Session session = Session{ clientSocket };

		cout << "Client Connected" << endl;
		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUF_SIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Alertable Wait
					::SleepEx(INFINITE, TRUE);
				}
				else
				{
					// TODO
					break;
				}
			}

			//cout << "Data Received = " << session.recvBuffer << endl;
		}
	}

	SocketUtils::Clear();
}

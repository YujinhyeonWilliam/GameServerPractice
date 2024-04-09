﻿#include "pch.h"
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
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUF_SIZE] = {};
	int32 recvBytes = 0;
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0;
};


void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		// GQCS
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr; 

		// 메인에서 WSARecv를 통해 일감을 받았다면 아래 함수를 통과
		bool ret = ::GetQueuedCompletionStatus(iocpHandle, 
							&bytesTransferred, 
							(ULONG_PTR*)&session, 
							(LPOVERLAPPED*)&overlappedEx, 
							INFINITE);

		if (ret == false || bytesTransferred == 0)
			continue;

		cout << "Recv Data Len = " << bytesTransferred << endl;
		cout << "Recv Data IOCP = " << session->recvBuffer << endl;
		
		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUF_SIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, /*!!!*/ &overlappedEx->overlapped, NULL);
	}
}


int main()
{
	SocketUtils::Init();

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	SocketUtils::SetReuseAddress(listenSocket, true);

	if (SocketUtils::BindAnyAddress(listenSocket, 7777) == false)
		return 0;

	if (SocketUtils::Listen(listenSocket) == false)
		return 0;


	vector<Session*> sessionManager;

	// Completion Port
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// WorkerThreads
	for (int32 i = 0; i < 5; i++)
		GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });

	// Accept
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
			return 0;

		Session* session = new Session();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected" << endl;

		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/(ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUF_SIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0; 
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, /*!!!*/ &overlappedEx->overlapped, NULL);
	}

	SocketUtils::Clear();
}

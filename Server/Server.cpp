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

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			// 에러보다는 아직 소켓이 도착하지 않은 경우 뱉는 메세지
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

		}
		
		cout << "Client Connected!" << endl;

		// Recv
		while (true)
		{
			char recvBuffer[100];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				// 아직 커널리시브버퍼에 버퍼가 도착하지 않은 상태
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				// TODO

				break;
			}

			cout << "Recv Data = " << recvBuffer << endl;
			cout << "Recv Data Len = " << recvLen << endl;
		}
	}

	SocketUtils::Clear();
}

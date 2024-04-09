#include "pch.h"

// 1) 소켓 설정
// 2) 서버에 연결 요청
// 3) 통신

int main()
{
	SocketUtils::Init();

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);
	
	// connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			int32 error = ::WSAGetLastError();

			// 넌블러킹 상태인데 아직 리시브버퍼에 데이터가 도착하지 않은 것
			if (error == WSAEWOULDBLOCK)
				continue;

			// 이미 연결된 상태인데 한 번 더 연결할려고 했을 때
			if (error == WSAEISCONN)
				break;
		}
	}

	// send

	while (true)
	{
		char sendBuffer[100] = "Hello I am Client!";
		int32 sendLen = sizeof(sendBuffer);

		if (::send(clientSocket, sendBuffer, sendLen, 0) == SOCKET_ERROR)
		{
			// 넌블러킹 상태인데 아직 리시브버퍼에 데이터가 도착하지 않은 것
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
		}

		cout << "Send Data! Len = " << sendLen << endl;

		this_thread::sleep_for(1s);
	}

	SocketUtils::Clear();
}

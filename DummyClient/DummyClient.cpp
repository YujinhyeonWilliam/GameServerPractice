#include "pch.h"

// 1) 소켓 설정
// 2) 서버에 연결 요청
// 3) 통신

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// 1) 소켓 생성 (socket)
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	// 2) 주소/포트 번호 설정 (bind)
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// 3) 연결
	if (::connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	cout << " Connected To Server !" << endl;

	while (true)
	{
		char sendBuffer[100] = "Hello! I am Client!";
		int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0); 
		if (resultCode == SOCKET_ERROR) 
			return 0;

		char recvBuffer[100];
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
			return 0;

		cout << "Echo Data : " << recvBuffer << endl;

		// 1초에 한 번씩 송신
		this_thread::sleep_for(1s);
	}

	::closesocket(clientSocket);
	::WSACleanup();
}

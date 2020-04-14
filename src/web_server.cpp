#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
//定义SOCKET宏
#define SOCKET int

int main(void)
{
    //创建服务器SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.s_addr = INADDR_ANY;
	_ser.sin_port = htons(9898);
    //绑定端口
	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));
    //监听端口
	listen(_serSock, 1);

	while (1)
	{
		struct sockaddr_in _cli;
		int length = sizeof(_cli);
        //创建客户端SOCKET
		SOCKET _cliSock = accept(_serSock, (struct sockaddr*)&_cli, (socklen_t*)&length);
		if (_cliSock < 0)
		{
			printf("errno\n");
		}
		else {
			char request[1024];
            //接收客户端消息
			recv(_cliSock, request, 1023, 0);
			request[strlen(request) + 1] = '\0';
			printf("%s\n", request);
			printf("successeful!\n");
			char buf[520] = "HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";
            //发送报头
			send(_cliSock, buf, strlen(buf), 0);
			int fd = open("index.html", O_RDONLY);
            //发送网页
			sendfile(_cliSock, fd, NULL, 2500);
			close(fd);
            //清理客户端SOCKET
			close(_cliSock);
		}
	}
    //清理服务器SOCKET
	close(_serSock);
	return 0;
}
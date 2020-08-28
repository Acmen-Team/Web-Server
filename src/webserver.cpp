#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define SOCKET int
char requestLine[512];
char URL[128];
//获取请求行
char *getRequestLine(char *);
//获取URL
char *getURL(char *);

int main(void)
{
	
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.s_addr = INADDR_ANY;
	_ser.sin_port = htons(9898);

	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));
	listen(_serSock, 1);

	while (true)
	{
		sockaddr_in _cli;
		char _url[128];
		int cliSize = sizeof(_cli);
		SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, (socklen_t*)&cliSize);
		char request[1024];
		recv(_cliSock, request, 1023, 0);
		request[strlen(request) + 1] = '\0';
		strcpy(_url, getURL(getRequestLine(request)));
		if(strcmp(_url, "/") == 0)
		{
			char buf[520]="HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";//HTTP响应
            int s = send(_cliSock,buf,strlen(buf),0);//发送响应
			int fd = open("index.html",O_RDONLY);//消息体
            sendfile(_cliSock,fd,NULL,5000);//零拷贝发送消息体
            close(fd);
            close(_cliSock);
		}
		else
		{
			char buf[520]="HTTP/1.1 200 ok\r\nconnection: close\r\n\r\n";//HTTP响应
            int s = send(_cliSock,buf,strlen(buf),0);//发送响应
			char str[512] = {"."};
			strcat(str, _url);
            int fd = open(str,O_RDONLY);//消息体
            sendfile(_cliSock,fd,NULL,205024);//零拷贝发送消息体
            close(fd);
            close(_cliSock);
		}
	}
	close(_serSock);
	return 0;
}
//获取请求行
char * getRequestLine(char *request)
{
	//清空字符串
	memset(requestLine, 0, sizeof(requestLine));
	char *rl = NULL;
	rl = requestLine;
	while (*request != '\n')
	{
		*rl = *request;
		rl++;
		request++;
	}
	requestLine[strlen(requestLine) + 1] = '\0';

	return requestLine;
}
//获取请求URL
char * getURL(char *RequestLine)
{
	//清空字符串
	memset(URL, 0, sizeof(URL));
	unsigned long int len = strlen(RequestLine);
	int x = 0;
	bool block = false;
	for (int i = 0; i < len; i++)
	{
		if (RequestLine[i] == ' ' || block == true)
		{
			block = true;
			if (RequestLine[i + 1] != ' ')
			{
				URL[x] = RequestLine[i + 1];
				x++;
			}
			else
				break;
		}
	}
	URL[strlen(URL) + 1] = '\0';
	return URL;
}


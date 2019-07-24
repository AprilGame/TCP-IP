#include<iostream>
using namespace std;
#include<thread>
#include<mutex>
#include<string.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

const int BUF_SIZE=100;
const int MAX_CONT=256;
const int PORT=8000;

int connectNum=0;		//客户端连接数量
int clientArray[MAX_CONT];	//已连接的客户端

mutex mtx;		//信息量


//发送消息
void sendMsg(char *msg,int len)
{
	mtx.lock();
	for(int i=0;i<connectNum;i++)
	{
		write(clientArray[i],msg,len);
	}
	mtx.unlock();
}


//处理客户端的连接
void handleConnect(int sockfd)
{
	int len=0;
	char msg[BUF_SIZE];

	while((len=read(sockfd,msg,sizeof(msg)))!=0)
	{
		sendMsg(msg,len);
	}

	//删除离线客户端
	mtx.lock();
	for(int i=0;i<connectNum;i++)
	{
		if(clientArray[i]==sockfd)
		{
			while(i++<connectNum-1)
			{
				clientArray[i]=clientArray[i+1];
			}
			--connectNum;
			break;
		}
	}
	mtx.unlock();
	close(sockfd);
}

int main()
{
	
	//创建sockfd
	int clientfd;
	int serverfd=socket(AF_INET,SOCK_STREAM,0);

	//创建sockaddr_in
	struct sockaddr_in clientAddr;
	struct sockaddr_in serverAddr;
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons(PORT);
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	//绑定
	if(bind(serverfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))==-1)
	{
		cerr<<"socket bind failed!"<<endl;
		return 0;
	}


	//监听
	if(listen(serverfd,10)==-1)
	{
		cerr<<"socket listen failed!"<<endl;
		return 0;
	}

	socklen_t addrlen=sizeof(clientAddr);
	while(1)
	{
		clientfd=accept(serverfd,(struct sockaddr*)&clientAddr,&addrlen);
		if(clientfd==-1)
		{
			cerr<<"socket accept failed!"<<endl;
			return 0;
		}
               
		mtx.lock();
		clientArray[connectNum++]=clientfd;
		mtx.unlock();

		//创建线程
		thread t(handleConnect,clientfd);
		t.detach();
		cout<<"a new client connected！"<<endl;
	}

	close(serverfd);
	return 0;
}


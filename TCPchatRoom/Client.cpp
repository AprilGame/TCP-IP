#include<iostream>
using namespace std;
#include<thread>
#include<string.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include<unistd.h>
#include<pthread.h>

const int BUF_SIZE=100;
const int NAME_SIZE=20;
const int PORT=8000;

char msg[BUF_SIZE];
char name[NAME_SIZE];

//发送消息
void sendMsg(int clientfd)
{
	char clientMsg[NAME_SIZE+BUF_SIZE];

	while(1)
	{
		fgets(msg,BUF_SIZE,stdin);

		//断开连接
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))
		{
			close(clientfd);
			exit(0);
		}
		sprintf(clientMsg,"%s %s",name,msg);

		//发送消息
		write(clientfd,clientMsg,strlen(clientMsg));

	}
}


//接收信息
void recvMsg(int clientfd)
{
	int len;
	char clientMsg[NAME_SIZE+BUF_SIZE];

	while((len=read(clientfd,clientMsg,NAME_SIZE+BUF_SIZE+1))!=-1)
	{
		clientMsg[len]=0;
		fputs(clientMsg,stdout);
	}

}

int main()
{

	cout<<"输入您的昵称："<<endl;
	cin>>name;

	//创建sockfd
	int clientfd=socket(AF_INET,SOCK_STREAM,0);

	//创建sockaddr_in
	struct sockaddr_in clientAddr;
	clientAddr.sin_family=AF_INET;
	clientAddr.sin_port=htons(PORT);
	clientAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	//连接
	if(connect(clientfd,(struct sockaddr*)&clientAddr,sizeof(clientAddr))==-1)
	{
		cerr<<"socket connect failed!"<<endl;
		return 0;
	}

	//创建线程
	thread t1(sendMsg,clientfd);
	thread t2(recvMsg,clientfd);
	t1.join();
	t2.join();

	close(clientfd);
	return 0;

}	


	

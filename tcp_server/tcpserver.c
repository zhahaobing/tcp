#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

typedef struct _SYSTEMTIME
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

static 	int     	SyncSocket1 	= 0;		//server socket
static 	int 		port_cli 		= 0;		//客户端端口号
static	char 		ipaddr_cli[32] 	= {0};		//客户端ip地址
static 	pthread_t 	g_ptClientEventTid[1024];

void 		LogInfo(const char *fmt, ...);
void 		GetTime(SYSTEMTIME *systime);
void 		GetLocalTime(LPSYSTEMTIME lpSystemTime);

int main(int argc, char *argv[])
{
	//定义Internet协议结构,客户端的IP信息
	struct sockaddr_in inaddr;
	struct in_addr in;	
    struct sockaddr_in inaddr_cli;
    int len = sizeof(inaddr_cli);
	int rc = 0;
	int ret = 0;

	memset(&inaddr, 0, sizeof(inaddr));
	inaddr.sin_family = AF_INET;
	inaddr.sin_port = htons(2404);
	inaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	LogInfo("start...,line=%d,func=%s.\n", __LINE__, __FUNCTION__);

	SyncSocket1 = socket(AF_INET, SOCK_STREAM, 0);
	if (SyncSocket1 <= 0)
	{
		LogInfo("SyncSocket1 error!line=%d,func=%s.\n", __LINE__, __FUNCTION__);
		return -1;
	}
	LogInfo("socket ok...,line=%d,func=%s.\n", __LINE__, __FUNCTION__);

	rc = bind(SyncSocket1, (struct sockaddr *)&inaddr, sizeof(inaddr));
	if (rc < 0) {
		LogInfo("bind error!line=%d,func=%s.\n", __LINE__, __FUNCTION__);
		return -1;
	}
	LogInfo("bind ok...,line=%d,func=%s.\n", __LINE__, __FUNCTION__);

	rc = listen(SyncSocket1, SOMAXCONN);
	if (rc < 0) {
		LogInfo("listen error!line=%d,func=%s.\n", __LINE__, __FUNCTION__);
		return -1;
	}
	LogInfo("listen ok ...,line=%d,func=%s.\n", __LINE__, __FUNCTION__);

	//接收消息
	while(1)
	{		
		int sock = 0;
		pthread_t pid;
		pthread_attr_t thread_attr;
		
		sock=accept(SyncSocket1, (struct sockaddr *)&inaddr_cli, (socklen_t *)&len);
		if(sock <0)
		{
			perror("accept error\n");
			close(SyncSocket1);
			return -1;
		}
		port_cli = ntohs(inaddr_cli.sin_port);
		in = inaddr_cli.sin_addr;
		inet_ntop(AF_INET, &in, ipaddr_cli, sizeof(ipaddr_cli));
		LogInfo("ip=%s,port=%d connected.\n", ipaddr_cli, port_cli);
		LogInfo("connect ok\n");
		
		pthread_attr_init(&thread_attr);
		pid = pthread_create(&g_ptClientEventTid, &thread_attr, ClientHandle, NULL);		
		if(pid != 0)
		{
			printf("pid = %lu,create pthread MainEventHandle failed!func:%s,line%d\n",pid,__func__,__LINE__);
		}

		usleep(500);
	}

	//关闭套接字
	close(SyncSocket1);
	close(sock);
	return 0;
}

void LogInfo(const char *fmt, ...)
{//服务器日志记录函数
	int 	   dwLength = 0;
    char       buf[32];
    va_list    vargs;

    /* open log file */
    FILE *fp = fopen("/mnt/zhahaobing_sync/sync_server.log", "at");
    if (fp == NULL)
	{
		system("mkdir -p /mnt/zhahaobing_sync/");
		fp = fopen("/mnt/zhahaobing_sync/sync_server.log", "at");
		if (fp == NULL) return;
    }
	fseek(fp,0,SEEK_END);
	dwLength = ftell(fp);
	if(dwLength > 10*1024*1204)	//如果数据大于10M，则文件数据清除，从头开始写
	{
		fclose(fp);
		fp = fopen("/mnt/zhahaobing_sync/sync_server.log","w+");
		if(fp == NULL)
		{
			return;
		}
		fseek(fp,0,SEEK_SET);
	}

	SYSTEMTIME systime;
	GetTime(&systime);
	sprintf(buf,"%d-%.2d-%.2d %.2d:%.2d:%.2d.%03d\t",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds);
    fprintf(fp, "%s ", buf);

    /* output message */
    va_start(vargs, fmt);
    vfprintf(fp, fmt, vargs);
    va_end(vargs);
    fprintf(fp, "\n");

    /* close log file */
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
}

void GetTime(SYSTEMTIME *systime)
{
#if 0
	time_t now;
	SYSTIME *psystime;
	time(&now);
	psystime = localtime(&now);
	memcpy(systime,psystime,sizeof(SYSTIME));
#else
	GetLocalTime(systime);
#endif
}

void GetLocalTime(LPSYSTEMTIME lpSystemTime)
{
    LPSYSTEMTIME st = lpSystemTime;
    //time_t t;
    //time(&t);

    struct tm *ptm;
    //ptm = localtime(&t);//masked by yy for SC-5160
	struct timeval valtime;
	struct tm  result;
	gettimeofday(&valtime, NULL);
    ptm = localtime_r(&valtime.tv_sec,&result);

    st->wYear = (unsigned short)ptm->tm_year + 1900;
    st->wMonth = (unsigned short)ptm->tm_mon + 1;
    st->wDayOfWeek = (unsigned short)ptm->tm_wday;
    st->wDay = (unsigned short)ptm->tm_mday;
    st->wHour = (unsigned short)ptm->tm_hour;
    st->wMinute = (unsigned short)ptm->tm_min;
    st->wSecond = (unsigned short)ptm->tm_sec;
    //st->wMilliseconds = 0;//masked by yy for SC-5160
	st->wMilliseconds = (unsigned short)(valtime.tv_usec / 1000);

}

void* 	ClientHandle(void* arg)
{
	// 分离线程，使主线程不必等待此线程  
	pthread_detach(pthread_self()); 
	int clientfd = *(int*)arg;	
	int recvBytes = 0;
	char* recvBuf = new char[BUFFER_SIZE];	
	memset(recvBuf, 0, BUFFER_SIZE);  
  
	while(1)  
	{  
		if ((recvBytes=recv(clientfd, recvBuf, BUFFER_SIZE,0)) <= 0)   
		{  
			perror("recv出错！\n");	
			break;	
		}  
		recvBuf[recvBytes]='\0';  
		printf("recvBuf:%s\n", recvBuf);  
	}  
  
	close(clientfd);  
	return NULL;  

}


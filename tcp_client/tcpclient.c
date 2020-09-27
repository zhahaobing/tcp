#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include "tcpclient.h"

void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock = -1;
    int i = 0;
    int flags = 0;
    struct sockaddr_in serv_addr;
    char message[1024];
    int str_len=0;
    int idx=0, read_len=0;

    if(argc!=3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
        error_handling("connect() error!");

    while(1)
    {
	/*read_len=read(sock, message, sizeof(message));
        if(read_len==-1) {
            error_handling("read() error!");
	}
	else if(read_len > 0) {
        	//str_len+=read_len;
    		LogInfo("Message from server: %s \n", message);
	}
	else {	
		memset(message, 0, sizeof(message));
		sprintf(message, "%08d:hello,zhazha\n", i++);

		write(sock, message, strlen(message));
	}*/
	memset(message, 0, sizeof(message));
	sprintf(message, "%08d:hello,zhazha\n", i++);

	write(sock, message, strlen(message));
	sleep(2);
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    LogInfo(message);
    exit(1);
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

void LogInfo(const char *fmt, ...)
{//·þÎñÆ÷ÈÕÖ¾¼ÇÂ¼º¯Êý
        int        dwLength = 0;
    char       buf[32];
    va_list    vargs;

    /* open log file */
    FILE *fp = fopen("/mnt/zhahaobing/tcp_client.log", "at");
    if (fp == NULL) {
                system("mkdir -p /mnt/zhahaobing/");
                fp = fopen("/mnt/zhahaobing/tcp_client.log", "at");
                if (fp == NULL) return;
    }
        fseek(fp,0,SEEK_END);
        dwLength = ftell(fp);
        if(dwLength > 10*1024*1204)     //Èç¹ûÊý¾Ý´óÓÚ10M£¬ÔòÎÄ¼þÊý¾ÝÇå³ý£¬´ÓÍ·¿ªÊ¼Ð´
        {
                fclose(fp);
                fp = fopen("/mnt/zhahaobing/tcp_client.log","w+");
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


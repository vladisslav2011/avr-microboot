#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "../firmware/bootloaderconfig.h"
#define BAUDRATE B9600

uint8_t * buf=NULL;
uint8_t * vbuf=NULL;
int openuart(char * fn)
{
	int fd=open(fn,O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(fd<0)
		return -1;
	struct termios cnew;
	if(tcgetattr(fd,&cnew)<0)
	{
		close(fd);
		return -1;
	};
	cnew.c_iflag=IGNBRK|IGNPAR;
	cnew.c_oflag=0;
	cnew.c_cflag=CS8|CREAD|CLOCAL;
	cnew.c_lflag=0;
	cfsetospeed(&cnew,BAUDRATE);
	cfsetispeed(&cnew,BAUDRATE);
	if(tcsetattr(fd,TCSANOW,&cnew)<0)
	{
		close(fd);
		return -1;
	};
	
	int cntrlbits=TIOCM_DTR;
	if(ioctl(fd,TIOCMBIS,&cntrlbits) < 0)
	{
		//error
		fprintf(stderr,"TIOCM_DTR failed\n");
	}
	
	if(ioctl(fd, TIOCMGET, &cntrlbits)==0)
	{
		cntrlbits |=  TIOCM_RTS;
		ioctl(fd, TIOCMSET, &cntrlbits);
	}else{
		fprintf(stderr,"TIOCM_RTS failed\n");
	}
	tcflush(fd,TCIOFLUSH);
	
	return fd;
}
int suart_io(int fd,void * outbuf,void *inbuf,int tx,int rx,int * rxed,int timeout)
{
	//struct termios cnew;
	struct pollfd pfd;
	pfd.fd=fd;
	pfd.events=POLLIN;
	while(poll(&pfd,1,1)!=0)
		if(read(fd,&((uint8_t *)inbuf)[0],1)==0)
			break;
	pfd.events=POLLOUT;
	int ok=poll(&pfd,1,timeout);
	if(ok<=0)
		return -9;
	int n=write(fd,outbuf,tx);
	//tcdrain(p->fd);
	if(n<tx)
		return -2;
	*rxed=0;
	pfd.events=POLLIN;
	do{
		ok=poll(&pfd,1,timeout);
		if(ok<=0)
			break;
		n=read(fd,&((uint8_t *)inbuf)[*rxed],rx-(*rxed));
		(*rxed)+=n;
	}while(*rxed<rx);
	return 0;
};




int openfile(char * fn)
{
	return open(fn,O_RDONLY);
}

int discover(int fd)
{
	char ib,ob;
	int rxed=0;
	while(1)
	{
		if(suart_io(fd,&ob,&ib,0,1,&rxed,100)==0)
		{
			if(rxed==1)
				if(ib=='B')
					printf("Detect B!\n");
		};
		ob=255;
		if(suart_io(fd,&ob,&ib,1,1,&rxed,300)==0)
		{
			if(rxed==1)
				if(ib==0x55)
				{
					printf("Found!\n");
					return 1;
				}
		}
	
	}
	return 0;
}


int getwritepagesize(int fd)
{
	char ib,ob;
	int rxed=0;
	ob=BOOT_FUNC_GET_PAGESIZE;
	if(suart_io(fd,&ob,&ib,1,1,&rxed,300)==0)
	{
		if(rxed==1)
			return ib;
	}
	return -1;
}

int read_page(int fd, int addr,uint8_t * buf)
{
	uint8_t ib;
	char ob[3];
	int rxed=0;
	int sz=0;
	ob[0]=BOOT_FUNC_READ_PAGE;
	ob[1]=addr&0xff;
	ob[2]=(addr>>8)&0xff;
	if(suart_io(fd,ob,buf,3,64,&rxed,2000)==0)
	{
		return rxed;
	}
	return -1;
}

void start_mcu(int fd)
{
	char ib,ob;
	int rxed=0;
	ob=BOOT_FUNC_LEAVE_BOOT;
	suart_io(fd,&ob,&ib,1,1,&rxed,100);
}

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage: %s /dev/ttyUSB0 filename.bin\n",argv[0]);
		return(2);
	};
	char * uart=argv[1];
	int ufd=openuart(uart);
	if(ufd<0)
	{
		fprintf(stderr,"Failed to open uart\n");
		return(2);
	}
	if(discover(ufd))
	{
		int cnt=0;
		buf=malloc(64);
		while(1)
		{
			int nwr=read_page(ufd,cnt,buf);
			cnt+=nwr;
			int k;
			for(k=0;k<nwr;k++)
			{
				if(!(k&0x1f))
					printf("\n");
				printf("%02x",buf[k]);
			}
			if(nwr<=0)
			{
				printf("\nnwr=%d\n",nwr);
				break;
			}
			if(cnt>=8192)
				break;
		}
		printf("\n");
		free(buf);
		close(ufd);
	}else{
		fprintf(stderr,"Failed to detect target device\n");
		return(2);
	}
	return 0;
}

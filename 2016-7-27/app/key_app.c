#include <stdio.h>
#include <stdlib.h>

int main()
{
	int fd;
	unsigned int key_number;
	//打开设备文件
	fd = open("/dev/key_lzy",0);
	//合法性验证
	if(fd<0)
	{
		printf("Open the device failed!!!\n");
	}
	//读取设备文件信息
	read(fd,&key_number,4);
	printf("key code is : %u\n",key_number);
	//关闭设备
	close(fd);
}










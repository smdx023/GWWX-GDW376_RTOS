
/*
 * eth_client.c
 * eth tcp comm in linux 
 * likejshy@126.com
 * 2017-01-16
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "dlt_104_lib.h"

//#define CFG_DEBUG

static int fd;


int eth_open(char *server_ip, unsigned short server_port)
{
	struct sockaddr_in  addr_info = {0};
	char ovr_s = 10;
	int ret;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		printf("socket fail!\r\n");
		return -1;
	}
	
	/* set socket no block */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	addr_info.sin_family = AF_INET;
	
	/* ip set as "192.168.158.1" */
	addr_info.sin_addr.s_addr = inet_addr(server_ip);
	addr_info.sin_port = htons(server_port);
	
	while (ovr_s--) {
		ret = connect(fd, (struct sockaddr *)(&addr_info), sizeof(struct sockaddr));
		if (ret == 0) {
			printf("cilent connet ok!\r\n");
			return 0;
		}

		sleep(1);
	}	
	
	printf("cilent connet fail!\r\n");
	return -1;
}





int eth_read(char *rxbuf, int size, int ovt_ms)
{
	int len = 0;
	
	while (1) {
		len = read(fd, rxbuf, size);
		if (len > 0) {
#ifdef CFG_DEBUG			
			print_data(0, rxbuf, len, " ");
#endif
			break;
		}

		if (ovt_ms > 0) {
			ovt_ms--;
			if (ovt_ms == 0)
				break;
		}

		usleep(1000);	
	}

	return len;
}


int eth_write(char *txbuf, int len)
{
	int ret;

	ret = write(fd, txbuf, len);
	if (ret < 0)
		printf("eth write error!\r\n");

	return 0;
}



void eth_close(void)
{
	close(fd);
}



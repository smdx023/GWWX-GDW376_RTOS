
/*
 * test_104.c
 * test dlt 104 ptl in linux sys
 * likejshy@126.com
 * 2017-02-9
 */
#include <stdio.h>
#include <unistd.h>

#include "eth_client.h"
#include "dlt_104.h"
#include "dlt_104_cfg.h"


int main(void)
{
	printf("\r\n\r\n\r\n");
	printf("**********************\r\n");
	printf("*    dlt 104 test    *\r\n");
	printf("*      by like       *\r\n");
	printf("**********************\r\n");

	eth_open("192.168.158.1", 8017);

	dlt_104_init(PORT_ETH);

	while(1) {
		dlt_104(PORT_ETH);
		sleep(1);
	}
	
	eth_close();
	return 0;
}


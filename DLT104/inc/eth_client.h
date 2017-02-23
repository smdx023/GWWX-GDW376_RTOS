/*
 * eth_client.h
 * eth tcp comm in linux 
 * likejshy@126.com
 * 2017-01-16
 */


#ifndef _ETH_CLIENT_H
#define _ETH_CLIENT_H

extern int eth_open(char *server_ip, unsigned short server_port);
extern int eth_read(char *rxbuf, int size, int ovt_ms);
extern int eth_write(char *txbuf, int len);
extern void eth_close(void);


#endif

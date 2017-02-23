
/*
 * Serial.h
 * likejshy@126.com
 * 2017-01-01
 */


#ifndef __SERIAL_H_
#define __SERIAL_H_

#define UART1_RX_BUFF_SIZE  512
#define UART2_RX_BUFF_SIZE  1024
#define UART5_RX_BUFF_SIZE  256


struct uart2_rec_ctl
{
	unsigned char  rxbuf[UART2_RX_BUFF_SIZE];
	unsigned short rxlen;	
	unsigned long interval_ms;
};


struct uart1_rec_ctl
{
	unsigned char  rxbuf[UART1_RX_BUFF_SIZE];
	unsigned short rxlen;	
	unsigned long interval_ms;
};

struct uart5_rec_ctl
{
	unsigned char  rxbuf[UART1_RX_BUFF_SIZE];
	unsigned short rxlen;	
	unsigned long interval_ms;
};




extern int uart2_send_byte(char *txbuf, int size);
extern int uart2_receive_packet(char *rxbuf, int size, 
				unsigned short interval_ms);


extern int uart1_write(char *txbuf, int size);
extern int uart1_read(char *rxbuf, int size, int ovt_ms);

extern int uart5_send_byte(char *txbuf, int size);
extern int uart5_receive_packet(char *rxbuf, int size, 
				unsigned short interval_ms);

#endif

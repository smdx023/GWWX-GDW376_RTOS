
#test.a:eth_client.o test_104.o
#	ar rcs ../obj/test.a eth_client.o test_104.o

ALL:eth_client.o test_104.o

eth_client.o:eth_client.c
	gcc-5 -c -Wall -g -I../inc eth_client.c -o ../obj/eth_client.o

test_104.o:test_104.c
	gcc-5 -c -Wall -g -I../inc test_104.c -o ../obj/test_104.o

clean:
	rm -vf *.o *.a 































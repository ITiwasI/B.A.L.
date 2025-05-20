all:
	gcc -Wall -Wextra -O2 tsock-v3.c -o tsock-v3

clean:
	rm -f tsock-v3

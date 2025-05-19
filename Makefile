CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = rpsd.o network.o

rpsd: $(OBJS)
	$(CC) $(CFLAGS) -o rpsd $(OBJS)

clean:
	rm -f rpsd *.o

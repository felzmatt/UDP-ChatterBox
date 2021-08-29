CC=gcc
CCOPTS=--std=gnu99 -Wall  -g
AR=ar

OBJS=list.o\

HEADERS=list.h

LIBS=liblist.a

BINS=list_test\

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

liblist.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)


list_test:	list_test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

server: main_server.c server.c $(LIBS) 
	$(CC) $(CCOPTS) -o $@ $^

client: main_client.c client.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)

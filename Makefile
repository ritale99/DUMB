all: DUMBserver.c DUMBclient.c
	gcc *r.c -lpthread -lm -o server.out; gcc *t.c -lpthread -lm -o client.out;

clean:
	rm *.out;

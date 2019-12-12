all: DUMBserver.c DUMBclient.c
	gcc *r.c -lpthread -lm -o DUMBserve; gcc *t.c -lm -o DUMBclient;

client: DUMBclient.c
	gcc *t.c -lm -o DUMBclient;

serve: DUMBserver.c
	gcc *r.c -lpthread -lm -o DUMBserve;

clean:
	rm DUMBserve DUMBclient;

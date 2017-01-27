LDFLAGS := -lportaudio
tx: build/tx
	./build/tx

rx: build/rx
	./build/rx

build/tx: build/tx.o
	gcc $(LDFLAGS) $^ -o $@

build/rx: build/rx.o
	gcc $(LDFLAGS) $^ -o $@

build/%.o: %.c
	gcc -c $^ -o $@

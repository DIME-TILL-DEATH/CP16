BIN=traceswo

all:
	gcc -Ofast -flto  bmp_traceswo.c -lusb-1.0 -ludev -lpthread -lrt -o $(BIN)
clean:
	rm -f $(BIN)

install:
	cp $(BIN) /opt/arm-kgp-eabi/bin

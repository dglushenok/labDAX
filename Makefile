CC=/opt/solarisstudio12.4/bin/cc
CFLAGS=-g -m64 -xcode=pic13 -xpagesize=4M -DDEBUG=1 -xO5
RM=rm -rf
OUT=bin/dax-in-range
INCLUDES=-I . -I include
LIBS=-L /usr/lib/sparcv9 -l dax_query -l dax -L lib

all:
	$(CC) $(CFLAGS) -o $(OUT) $(INCLUDES) src/dax-bit-vectors.c lib/vector.so $(LIBS)
#	$(CC) $(CFLAGS) -o $(OUT) $(INCLUDES) src/dax-in-range-array.c lib/vector.so $(LIBS)
#	$(CC) $(CFLAGS) -o $(OUT) $(INCLUDES) src/dax-in-range-text.c lib/vector.so $(LIBS)

clean:
	$(RM) $(OUT)
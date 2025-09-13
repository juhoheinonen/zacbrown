CC = cc
CFLAGS = -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SOURCE = zacbrown.c
OUTPUT = zacbrown 

all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
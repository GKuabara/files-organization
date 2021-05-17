# Paths for the files when not in current directory
vpath %.c . src
vpath %.h include

CC = gcc
MAIN = main
OUTPUT_OPTION = -o $@

INCLUDES = -I ./include 
CFLAGS = 
COMPILE.c = $(CC) $(DDEBUG) $(CFLAGS) $(INCLUDES) $(CPPFLAGS) $(TARGET_ARCH) -c 

LDLIBS =
LDFLAGS =
LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
DDEBUG = -g
DEBUGER = valgrind
DBFLAGS = --leak-check=full --show-leak-kinds=all\
		  --track-origins=yes --verbose 
TEST_CASE = 

clean: $(MAIN)
$(MAIN): main.o stream.o global.o vehicle.o operations.o funcao-fornecida.o


%: %.o
	$(LINK.o) $^ $(LDBIBS) $(OUTPUT_OPTION)

%.o: %.c
	$(COMPILE.c) $< $(OUTPUT_OPTION)

.PHONY: debug
debug:
	$(DEBUGER) $(DBFLAGS) ./$(MAIN) $(TEST_CASE)

.PHONY: clean
clean:
	rm -f *.o


SRC=$(wildcard *.c) # list of all .c files
OBJ=$(SRC:.c=.o) # for each .c file get a .o file
CFLAGS= -I/home/router/lib -I../..
LDFLAGS= #-L@top_srcdir@/lib -L@top_srcdir@/ospfd -L@top_srcdir@/ubpf/vm -L@top_srcdir@/pimd -lpim -lubpf -lfrrospf -lfrr

all: $(SRC) $(OBJ)

$(OBJ): %.o

%.o: %.c # .o constructed for dependance .c
	clang-3.9 -O2 -target bpf -c $< -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o

.PHONY: %.o # dependances systematically reconstructed

# Variabler
CC = gcc
CFLAGS = -std=gnu11 -g -Wall -Wextra
VFLAGS = --track-origins=yes --leak-check=full --show-leak-kinds=all --malloc-fill=0x40 --free-fill=0x23
RM = rm -rf
BIN = ruterdrift


# Forste target
all: $(BIN)


# Kompilerer program
ruter: ruterdrift.c
	$(CC) $(CFLAGS) ruterdrift.c -o ruterdrift


# Kjorer program
run: $(BIN)
	./$(BIN) topology_10_routers_15_edges.dat commands_10_routers_15_edges.txt


# Kjorer valgrind
val: $(BIN)
	valgrind $(VFLAGS) ./$(BIN) topology_10_routers_15_edges.dat commands_10_routers_15_edges.txt


# Fjerner kjorbar fil og opprettet binaerfil
clean:
	$(RM) $(BIN)
	$(RM) ruterdrift.dSYM
	$(RM) new-topology.dat



# Andre filer til run:
# ./$(BIN) topology_5_routers_fully_connected.dat
# ./$(BIN) topology_50_routers_150_edges.dat commands_50_routers_150_edges.txt


# Andre filer til val:
# valgrind $(VFLAGS) ./$(BIN) topology_5_routers_fully_connected.dat
# valgrind $(VFLAGS) ./$(BIN) topology_50_routers_150_edges.dat commands_50_routers_150_edges.txt

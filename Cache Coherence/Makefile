# Makefile
#
SH = bash
CFLAGS = -g
LDFLAGS = -lm

all: main

main: simulator.o main.o core.o msi_protocol.o emsi_protocol.o 
	g++ $(LDFLAGS) $^ -o $@

main.o: main.cpp simulator.h
	g++ -c $(CFLAGS) $< -o $@

simulator.o: simulator.cpp simulator.h core.h
	g++ -c $(CFLAGS) $< -o $@

core.o: core.cpp core.h cache_coherency_protocol.h msi_protocol.h emsi_protocol.h
	g++ -c $(CFLAGS) $< -o $@

msi_protocol.o: msi_protocol.cpp msi_protocol.h
	g++ -c $(CFLAGS) $< -o $@

emsi_protocol.o: emsi_protocol.cpp emsi_protocol.h
	g++ -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o main


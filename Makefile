all: runAll

runAll: phsp

phsp: phsp.c
	gcc phsp.c -o phsp -lm
	./run

clean:
	rm -rf *o hello

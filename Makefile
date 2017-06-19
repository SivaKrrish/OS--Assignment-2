swimmill:
	cc -lm -o fish.o fish.c 
	cc -lm -o pellet.o pellet.c 
	cc -o swim_mill swim_mill.c 
	chmod u+x swim_mill
clean : 
	rm fish.o pellet.o swim_mill


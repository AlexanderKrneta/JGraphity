.PHONY: run
grav: bin/grav.o bin/body.o bin/particle.o bin/quadtree.o bin/vec.o
	g++ -g -Wall -O3 bin/grav.o bin/body.o bin/particle.o bin/quadtree.o bin/vec.o -o grav
bin/grav.o: src/grav.cpp
	g++ -g -Iinclude -Wall -O3 -c src/grav.cpp -o bin/grav.o

bin/body.o: src/body.cpp include/body.h
	g++ -g -Iinclude -Wall -O3 -c src/body.cpp -o bin/body.o

bin/particle.o: src/particle.cpp include/particle.h
	g++ -g -Iinclude -Wall -O3 -c src/particle.cpp -o bin/particle.o

bin/quadtree.o: src/quadtree.cpp include/quadtree.h
	g++ -g -Iinclude -Wall -O3 -c src/quadtree.cpp -o bin/quadtree.o

bin/vec.o: src/vec.cpp include/vec.h
	g++ -g -Iinclude -Wall -O3 -c src/vec.cpp -o bin/vec.o

clean:
	rm bin/*.o grav

run:
	./grav bod1.txt .03 1 130
	./j -P bod1.jgr | ps2pdf - | convert -density 300 - -quality 100 bod1.jpg
	./grav bod2.txt .1 1 175
	./j -P bod2.jgr | ps2pdf - | convert -density 300 - -quality 100 bod2.jpg
	./grav bod3.txt .1 1 240
	./j -P bod3.jgr | ps2pdf - | convert -density 300 - -quality 100 bod3.jpg
	./grav bod5.txt .1 1 240
	./j -P bod5.jgr | ps2pdf - | convert -density 300 - -quality 100 bod5.jpg
	./grav bod4.txt .05 1 140
	./j -P bod4.jgr | ps2pdf - | convert -density 300 - -quality 100 bod4.jpg
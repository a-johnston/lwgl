build: src/lwgl.hpp src/*.cpp
	mkdir -p include bin lib
	cp src/lwgl.hpp include/lwgl.hpp
	cd bin/; \
	gcc -fpic -flto -fno-strict-aliasing -Os -Wall -Werror -Wextra -c ../src/*.cpp ../src/lwgl.hpp ;\
	gcc -shared -fpic -flto -fno-strict-aliasing -Os -Wall -Werror -Wextra -o ../lib/liblwgl.so *.o -lc

install: lib/liblwgl.so include/lwgl.hpp
	sudo cp lib/liblwgl.so /usr/lib/
	sudo cp include/lwgl.hpp /usr/local/include/

test: test.cpp
	g++ -o test.bin test.cpp -g -Og -Wall -Werror -Wextra -llwgl -lglfw -lGL -lGLEW -lX11 -lXrandr -lXi -lXxf86vm -lpthread -lm

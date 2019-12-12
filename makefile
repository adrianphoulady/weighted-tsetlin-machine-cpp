mnist: array.h fastrand.h weightm.h multiweightm.h utils.h implementations.cpp
	g++ -std=c++14 -O3 -Wall -Wextra -o mnist -Dmnist implementations.cpp

imdb: array.h fastrand.h weightm.h multiweightm.h utils.h implementations.cpp
	g++ -std=c++14 -O3 -Wall -Wextra -o imdb -Dimdb implementations.cpp

connect4: array.h fastrand.h weightm.h multiweightm.h utils.h implementations.cpp
	g++ -std=c++14 -O3 -Wall -Wextra -o connect4 -Dconnect4 implementations.cpp

clean:
	rm *.o mnist imdb connect4 connect4wl

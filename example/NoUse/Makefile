all: sbpool

sbpool: main.cpp pool.cpp
	g++ -g -o sbpool main.cpp pool.cpp -lmysqlclient -lpthread

clean:
	rm -f *.o accessdb sbpool core*
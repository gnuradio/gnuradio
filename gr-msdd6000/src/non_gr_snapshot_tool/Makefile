CCFLAGS	= -O3
LDFLAGS	= -lpthread
FFTW_LDFLAGS = -lfftw3f


all:	
	g++ $(CCFLAGS) -c client5.cc
	g++ $(CCFLAGS) -c msdd6000.cc
	g++ $(CCFLAGS) -o cap4 client5.o msdd6000.o $(LDFLAGS)


clean:
	rm test.out*

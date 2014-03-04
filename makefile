warmup2: warmup2.o func.o my402list.o
	gcc -o warmup2 -g warmup2.o my402list.o func.o -pthread -lm
warmup2.o: warmup2.c my402list.h func.h cs402.h
	gcc -g -c -Wall warmup2.c
my402list.o: my402list.c my402list.h cs402.h
	gcc -g -c -Wall my402list.c
func.o: func.c func.h my402list.h cs402.h
	gcc -g -c -Wall func.c

clean:
	rm -f *.o warmup2

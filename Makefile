output: raytraicer.o vector_math.o file_operation.o
	gcc raytraicer.c vector_math.c file_operation.c -lm -lpthread -o output

raytraicer.o: raytraicer.c
	gcc -c raytraicer.c

vector_math.o: vector_math.c vector_math.h
	gcc -c vector_math.c

file_operation.o: file_operation.c file_operation.h
	gcc -c file_operation.c

clean:
	rm *.o output
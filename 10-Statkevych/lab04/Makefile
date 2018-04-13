EXECS=./bin/main
TEST_CASES=./bin/generate_test_cases

MPICC?=mpicc

NODES_AMOUNT?=4
MATRIX_SIZE?=12
RAND_RANGE?=1000


all:
	mpirun -print-all-exitcodes -n ${NODES_AMOUNT} -profile ${EXECS}

generate_test_cases: test_cases
	${TEST_CASES} ${MATRIX_SIZE} ${RAND_RANGE}

test_cases: src/test_case_generator.c
	gcc -o bin/generate_test_cases src/test_case_generator.c 

main: src/main.c
	${MPICC} -o bin/main src/main.c

debug: src/main.c
	${MPICC} -o bin/main src/main.c -g

clean:
	rm ${EXECS}


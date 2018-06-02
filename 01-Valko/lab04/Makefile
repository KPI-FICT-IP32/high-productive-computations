CC=gcc
MPICC=mpicc
MPIRUN=mpirun
PAR=LU_mpi.c
EX_PAR=par.out
KEK=LU_mpi_counts.c
EX_KEK=kek.out
SEQ=LU_seq.c
EX_SEQ=seq.out
ifdef N
  SAMPLE:=$(N)
else
  SAMPLE=14
endif
ifdef P
  PROC:=$(P)
else
  PROC=4
endif
PRINT_ALU := $(shell if [[ ${SAMPLE} -le 15 ]]; then echo -DALU; fi)

default: seq mpi

.PHONY: seq mpi clean

$(EX_SEQ): $(SEQ)
	$(CC) $(SEQ) $(PRINT_ALU) -o $(EX_SEQ)

$(EX_PAR): $(PAR)
	$(MPICC) $(PAR) $(PRINT_ALU) -o $(EX_PAR)

$(EX_KEK): $(KEK)
	$(MPICC) $(KEK) $(PRINT_ALU) -o $(EX_KEK)

mpi: $(EX_PAR) 
	$(MPIRUN) -np $(PROC) ./$(EX_PAR) $(SAMPLE)

mpi2: $(EX_KEK)
	$(MPIRUN) -np $(PROC) ./$(EX_KEK) $(SAMPLE)

seq: $(EX_SEQ)
	./$(EX_SEQ) $(SAMPLE)

clean:
	$(RM) -f *.out

# Prerequisites

Docker installation required

# How it works

To run a container with installed GCC and MPI in interactive (bash) mode, simply run: 

```
sh ./entrypoint.sh
```

Docker will mount folder with the code inside the container. The following commands could be run within container:

 - To generate test cases:

```
make MATRIX_SIZE=20 RAND_RANGE=100 generate_test_cases
```

 - To run code:

```
make all
```

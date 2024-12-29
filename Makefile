CC=gcc
CARGS=-shared
CARGS-test=
CIN=src/src/*
CIN-test=./test.c
COUT=bin/cfs.so
COUT-test=bin/test

build-so:
	${CC} ${CARGS} ${CIN} -o ${COUT}

test: build-so
	${CC} ${CARGS-test} ${CIN-test} ${COUT} -o ${COUT-test}
	bash -c ${COUT-test}

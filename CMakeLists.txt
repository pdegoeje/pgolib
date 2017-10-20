cmake_minimum_required(VERSION 3.0.0)
project(pgolib)

add_library(pgolib 
	hash.c
	rational.c
	pcg.c
	minunit.c
	bin_coeff.c
)

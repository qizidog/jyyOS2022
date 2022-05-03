NAME := $(shell basename $(PWD))
export MODULE := M1
all: $(NAME)-64 $(NAME)-32

run: $(NAME)-64
	./$(NAME)-64

include ../Makefile

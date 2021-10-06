
# makefile for Mysql-bruteforce

CC = gcc
CFLAGS = -Ofast -Wall -Wextra -Wuninitialized -Wunused -Werror -std=gnu99 -s -lpthread
NAME = mysql
OUTPUT_NAME = mysql-bruteforce

MYSQLCFLAGS = $(shell mysql_config --cflags)
MYSQLLIBS = $(shell mysql_config --libs)

$(NAME):
	$(CC) $(NAME).c -o $(OUTPUT_NAME) $(CFLAGS) $(MYSQLCFLAGS) $(MYSQLLIBS)

install:
	sudo cp $(OUTPUT_NAME) /usr/local/bin/$(OUTPUT_NAME)
	@echo "Attempted to copy $(OUTPUT_NAME) to /usr/local/bin"

deps:
	sudo apt install libmysqlclient-dev


CC = gcc
CFLAGS = -Wall -Wextra -lcurl

all: web_crawler

web_crawler: web_crawler.c
	$(CC) $(CFLAGS) -o web_crawler web_crawler.c

clean:
	rm -f web_crawler

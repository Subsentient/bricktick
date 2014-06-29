
PREFIX = /usr/local

all:
	$(MAKE) -C src all
clean:
	rm -fv bricktick
	$(MAKE) -C src clean
install:
	install -m 755 bricktick $(PREFIX)

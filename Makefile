
PREFIX = /usr/local

all:
	$(MAKE) -C src all
clean:
	rm -f bricktick
	$(MAKE) -C src clean
install:
	install -m 755 bricktick $(PREFIX)/bin/
uninstall:
	rm -f $(PREFIX)/bin/bricktick

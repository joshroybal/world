CC = clang
CFLAGS = -g
LFLAGS = -lm lib/libcgic.a
PROG = bin/world.cgi
OBJS = obj/countries.o obj/main.o
HDRS = src/countries.h
DIRS = bin obj

obj/%.o: src/%.c $(HDRS)
	$(CC) -o $@ $(CFLAGS) -c $<

$(PROG): $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

install:
	sudo -u apache -g apache cp bin/world.cgi /srv/httpd/cgi-bin/world.cgi
	sudo -u apache -g apache cp -R dat/* /srv/httpd/cgi-bin/world
	sudo -u apache -g apache cp src/world.html /srv/httpd/htdocs

.PHONY : clean
clean:
	$(RM) $(PROG) obj/* src/*~ src/.*~ src/*.swp

$(shell mkdir -p $(DIRS))

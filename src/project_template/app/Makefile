



SHELL = /bin/sh
PLAT = linux
DIRS =  app
all: install

linux:
	make PLAT=linux install

macosx:
	make PLAT=macosx install

install:
	for d in $(DIRS); do \
		cd $$d; echo $$d; make PLAT=$(PLAT) install; cd ..;\
	done
clean:
	for d in $(DIRS); do \
		cd $$d; make clean; cd ..;\
	done

distclean:
	for d in $(DIRS); do \
		cd $$d; make distclean; cd ..;\
	done


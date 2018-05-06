



SHELL = /bin/sh
PLAT = linux
DIRS = hello cap1_ex1 cap1_ex2 cap1_ex3 cap2_ex1 cap2_ex2 cap2_ex3 cap2_ex4 cap2_ex5 cap2_ex6 cap3_ex1 cap3_ex2 cap3_ex3 cap3_ex4 cap3_ex5 cap4_ex1 cap4_ex2 cap5_ex1
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


INCLUDES = -I$(ARNOLD_ROOT)/include
LIBS = -L$(ARNOLD_ROOT)/bin

lgt_depth.so : lgt_depth.o 
	gcc-9 -o $@ -shared $(LIBS) -lai $<

lgt_depth.o : ./src/lgt_depth.cpp
	gcc-9 -o $@ -fPIC -O2 -c $(INCLUDES) $<

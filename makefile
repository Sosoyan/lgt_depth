INCLUDES = -I$(ARNOLD_ROOT)/include
LIBS = -L$(ARNOLD_ROOT)/bin

lgt_depth.so : lgt_depth.o 
	g++ -o $@ -shared $(LIBS) -lai $<

lgt_depth.o : ./src/lgt_depth.cpp
	g++ -o $@ -fPIC -O2 -c $(INCLUDES) $<

INCLUDES = -I$(ARNOLD_ROOT)/include
LIBS = -L$(ARNOLD_ROOT)/bin

lgt_depth.so : lgt_depth.o 
	g++ -o $@ -shared -std=c++11 $(LIBS) -lai $<

lgt_depth.o : ./src/lgt_depth.cpp
	g++ -o $@ -fPIC -O2 -std=c++11 -c $(INCLUDES) $<

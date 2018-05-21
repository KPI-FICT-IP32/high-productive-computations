PROG=app

CXX=g++
CXXFLAGS=
LIBS=-lrt  
TBBLIB=-ltbb 
TBBLIB_DEBUG=-ltbb_debug


all:	release

release: *.cpp
	echo $(CXX) "---" $(CXXFLAGS) "---" $(LIBS) "--" $(TBBLIB) "---" $(TBBLIB_DEBUG)
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o app app.cpp $(TBBLIB) $(LIBS)

debug: *.cpp
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o app app.cpp $(TBBLIB_DEBUG) $(LIBS)

clean:
	$(RM) app app_extended app_pretty *.o *.d

test:
	./$(PROG) $(ARGS)

light_test:
	./$(LIGHT_PROG) $(ARGS)

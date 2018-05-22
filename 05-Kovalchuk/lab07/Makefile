PROG=app

CXX=g++
CXXFLAGS=
LIBS=-lrt
TBBLIB=-ltbb
TBBLIB_DEBUG=-ltbb_debug


.PHONY: help
help:
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@echo "	release		Make a release build of the app"
	@echo "	debug		Make a debug build of the app"
	@echo "	clean		clean everything and exit"
	@echo "	help		Show this help and exit"


build:
	mkdir -p build/

.PHONY: debug
debug: build/app.debug
build/app.debug: src/*.cpp build
	$(CXX) -O0 -g -DTBB_USE_DEBUG $(CXXFLAGS) -o build/app.debug src/*.cpp $(TBBLIB_DEBUG) $(LIBS)


.PHONY: release
release: build/app.release
build/app.release: src/*.cpp build
	$(CXX) -O2 -DNDEBUG $(CXXFLAGS) -o build/app.release src/*.cpp $(TBBLIB) $(LIBS)


.PHONY: clean
clean:
	$(RM) -rf build *.o *.d

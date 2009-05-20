ROOTLIBS = $(shell root-config --libs)
ROOTINC = -I$(shell root-config --incdir)

TOPOBJ:=include/TrackedOpticalPhoton.hpp
TOPLINKDEF:=include/LinkDef.h

test:
ifeq ($(shell uname),Darwin)
	@echo 'This is Darwin.' 
endif

dictTOP.C: $(TOPOBJ)
	rootcint  

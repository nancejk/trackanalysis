ROOTLIBS = $(shell root-config --libs)
ROOTINC = -I$(shell root-config --incdir)

#For ROOTCINT
TOPOBJ:=include/TrackedOpticalPhoton.hpp
TOPLINKDEF:=include/LinkDef.h

#TRACKANALYSIS DEFS
TAOBJ:=

test:
ifeq ($(shell uname),Darwin)
	@echo 'This is Darwin.' 
endif

dictTOP.C: $(TOPOBJ)
	rootcint dictTOP.C -c -p $(TOPOBJ) $(TOPLINKDEF)

trackanalysis: 

ROOTCFLAGS	=	$(shell root-config --cflags)
ROOTGLIBS		=	$(shell root-config --glibs)

CPP					=	g++
CPPFLAGS		=	-g -fPIC -Wno-deprecated -O2 -ansi
LD					=	g++
LDFLAGS			=	-g
SOFLAGS			=	-shared

CPPFLAGS		+= $(ROOTCFLAGS)
NGLIBS			=	$(ROOTGLIBS)
NGLIBS			+= -L/afs/cern.ch/cms/slc5_amd64_gcc434/cms/cmssw/CMSSW_4_4_2/external/slc5_amd64_gcc434/lib -lMathMore -lMinuit -lRooFit -lRooFitCore -lFoam 
GLIBS				= $(filter-out -lNew, $(NGLIBS))

INCLUDEDIR	=	./
CPP					+= -I$(INCLUDEDIR)
CPP					+= -I/afs/cern.ch/cms/slc5_amd64_gcc434/lcg/roofit/5.28.00d/include
OUTLIB			= $(CMSSW_BASE)/lib

.SUFFIXES:	.cc,.C,.hh,.h
.PREFIXES:	$(CMSSW_BASE)/lib

RooHistPdfConv.o: $(INCLUDEDIR)/RooHistPdfConv.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooHistPdfConv.o $(NGLIBS) $<

RooExp2.o: $(INCLUDEDIR)/RooExp2.cpp
	$(CPP) $(CPPFLAGS) -c -o $(OUTLIB)/libRooExp2.o $(NGLIBS) $<

Tree2Datasets:	$(INCLUDEDIR)tree2Datasets.cpp
	$(CPP) $(CPPFLAGS) -o Tree2Datasets $(GLIBS) $ $<

Tree2DatasetsMC:	$(INCLUDEDIR)tree2Datasets_mc.cpp
	$(CPP) $(CPPFLAGS) -o Tree2DatasetsMC $(GLIBS) $ $<

Fit2DDataPbPb:	$(INCLUDEDIR)fit2DData_pbpb.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDataPbPb $(OUTLIB)/*.o $(GLIBS) $ $<

Fit2DDataTest:	$(INCLUDEDIR)fit2DData_test.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDataTest $(OUTLIB)/*.o $(GLIBS) $ $<

Fit2DDataPeePbPb:  $(INCLUDEDIR)fit2DData_pee_pbpb.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDataPeePbPb $(OUTLIB)/*.o $(GLIBS) $ $<

Fit2DDatapp:	$(INCLUDEDIR)fit2DData_pp.cpp
	$(CPP) $(CPPFLAGS) -o Fit2DDatapp $(OUTLIB)/*.o $(GLIBS) $ $<

CTauErr:	$(INCLUDEDIR)check_ctauErr.cpp
	$(CPP) $(CPPFLAGS) -o CTauErr $(OUTLIB)/*.o $(GLIBS) $ $<

clean:
	rm -f $(OUTLIB)*.o $(OUTLIB)*.so

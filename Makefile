APP=seshat
CC=g++
LINK=-lm
BUILDDIR=build
FLAGS = -std=c++23 -O3 -Wno-unused-result

OBJFEAS=$(BUILDDIR)/symfeatures.o $(BUILDDIR)/featureson.o $(BUILDDIR)/online.o
OBJMUESTRA=$(BUILDDIR)/sample.o $(BUILDDIR)/stroke.o
OBJPARSE=$(BUILDDIR)/seshat.o $(BUILDDIR)/meparser.o $(BUILDDIR)/gparser.o $(BUILDDIR)/grammar.o $(BUILDDIR)/production.o $(BUILDDIR)/symrec.o $(BUILDDIR)/duration.o $(BUILDDIR)/segmentation.o $(BUILDDIR)/sparel.o $(BUILDDIR)/gmm.o
OBJTABLA=$(BUILDDIR)/tablecyk.o $(BUILDDIR)/cellcyk.o $(BUILDDIR)/hypothesis.o $(BUILDDIR)/logspace.o
OBJRNNLIB=$(BUILDDIR)/Random.o $(BUILDDIR)/DataExporter.o $(BUILDDIR)/WeightContainer.o $(BUILDDIR)/ClassificationLayer.o $(BUILDDIR)/Layer.o $(BUILDDIR)/Mdrnn.o $(BUILDDIR)/Optimiser.o
RNNLIBHEADERS=rnnlib4seshat/DataSequence.hpp rnnlib4seshat/NetcdfDataset.hpp rnnlib4seshat/Mdrnn.hpp rnnlib4seshat/MultilayerNet.hpp rnnlib4seshat/Rprop.hpp rnnlib4seshat/SteepestDescent.hpp rnnlib4seshat/Trainer.hpp rnnlib4seshat/WeightContainer.hpp
OBJS=$(OBJFEAS) $(OBJMUESTRA) $(OBJPARSE) $(OBJTABLA) $(OBJRNNLIB)

seshat: $(BUILDDIR) $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(FLAGS) $(LINK)

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/seshat.o: seshat.cc $(BUILDDIR)/grammar.o $(BUILDDIR)/sample.o $(BUILDDIR)/meparser.o
	$(CC) -c seshat.cc $(FLAGS) -o $@

$(BUILDDIR)/production.o: production.h production.cc $(BUILDDIR)/symrec.o
	$(CC) -c production.cc $(FLAGS) -o $@

$(BUILDDIR)/grammar.o: grammar.h grammar.cc $(BUILDDIR)/production.o $(BUILDDIR)/gparser.o $(BUILDDIR)/symrec.o
	$(CC) -c grammar.cc $(FLAGS) -o $@

$(BUILDDIR)/meparser.o: meparser.h meparser.cc $(BUILDDIR)/grammar.o $(BUILDDIR)/production.o $(BUILDDIR)/symrec.o $(BUILDDIR)/tablecyk.o $(BUILDDIR)/cellcyk.o $(BUILDDIR)/logspace.o $(BUILDDIR)/duration.o $(BUILDDIR)/segmentation.o $(BUILDDIR)/sparel.o $(BUILDDIR)/sample.o $(BUILDDIR)/hypothesis.o
	$(CC) -c meparser.cc $(FLAGS) -o $@

$(BUILDDIR)/gparser.o: gparser.h gparser.cc
	$(CC) -c gparser.cc $(FLAGS) -o $@

$(BUILDDIR)/sample.o: vectorimage.h sample.h sample.cc $(BUILDDIR)/tablecyk.o $(BUILDDIR)/cellcyk.o $(BUILDDIR)/stroke.o $(BUILDDIR)/grammar.o
	$(CC) -c sample.cc $(FLAGS) -o $@

$(BUILDDIR)/symrec.o: vectorimage.h symrec.h symrec.cc $(BUILDDIR)/symfeatures.o $(RNNLIBHEADERS)
	$(CC) -c symrec.cc $(FLAGS) -o $@

$(BUILDDIR)/duration.o: duration.h duration.cc $(BUILDDIR)/symrec.o
	$(CC) -c duration.cc $(FLAGS) -o $@

$(BUILDDIR)/segmentation.o: segmentation.h segmentation.cc $(BUILDDIR)/cellcyk.o $(BUILDDIR)/sample.o $(BUILDDIR)/gmm.o
	$(CC) -c segmentation.cc $(FLAGS) -o $@

$(BUILDDIR)/tablecyk.o: tablecyk.h tablecyk.cc $(BUILDDIR)/cellcyk.o $(BUILDDIR)/hypothesis.o
	$(CC) -c tablecyk.cc $(FLAGS) -o $@

$(BUILDDIR)/cellcyk.o: cellcyk.h cellcyk.cc $(BUILDDIR)/hypothesis.o
	$(CC) -c cellcyk.cc $(FLAGS) -o $@

$(BUILDDIR)/hypothesis.o: hypothesis.h hypothesis.cc $(BUILDDIR)/production.o $(BUILDDIR)/grammar.o
	$(CC) -c hypothesis.cc $(FLAGS) -o $@

$(BUILDDIR)/logspace.o: logspace.h logspace.cc $(BUILDDIR)/cellcyk.o
	$(CC) -c logspace.cc $(FLAGS) -o $@

$(BUILDDIR)/sparel.o: sparel.h sparel.cc $(BUILDDIR)/hypothesis.o $(BUILDDIR)/cellcyk.o $(BUILDDIR)/gmm.o $(BUILDDIR)/sample.o
	$(CC) -c sparel.cc $(FLAGS) -o $@

$(BUILDDIR)/gmm.o: gmm.cc gmm.h
	$(CC) -c gmm.cc $(FLAGS) -o $@

$(BUILDDIR)/stroke.o: stroke.cc stroke.h
	$(CC) -c stroke.cc $(FLAGS) -o $@

$(BUILDDIR)/symfeatures.o: vectorimage.h symfeatures.cc $(BUILDDIR)/online.o $(BUILDDIR)/featureson.o
	$(CC) -c symfeatures.cc $(FLAGS) -o $@

$(BUILDDIR)/featureson.o: featureson.cc featureson.h $(BUILDDIR)/online.o
	$(CC) -c featureson.cc $(FLAGS) -o $@

$(BUILDDIR)/online.o: online.cc online.h
	$(CC) -c online.cc $(FLAGS) -o $@

#rnnlib4seshat
$(BUILDDIR)/Random.o: rnnlib4seshat/Random.cpp
	$(CC) -c rnnlib4seshat/Random.cpp $(FLAGS) -o $@

$(BUILDDIR)/DataExporter.o: rnnlib4seshat/DataExporter.cpp
	$(CC) -c rnnlib4seshat/DataExporter.cpp $(FLAGS) -o $@

$(BUILDDIR)/WeightContainer.o: rnnlib4seshat/WeightContainer.cpp
	$(CC) -c rnnlib4seshat/WeightContainer.cpp $(FLAGS) -o $@

$(BUILDDIR)/ClassificationLayer.o: rnnlib4seshat/ClassificationLayer.cpp
	$(CC) -c rnnlib4seshat/ClassificationLayer.cpp $(FLAGS) -o $@

$(BUILDDIR)/Layer.o: rnnlib4seshat/Layer.cpp
	$(CC) -c rnnlib4seshat/Layer.cpp $(FLAGS) -o $@

$(BUILDDIR)/Mdrnn.o: rnnlib4seshat/Mdrnn.cpp
	$(CC) -c rnnlib4seshat/Mdrnn.cpp $(FLAGS) -o $@

$(BUILDDIR)/Optimiser.o: rnnlib4seshat/Optimiser.cpp
	$(CC) -c rnnlib4seshat/Optimiser.cpp $(FLAGS) -o $@

clean:
	rm -r $(BUILDDIR)
	rm $(APP)

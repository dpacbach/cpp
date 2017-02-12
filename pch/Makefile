# User defined
target   := sample-application
sources  := $(wildcard *.cpp)
objects  := $(sources:%.cpp=%.o)

CXXFLAGS := -Wall
LD       := g++

all: $(target)

precomp.hpp.gch: precomp.hpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

%.o: %.cpp precomp.hpp.gch
	$(CXX) -c $< $(CXXFLAGS) -H -Winvalid-pch -include precomp.hpp -o $@
	@#$(CXX) -c $< $(CXXFLAGS) -H -Winvalid-pch -o $@

$(target) : precomp.hpp.gch $(objects)
	$(LD) -o $@ $(objects)

clean:
	rm -f precomp.hpp.gch $(objects) $(target)

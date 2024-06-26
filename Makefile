# Specify the default compiler
CXX = g++

# Specify the -fpic flag
CXXFLAGS += -fpic

# Additional LDFLAGS for znzlib library
ZNZLIB_LDFLAGS = -L${HOME}/FSL-cluster/znzlib -lfsl-znz

# Additional LDFLAGS for warpfns library
WARPFNS_LDFLAGS = -L${HOME}/FSL-cluster/warpfns  -L${HOME}/FSL-cluster/meshclass -L${HOME}/FSL-cluster/basisfield -L${HOME}/FSL-cluster/miscmaths -lfsl-warpfns -lfsl-meshclass -lfsl-basisfield -lfsl-miscmaths
# Define source files
SRCS = cluster.cc connectedcomp.cc infer.cc infertest.cc smoothest.cc 

# Define object files
OBJS = $(SRCS:.cc=.o)

# Define library source files and directories
LIB_DIRS = warpfns basisfield miscmaths newimage NewNifti utils cprob znzlib meshclass
LIB_SRCS = $(foreach dir,$(LIB_DIRS),$(wildcard $(dir)/*.cc))
LIB_OBJS = $(LIB_SRCS:.cc=.o)
 
# Define targets
all: smoothest fsl-cluster connectedcomp
 
# Compile the final executable
smoothest: libraries smoothest.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ smoothest.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS)  -lblas -llapack -lz

fsl-cluster: libraries cluster.o infer.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ cluster.o infer.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS) $(WARPFNS_LDFLAGS)  -lblas -llapack -lz

connectedcomp: libraries connectedcomp.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ connectedcomp.o $(LIB_OBJS) $(LDFLAGS) $(ZNZLIB_LDFLAGS)  -lblas -llapack -lz

# Rule to build object files
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile infertest.o
infertest.o: infertest.cc infer.o
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

# Phony target to build all libraries
.PHONY: libraries
libraries:
	@for dir in $(LIB_DIRS); do \
	$(MAKE) -C $$dir CXX=$(CXX) CXXFLAGS='$(CXXFLAGS)' LDFLAGS='$(LDFLAGS)'; \
	done

# Clean rule
clean:
	rm -f smoothest fsl-cluster connectedcomp $(OBJS) $(LIB_OBJS) $(shell find . -type f \( -name "*.o" -o -name "*.so" \))


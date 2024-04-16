# $Id$
#include ${FSLCONFDIR}/default.mk

PROJNAME = meshclass

XFILES=drawmesh selfintersection

LIBS=-lfsl-newimage -lfsl-miscmaths -lfsl-NewNifti -lfsl-cprob -lfsl-znz -lfsl-utils

MESHOBJS=point.o mpoint.o triangle.o mesh.o pt_special.o profile.o
DRAWOBJS=drawmesh.o mpoint.o triangle.o mesh.o point.o pt_special.o
INTEROBJS=selfintersection.o mpoint.o triangle.o mesh.o point.o pt_special.o

all: libfsl-meshclass.so ${XFILES}

libfsl-meshclass.so: ${MESHOBJS}
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(LDFLAGS)

drawmesh: ${DRAWOBJS}
	${CXX} ${CXXFLAGS} -o $@ $^ -L${HOME}/FSL-cluster/newimage -L${HOME}/FSL-cluster/miscmaths -L${HOME}/FSL-cluster/NewNifti \
        -L${HOME}/FSL-cluster/cprob -L${HOME}/FSL-cluster/znzlib -L${HOME}/FSL-cluster/utils -lfsl-newimage -lfsl-miscmaths -lfsl-NewNifti -lfsl-cprob -lfsl-znz -lfsl-utils -llapack -lblas -lz

selfintersection: ${INTEROBJS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

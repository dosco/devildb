COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = cc
OUTPUT_PATH = bin/
SOURCE_PATH = src/
EXE = $(OUTPUT_PATH)devildb

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  COPT = -O2
  CCMD = c++
	PROTOC = protoc
  OBJFLAG = -o
  EXEFLAG = -o
# INCLUDES = -I../.includes
  INCLUDES =
# LIBS = -lgc
  LIBS = -lboost_thread-mt -lboost_system-mt -ltcmalloc -lprotobuf
# LIBPATH = -L../gc/.libs
  CPPFLAGS = $(COPT) -g $(INCLUDES)
  LDFLAGS = $(LIBPATH) -g $(LIBS)
# LDFLAGS = $(LIBPATH) -g $(LIBS)
  DEP = dep
else
  OBJ = obj
  COPT = /O2
  CCMD = cl
  OBJFLAG = /Fo
  EXEFLAG = /Fe
# INCLUDES = /I..\\.includes
  INCLUDES =
# LIBS = ..\\.libs\\libgc.lib
  LIBS =
  CPPFLAGS = $(COPT) /DEBUG $(INCLUDES)
  LDFLAGS = /DEBUG
endif

OBJS := $(patsubst %.$(C),%.$(OBJ),$(wildcard $(SOURCE_PATH)*.$(C)))
PB := $(wildcard $(SOURCE_PATH)*.proto)

protobuf: $(PB)
	@echo Compiling protobufs...
	$(PROTOC) --proto_path=$(SOURCE_PATH) --cpp_out=$(SOURCE_PATH) $<
	
%.$(OBJ) %.pb.o: %.$(C) %.pb.cc
	@echo Compiling $(basename $<)...
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $< $(OBJFLAG) $@

all: $(OBJS)
	@echo Linking...
	$(CCMD) $(LDFLAGS) $^ $(LIBS) $(EXEFLAG) $(EXE)

clean:
	rm -rf $(SOURCE_PATH)*.$(OBJ) $(SOURCE_PATH)*.pb.o $(EXE)


rebuild: clean all
#rebuild is not entirely correct

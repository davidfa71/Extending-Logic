_OBJ=parser.o mytime.o varInfo.o ifInfo.o configInfo.o choiceInfo.o synExp.o  symbolTable.o humanNums.o eval.o MenuBlock.o maxOccSorter.o Scorer.o PermScorer.o SiftScorer.o RandomScorer.o forceScorer.o BlockScorer.o forceBlockScorer.o OtherScorer.o MultiComponents.o OneComponent.o DynamicScorer.o

OBJ = $(patsubst %,../obj/%,$(_OBJ))
DEP = $(OBJ:.o=.d)  # one dependency file for each source


INCLUDEFLAGS=-I/opt/local/lib -I../tools/src -Isrc -I/opt/local/include -I/usr/local/include 

LDFLAGS=-lcudd -L/usr/local/lib -L/opt/local/lib

ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
       CXXFLAGS=-std=c++11 -O3 -g -I. -I/usr/local/include  
       LDFLAGS += -lpthread
    endif
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS=-fcolor-diagnostics -O3 -Wno-deprecated-register -std=c++11 -stdlib=libc++ -g -mtune=native
	LDFLAGS += -Wl,-stack_size,0x40000000,-stack_addr,0xf0000000
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64:wq
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

CC=c++

../bin/Kconfig2Logic: ${OBJ} ../obj/driver.o ../obj/Kconfig2Logic.o ../obj/scanner.o ../obj/tokens.hash.o
	@echo \\t"[CXX]  " \\t$@
	@$(CC) $(CXXFLAGS) -o $@ $^ $(INCLUDEFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	-@rm -f ${OBJ} ${DEP} ../bin/Kconfig2Logic ../obj/Kconfig2Logic.o ../obj/driver.o ../obj/scanner.o ../obj/tokens.hash.o src/tokens.hash.cpp src/parser.hpp src/parser.cpp src/scanner.cpp src/location.hh src/stack.hh src/position.hh src/parser.output 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif


# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
../obj/%.d: src/%.cpp
	-@$(CPP) $(CFLAGS) $(INCLUDEFLAGS) $< -MM -MT $(@:.d=.o) >$@ >/dev/null 2>/dev/null


src/parser.cpp: src/parser.ypp src/scanner.cpp
	@echo \\t[BISON] \\t$@ 
	@bison -Wno-conflicts-sr -v -o $@ -t -b $*  $< 

src/scanner.cpp: src/scanner.lpp src/tokens.hash.hpp src/scanner.hpp src/tokens.hash.cpp
	@echo \\t"[FLEX] " \\t$@
	@flex --c++ -L  -o $@ $<

../obj/%.o: src/%.cpp src/%.hpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<


../obj/%.o: ../tools/src/%.cpp ../tools/src/%.hpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<

../obj/%.o: src/%.cpp
	@echo \\t"[CXX]  " \\t$@
	@$(CC) -c $(CXXFLAGS) $(INCLUDEFLAGS) -o $@ $<

src/tokens.hash.cpp: src/tokens.gperf
	@echo \\t"[GPERF]" \\t$@
	@gperf -a -C -E -g -k '1,3,$$' -P <$< >$@

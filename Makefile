EXE = server

CFLAGS = -Wall -pedantic -std=c11
LDFLAGS = -lm

BUILD_MARKER = build/$(TARGET).build
DEP = build/
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.d

CC = gcc

CFLAGS += -ggdb -pipe
LDFLAGS += -ggdb

SRCFILES = main.c status.c routes.c algo.c

OBJFILES = $(addprefix build/, $(patsubst %.c, %.o, $(SRCFILES)))

$(EXE): $(OBJFILES)
	$(CC) $^ -o $@ $(LDFLAGS)

build/%.o: src/%.c Makefile $(BUILD_MARKER) | build
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir build

$(BUILD_MARKER): | build
	rm -fv build/*.build
	touch $(BUILD_MARKER)

.PHONY: full
full: clean $(EXE)

.PHONY: clean
clean:
	rm -fv $(OBJFILES)
	rm -frv build
	rm -fv $(EXE)*

DEPFILES := $(OBJFILES:%.o=%.d)
include $(wildcard $(DEPFILES))


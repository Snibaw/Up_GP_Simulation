CC = gcc
CFLAGS := -g -Wall -Wextra

OBJ_EXE := Projet.o

LDFLAGS := -g

EXECUTABLE = _Projet

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_EXE)
	$(CC) $(OBJ_EXE) -o $@ $(LDFLAGS)

OBJ_ALL := Projet.o

$(OBJ_ALL): %.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

Projet.o: Projet.h

.PHONY: all clean update cancel_generation run debug

clean:
	rm -f $(EXECUTABLE) $(OBJ_ALL)

run: $(EXECUTABLE)
	./$<

debug: $(EXECUTABLE)
	gdb $<

update:
	bash Projet.c -f "$(CFLAGS)" -b "$(EXECUTABLE)" -c "$(CC)" -l "$(LDFLAGS)"

cancel_generation:
	mv Makefile~ Makefile

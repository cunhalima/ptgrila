#===========================================================================
#
# PTGrila
# Copyright (C) 2013 Alex Reimann Cunha Lima, Andrey Vinicius Fagundes,
# Gustavo Henrique Knob, Michel Costa, Ramon Perondi
#
# This file is part of the PTGrila
#
# PTGrila is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# PTGrila is distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MDB Modular Database System. If not, see
# <http://www.gnu.org/licenses/>.
#
#===========================================================================
BDIR=bin
ODIR=obj
SDIR=src
CC=gcc
CFLAGS=-O2 -Wall -Wextra -g
CFLAGS+=-Wno-unused-parameter -Wno-unused-variable
CFLAGS+=-Wno-unused-but-set-variable
CFLAGS+=-Wno-unused-function
CFLAGS+=-Iinc
LDFLAGS=-g
_MAINTEST=main.o
_MODULES=nada.o
MAINTEST=$(patsubst %, $(ODIR)/%, $(_MAINTEST))
MODULES=$(patsubst %, $(ODIR)/%, $(_MODULES))
OBJS=$(MAINTEST) $(MODULES)
EXE=$(BDIR)/ptg
TARBALL=trabalho.tar.gz

.PHONY: all clean run val tar test

all: $(EXE)

clean:
	@rm -f $(EXE) $(OBJS)

$(EXE): $(MAINTEST) $(MODULES)
	@$(CC) $^ $(LDFLAGS) -o $@

$(ODIR)/%.o: $(SDIR)/%.c
	@$(CC) -c $< $(CFLAGS) -o $@

$(EXE): | $(BDIR)

$(BDIR):
	@mkdir -p $(BDIR)

$(OBJS): | $(ODIR)

$(ODIR):
	@mkdir -p $(ODIR)

#.c.o:
#	@$(CC) -c $< $(CFLAGS) -o $@
#
run: $(EXE)
	@./$(EXE)

val: $(EXE)
	@valgrind --tool=memcheck ./$(EXE)

tar: $(TARBALL)

$(TARBALL):
	tar -cvzf $@ README COPYING src doc \
	Makefile

test: $(EXE)
	@./$(EXE) < data/infile

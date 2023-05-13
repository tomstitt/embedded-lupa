python.Iflags  = -I$(shell $(python) -c "from sysconfig import get_config_var; print(get_config_var('INCLUDEPY'))")
python.libdir  = $(shell $(python) -c "from sysconfig import get_config_var; print(get_config_var('LIBDIR'))")
python.version = $(shell $(python) -c "from sysconfig import get_config_var; print(get_config_var('VERSION'))")
python.abiflag = $(shell $(python) -c "import sys; print(getattr(sys,'abiflags',''))")
python.Lflags  = -L$(python.libdir) -Wl,-rpath $(python.libdir)
python.lflags  = -lpython$(python.version)$(python.abiflag)
python.root    = $(dir $(python))
cython         = $(python.root)cython

lua.Iflags = -I$(lua.include)
lua.Lflags = -L$(lua.lib)
lua.lflags = -llua

packages = python lua

Iflags = $(foreach pkg,$(packages),$($(pkg).Iflags))
Lflags = $(foreach pkg,$(packages),$($(pkg).Lflags))
lflags = $(foreach pkg,$(packages),$($(pkg).lflags)) -ldl -lm

%.o: %.c
	$(CC) -g -O0 $(Iflags) -c $^ -o $@

.PHONY: info clean

all: liblupa.a

liblupa.a: _lupa.o
	ar -rc $@ $^

_lupa.c: lupa/lupa/_lupa.pyx
	$(cython) $(cython.opts) $< -o $@

example: liblupa.a example.c
	$(CC) -g -O0 example.c $(Iflags) $(Lflags) $(lflags) $< -o $@

info:
	@echo "python:      $(python)"
	@echo "python.root: $(python.root)"
	@echo "cython:      $(cython)"
	@echo "py.Iflags:   $(python.Iflags)"
	@echo "py.Lflags:   $(python.Lflags)"
	@echo "py.lflags:   $(python.lflags)"

clean:
	rm -f _lupa.* liblupa.a example

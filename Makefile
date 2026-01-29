#
# Copyright (c) 2026, Ian Moffett.
# Provided under the BSD-3 clause.
#

include mk/default.mk

CFILES = $(shell find . -name "*.c" | grep -v arch)
CFILES += src/arch/$(TARGET).c
DFILES = $(CFILES:.c=.d)
OFILES = $(CFILES:.c=.o)

.PHONY: all
all: $(OFILES)
	$(CC) $(OFILES) -o gup

-include $(DFILES)
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(DFILES) $(OFILES)

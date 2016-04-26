EXAMPLES = shm mm pipe fifo

TARGET = $(EXAMPLES:%=%_example)
CC = gcc
LD = gcc
CFLAGS = -g
LDFLAGS =

all: $(TARGET)
	@echo
	@echo "*** Run the following examples ***"
	@for example in $(TARGET); do \
		echo "  $$example"; \
	done
	@echo

%_example: %_example.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o *_temp

.PHONY: all clean fifo_extra
.SECONDARY: $(TARGET:%=%.o)

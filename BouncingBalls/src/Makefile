EXECUTABLE = app
CC = gcc

CFLAGS = -Wall -Wextra -std=gnu99 -D_REENTRANT -DSDL_MAIN_HANDLED 
LIBS = -lm -L. -lSDL2


# OS X-specific nonsense
ifeq ($(shell uname -s),Darwin)
	BREWPATH = $(shell brew --prefix)
	CFLAGS += -I$(BREWPATH)/include
	LIBS += -L$(BREWPATH)/lib
endif

SOURCE = main.c triangle.c drawline.c object.c list.c
HEADER = drawline.h triangle.h object.h list.h teapot_data.h sphere_data.h

.PHONY: all
all: $(EXECUTABLE)
$(EXECUTABLE): $(SOURCE) $(HEADER)
	$(info === Compiling...)
	$(shell $(PRE_BUILD))
	$(CC) $(CFLAGS) -o $@ $(SOURCE) $(LIBS)
    
.PHONY: clean
clean:
	@rm -f $(EXECUTABLE) 
	$(info === Cleaned)


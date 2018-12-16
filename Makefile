BIN      := ftp
CC       := gcc
CFLAGS := -Wall -Werror -Wextra -O3
LDFLAGS  := -Wall -Werror -Wextra -O3
SRC      := $(wildcard *.c)
HEADERS  := $(wildcard *.h)
OBJ      := $(SRC:.c=.o)
DEPS     := $(SRC:.c=.d)

$(BIN): $(OBJ)

%.d: %.c
	$(CC) $(CFLAGS) -MF"$@" -MG -MM -MP -MT"$@" -MT"$(<:.c=.o)" "$<"

-include $(DEPS)

.PHONY: format
format:
	clang-format -i $(SRC) $(HEADERS)

.PHONY: run
run:
	@./$(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJ) $(DEPS)

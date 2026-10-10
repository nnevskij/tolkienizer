CC ?= clang
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
INCLUDES = -Iinclude

# Detect cJSON flags via pkg-config, with fallback for Homebrew and standard paths
CJSON_CFLAGS ?= $(shell pkg-config --cflags libcjson 2>/dev/null || (test -d /opt/homebrew/include/cjson && echo "-I/opt/homebrew/include -I/opt/homebrew/include/cjson") || (test -d /usr/local/include/cjson && echo "-I/usr/local/include -I/usr/local/include/cjson") || echo "")
CJSON_LIBS ?= $(shell pkg-config --libs libcjson 2>/dev/null || (test -d /opt/homebrew/lib && echo "-L/opt/homebrew/lib -lcjson") || (test -d /usr/local/lib && echo "-L/usr/local/lib -lcjson") || echo "-lcjson")

# Detect curl flags via curl-config or pkg-config
CURL_CFLAGS ?= $(shell curl-config --cflags 2>/dev/null || pkg-config --cflags libcurl 2>/dev/null || echo "")
CURL_LIBS ?= $(shell curl-config --libs 2>/dev/null || pkg-config --libs libcurl 2>/dev/null || echo "-lcurl")

PTHREAD_LIBS = -lpthread

ALL_CFLAGS = $(CFLAGS) $(INCLUDES) $(CJSON_CFLAGS) $(CURL_CFLAGS)
ALL_LIBS = $(CJSON_LIBS) $(CURL_LIBS) $(PTHREAD_LIBS)

# Installation prefix:
# Defaults to /usr/local if root, or $(HOME)/.local if regular user
ifeq ($(shell id -u), 0)
    PREFIX ?= /usr/local
else
    PREFIX ?= $(HOME)/.local
endif

BINDIR ?= $(PREFIX)/bin

SRCS = src/main.c src/config.c src/prompts.c src/ai_client.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = tolkienizer

.PHONY: all clean install uninstall check-deps

all: check-deps $(TARGET)

check-deps:
	@which curl-config >/dev/null 2>&1 || which pkg-config >/dev/null 2>&1 || (echo "Warning: libcurl / pkg-config not found in standard paths.")

$(TARGET): $(OBJS)
	$(CC) $(ALL_CFLAGS) -o $@ $(OBJS) $(ALL_LIBS)
	@echo "✓ Build complete: $(TARGET)"

%.o: %.c
	$(CC) $(ALL_CFLAGS) -c $< -o $@

install: $(TARGET)
	@echo "Installing $(TARGET) to $(DESTDIR)$(BINDIR)..."
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	@echo "✓ $(TARGET) installed successfully to $(DESTDIR)$(BINDIR)/$(TARGET)"
	@echo "You can now run '$(TARGET)' directly from your terminal!"

uninstall:
	@echo "Removing $(TARGET) from $(DESTDIR)$(BINDIR)..."
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
	@echo "✓ $(TARGET) uninstalled."

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ Clean complete."

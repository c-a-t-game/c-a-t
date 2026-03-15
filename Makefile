ASSETS := $(shell find assets -type f)

src/core/assets/data.h: $(ASSETS)
	./assetgen.sh $^ > $@

CORE_SOURCES := $(shell find src/core/assets src/core/engine src/core/io src/core/jitc src/core -maxdepth 1 -name "*.c")

RUNTIME_TARGET := cat
RUNTIME_SOURCES := $(shell find src/runtime -name "*.c")

LIBS := glew sdl3
PKGCONF_FLAGS :=
CFLAGS := -g -Isrc -Iinclude -I.
LDFLAGS := -g -lm

ifeq ($(OS),Windows_NT)
	CORE_SOURCES += src/core/platform/windows.c
	CFLAGS += "-Dexport=__declspec(dllexport)"
else ifeq ($(shell uname -s),Linux)
	CORE_SOURCES += src/core/platform/linux.c
	CFLAGS += -Dexport
	LDFLAGS += -rdynamic
else
	$(error Unsupposed operating system)
endif

CFLAGS += $(shell pkgconf --cflags $(PKGCONF_FLAGS) $(LIBS))
LDFLAGS += $(shell pkgconf --libs $(PKGCONF_FLAGS) $(LIBS))

CORE_OBJECTS := $(patsubst src/%.c,bin/%.o,$(CORE_SOURCES))
RUNTIME_OBJECTS := $(CORE_OBJECTS) $(patsubst src/%.c,bin/%.o,$(RUNTIME_SOURCES))

bin/%.o: src/%.c
	@mkdir -p $(dir $@)
	gcc -c $< $(CFLAGS) -o $@

$(RUNTIME_TARGET): $(RUNTIME_OBJECTS)
	gcc $^ $(LDFLAGS) -o $@

all: $(RUNTIME_TARGET)

clean:
	rm -rf bin

bin/%.d: src/%.c
	@mkdir -p $(dir $@)
	@gcc $(CFLAGS) -MM -MT $(@:.d=.o) $< -o $@

-include $(CORE_OBJECTS:%.o=%.d)
-include $(RUNTIME_OBJECTS:%.o=%.d)
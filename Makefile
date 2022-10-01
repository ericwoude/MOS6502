.PHONY: build

default: all

all: format build

lint:
	@find src/ include/ tests/ -type f \( -iname "*.h" -or -iname "*.cpp" \) | xargs clang-format -i -n -Werror

format:
	@find src/ include/ tests/ -type f \( -iname "*.h" -or -iname "*.cpp" \) | xargs clang-format -i

build:
	mkdir -p build
	cmake -B build
	cmake --build build

test:
	./bin/instruction_tests
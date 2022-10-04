
default:
	clang parser.c -Wall -o parser

build-wasm:
	emcc parser.c -o parser.wasm


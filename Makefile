
default:
	clang parser.c -Wall -o parser

wasm:
	emcc -O3 -sEXPORTED_RUNTIME_METHODS='["ccall", "allocateUTF8", "UTF8ToString"]' -sEXPORTED_FUNCTIONS='["_malloc", "_free"]' -sASSERTIONS parser_wasm.c

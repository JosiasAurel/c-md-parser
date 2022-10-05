# C Markdown Parser

This is a C program that can convert basic markdown text into a HTML document and rendered in the browser.
Markdown support here is very limited. The flavour of the markdown is described below.

A sample input and output is provided in along with the code.

## TODO

A list of interesting things I want to do with this code.

- [x] Write a WASM module out of the transpiler
- [x] Build a web application that will use the WASM module for rendering markdown

## How to use

Build the CLI tool by running `make` in your terminal.
In order to transform a markdown file into HTML, run 

```shell
./parser input.md output.html
```
> Not there is an issue when dealing with three backticks (```) - the first two will be converted into an opening and closing emphasis tag (`<em></em>`)

For the web based application, run `make wasm`. This requires you to have emscripten installed and setup correctly on your system.
The HTML file required for this demo is already written. You may use it by running `python3 -m http.server` and going to the address.
You should see an editor on the screen where you can type in markdown text.

This code is probably not very extensible and you may avoid touching it for your own mental sanity. Touch it, if you're courageous enough (this goes to myself, too).
It might need a lot of rework.

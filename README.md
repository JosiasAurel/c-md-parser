
# C Markdown Parser

This is a C program that can convert basic markdown text into a HTML document and rendered in the browser.
Markdown support here is very limited. The flavour of the markdown is described below.

```
# H1 -> <h1>H1</h1>
## H2 -> <h2>H1</h2>
### H3 -> <h3>H3</h3>
### H4 -> <h4>H4</h4>
*bold* -> <b>bold</b>
_italic_ -> <i>italic</i>
~cancelled~ -> <strike>cancelled</strike>
- List item -> <li>List Item</li>
```

## TODO

A list of interesting things I want to do with this code.

- [ ] Write a WASM module out of the transpiler
- [ ] Build a web application that will use the WASM module for rendering markdown

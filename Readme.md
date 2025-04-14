# HTML Generation Library in C

## Overview

This HTML Generation Library is a comprehensive C library that allows developers to programmatically create, manipulate, and render HTML documents with proper structure and tag placement. It provides DOM-like functionality where elements can be accessed and manipulated by their ID attribute, ensuring proper HTML document structure and support for nested elements.

## Features

- **DOM-like structure**: Elements can be accessed and manipulated by their ID
- **Proper HTML document structure**: Automatic placement of tags in their correct locations
- **Nested elements support**: Create complex HTML hierarchies with proper parent-child relationships
- **Memory management**: Built-in safeguards to prevent memory leaks
- **Error handling**: Graceful handling of improper usage with clear error messages
- **Pretty printing**: Generated HTML is properly indented for readability

## Installation

Clone the repository and build the library using the provided Makefile:

```bash
git clone https://github.com/harshitsaini17/ic-project/tree/html
cd ic-project
make
```

To install the library system-wide:

```bash
make install
```

## Project Structure

```
htmlgen/
├── src/
│   ├── html_context.c
│   ├── html_elements.c
│   ├── html_gen.c
│   ├── html_utils.c
├── HTML.h
├── examples/
│   ├── simple_page.c
│   ├── complex_page.c
├── Makefile
└── README.md
```

## Usage

### Basic Example

```c
#include "HTML.h"

int main() {
    // Initialize HTML context with output file
    html_context* ctx = html_init_file("output.html", "My Page");
    
    // Add style to the head section
    html_add_style(ctx, "body { font-family: Arial; }");
    
    // Add elements to the body
    html_add_div(ctx, "id='container' class='main'", "");
    
    // Begin a section (creates a div and sets it as current)
    html_begin_section(ctx, "id='main'");
    
    // Add elements to the current section
    html_add_heading(ctx, 1, "Welcome to my page", "class='title'");
    html_add_paragraph(ctx, "id='intro'", "This is a test page generated with C.");
    
    // End the section (returns to parent element)
    html_end_section(ctx);
    
    // Render and finalize
    html_finalize(ctx);
    
    return 0;
}
```

### Accessing Elements by ID

```c
// Get element by ID
html_element* intro = html_get_element_by_id(ctx, "intro");

// Modify the element
html_set_element_content(intro, "Updated introduction text");
html_add_class(intro, "highlighted");
```

## API Reference

### Initialization and Finalization

- `html_context* html_init_file(const char* filename, const char* title)`: Initialize an HTML context with file output
- `void html_finalize(html_context* ctx)`: Free all resources used by the HTML context

### Element Creation

- `int html_add_div(html_context* ctx, const char* attributes, const char* content)`: Add a div element
- `int html_add_paragraph(html_context* ctx, const char* attributes, const char* content)`: Add a paragraph element
- `int html_add_heading(html_context* ctx, int level, const char* content, const char* attributes)`: Add a heading element (h1-h6)
- `int html_add_image(html_context* ctx, const char* src, const char* alt, const char* attributes)`: Add an image element
- `int html_add_anchor(html_context* ctx, const char* href, const char* content, const char* attributes)`: Add an anchor (a) element

### Section Management

- `int html_begin_section(html_context* ctx, const char* attributes)`: Begin a section (creates a div and sets it as current)
- `int html_end_section(html_context* ctx)`: End a section (returns to parent element)

### List Management

- `int html_begin_unordered_list(html_context* ctx, const char* attributes)`: Begin an unordered list
- `int html_begin_ordered_list(html_context* ctx, const char* attributes)`: Begin an ordered list
- `int html_add_list_item(html_context* ctx, const char* content, const char* attributes)`: Add a list item
- `int html_end_list(html_context* ctx)`: End a list

### Table Management

- `int html_begin_table(html_context* ctx, const char* attributes)`: Add a table element
- `int html_begin_table_row(html_context* ctx, const char* attributes)`: Add a table row element
- `int html_add_table_cell(html_context* ctx, const char* content, const char* attributes, int is_header)`: Add a table cell element
- `int html_end_table_row(html_context* ctx)`: End a table row element
- `int html_end_table(html_context* ctx)`: End a table element

### Form Elements

- `int html_add_form(html_context* ctx, const char* action, const char* method, const char* attributes)`: Add a form element
- `int html_add_input(html_context* ctx, const char* type, const char* name, const char* value, const char* attributes)`: Add an input element
- `int html_add_button(html_context* ctx, const char* type, const char* content, const char* attributes)`: Add a button element

### Head Section Elements

- `int html_add_style(html_context* ctx, const char* style_content)`: Add a style element to the head section
- `int html_add_script(html_context* ctx, const char* script_content, int is_external)`: Add a script element to the head section
- `int html_add_meta(html_context* ctx, const char* name, const char* content)`: Add a meta tag to the head section
- `int html_add_link(html_context* ctx, const char* rel, const char* href, const char* type)`: Add a link tag to the head section

### Element Manipulation

- `html_element* html_get_element_by_id(html_context* ctx, const char* id)`: Get an element by its ID
- `int html_set_element_content(html_element* element, const char* content)`: Set the content of an element
- `int html_set_element_attribute(html_element* element, const char* name, const char* value)`: Set an attribute on an element
- `int html_add_class(html_element* element, const char* classname)`: Add a class to an element

### Generic Tag Management

- `int html_begin_tag(html_context* ctx, const char* tagname, const char* attributes)`: Begin a specific tag and set it as current
- `int html_end_tag(html_context* ctx)`: End the current tag (returns to parent element)

## Error Handling

Most functions return an integer status code (0 for success, non-zero for failure). When an error occurs, you can retrieve the error message using:

```c
const char* error = html_get_error();
printf("Error: %s\n", error);
```

## Memory Management

The library handles memory management internally. All resources are freed when calling `html_finalize()`. However, if you need to free individual elements, you can use:

```c
void html_free_element(html_element* element);
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

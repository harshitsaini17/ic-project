/**
 * @file HTML.h
 * @brief HTML Generation Library Header
 *
 * This library provides functionality to generate HTML documents programmatically in C,
 * maintaining proper HTML structure and tag placement.
 */

 #ifndef HTML_H
 #define HTML_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * @brief Structure representing an HTML element
  */
 typedef struct html_element {
     char* id;                      /* ID attribute of the element */
     char* tagname;                 /* Name of the HTML tag (e.g., div, p) */
     char* content;                 /* Inner content of the element */
     struct html_element* parent;   /* Pointer to the parent element */
     struct html_element** children; /* Array of pointers to child elements */
     int children_count;            /* Number of child elements */
     int children_capacity;         /* Capacity of children array */
     char* attributes;              /* Additional attributes as string */
 } html_element;
 
 /**
  * @brief Structure for ID-to-element mapping (hash map)
  */
 typedef struct {
     char** keys;                  /* Array of ID keys */
     html_element** values;        /* Array of pointers to elements with IDs */
     int capacity;                 /* Capacity of the hash map */
     int size;                     /* Current size of the hash map */
 } id_map;
 
 /**
  * @brief Structure representing the HTML document context
  */
 typedef struct html_context {
     html_element* root;           /* Root element of the document */
     html_element* current;        /* Current element being worked on */
     id_map* element_map;          /* Hash map for ID lookups */
     FILE* output_file;            /* File pointer for output file */
     char* title;                  /* Title of the HTML document */
     int indent_level;             /* Current indentation level */
 } html_context;
 
 /* ===== String and Memory Utilities ===== */
 
 /**
  * @brief Safe string duplication
  */
 char* html_strdup(const char* str);
 
 /**
  * @brief Create a new attribute string with the given attribute added
  */
 char* html_add_attribute(const char* attributes, const char* name, const char* value);
 
 /**
  * @brief Extract attribute value from attributes string
  */
 char* html_extract_attribute(const char* attributes, const char* name);
 
 /**
  * @brief Extract ID from attributes string
  */
 char* html_extract_id(const char* attributes);
 
 /**
  * @brief Generate indentation string for pretty-printing
  */
 char* html_generate_indent(int level);
 
 /* ===== Error Handling ===== */
 
 /**
  * @brief Set error message
  */
 void html_set_error(const char* format, ...);
 
 /**
  * @brief Get last error message
  */
 const char* html_get_error(void);
 
 /**
  * @brief Get last error message (alias for html_get_error)
  */
 const char* html_get_last_error(void);
 
 /**
  * @brief Clear error message
  */
 void html_clear_error(void);
 
 /* ===== Hash Map Management ===== */
 
 /**
  * @brief Create a new ID map
  */
 id_map* html_create_id_map(int initial_capacity);
 
 /**
  * @brief Free all resources used by an ID map
  */
 void html_free_id_map(id_map* map);
 
 /**
  * @brief Resize the ID hash map when it reaches capacity
  */
 int html_resize_id_map(id_map* map);
 
 /**
  * @brief Calculate a simple hash for a string
  */
 unsigned int html_hash_string(const char* str);
 
 /* ===== Element Validation and Navigation ===== */
 
 /**
  * @brief Check if an element can be a child of another element
  */
 int html_is_valid_child(const char* parent_tag, const char* child_tag);
 
 /**
  * @brief Check if an element should have its content on a new line
  */
 int html_is_block_element(const char* tagname);
 
 /**
  * @brief Check if an element is self-closing
  */
 int html_is_self_closing(const char* tagname);
 
 /**
  * @brief Find the head element in the document
  */
 html_element* html_find_head(html_context* ctx);
 
 /**
  * @brief Find the body element in the document
  */
 html_element* html_find_body(html_context* ctx);
 
 /**
  * @brief Navigate to the body element
  */
 int html_navigate_to_body(html_context* ctx);
 
 /**
  * @brief Navigate to the head element
  */
 int html_navigate_to_head(html_context* ctx);
 
 /* ===== Document Structure ===== */
 
 /**
  * @brief Create the basic HTML document structure
  */
 int html_create_document_structure(html_context* ctx);
 
 /**
  * @brief Render an HTML element and its children to the output file
  */
 int html_render_element(html_context* ctx, html_element* element);
 
 /* ===== Core API Functions ===== */
 
 /**
  * @brief Initialize an HTML context with file output
  */
 html_context* html_init_file(const char* filename, const char* title);
 
 /**
  * @brief Free all resources used by the HTML context
  */
 void html_finalize(html_context* ctx);
 
 /**
  * @brief Add a style element to the head section
  */
 int html_add_style(html_context* ctx, const char* style_content);
 
 /**
  * @brief Add a script element to the head section
  */
 int html_add_script(html_context* ctx, const char* script_content, int is_external);
 
 /**
  * @brief Add a div element
  */
 int html_add_div(html_context* ctx, const char* attributes, const char* content);
 
 /**
  * @brief Add a paragraph element
  */
 int html_add_paragraph(html_context* ctx, const char* attributes, const char* content);
 
 /**
  * @brief Add a heading element (h1-h6)
  * 
  * Note: Updated signature to match implementation
  */
 int html_add_heading(html_context* ctx, int level, const char* content, const char* attributes);
 
 /**
  * @brief Begin a section (creates an element and sets it as current)
  */
 int html_begin_section(html_context* ctx, const char* attributes);
 
 /**
  * @brief End a section (returns to parent element as current)
  */
 int html_end_section(html_context* ctx);
 
 /**
  * @brief Add an image element
  */
 int html_add_image(html_context* ctx, const char* src, const char* alt, const char* attributes);
 
 /**
  * @brief Add an anchor (a) element
  */
 int html_add_anchor(html_context* ctx, const char* href, const char* content, const char* attributes);
 
 /**
  * @brief Begin an unordered list
  */
 int html_begin_unordered_list(html_context* ctx, const char* attributes);
 
 /**
  * @brief Begin an ordered list
  */
 int html_begin_ordered_list(html_context* ctx, const char* attributes);
 
 /**
  * @brief End a list
  */
 int html_end_list(html_context* ctx);
 
 /**
  * @brief Add a list item
  */
 int html_add_list_item(html_context* ctx, const char* content, const char* attributes);
 
 /**
  * @brief Get an element by its ID
  */
 html_element* html_get_element_by_id(html_context* ctx, const char* id);
 
 /**
  * @brief Set the content of an element
  */
 int html_set_element_content(html_element* element, const char* content);
 
 /**
  * @brief Set an attribute on an element
  */
 int html_set_element_attribute(html_element* element, const char* name, const char* value);
 
 /**
  * @brief Add a class to an element
  */
 int html_add_class(html_element* element, const char* classname);
 
 /**
  * @brief Add a form element
  */
 int html_add_form(html_context* ctx, const char* action, const char* method, const char* attributes);
 
 /**
  * @brief Add an input element
  */
 int html_add_input(html_context* ctx, const char* type, const char* name, const char* value, const char* attributes);
 
 /**
  * @brief Add a button element
  */
 int html_add_button(html_context* ctx, const char* type, const char* content, const char* attributes);
 
 /**
  * @brief Add a table element
  */
 int html_begin_table(html_context* ctx, const char* attributes);
 
 /**
  * @brief End a table element
  */
 int html_end_table(html_context* ctx);
 
 /**
  * @brief Add a table row element
  */
 int html_begin_table_row(html_context* ctx, const char* attributes);
 
 /**
  * @brief End a table row element
  */
 int html_end_table_row(html_context* ctx);
 
 /**
  * @brief Add a table cell element
  */
 int html_add_table_cell(html_context* ctx, const char* content, const char* attributes, int is_header);
 
 /**
  * @brief Add a meta tag to the head section
  */
 int html_add_meta(html_context* ctx, const char* name, const char* content);
 
 /**
  * @brief Add a link tag to the head section
  */
 int html_add_link(html_context* ctx, const char* rel, const char* href, const char* type);
 
 /**
  * @brief Register an element by ID in the hash map
  */
 int html_register_element_by_id(html_context* ctx, html_element* element);
 
 /**
  * @brief Add a child element to a parent element
  */
 html_element* html_add_child(html_context* ctx, html_element* parent, const char* tagname, const char* attributes, const char* content);
 
 /**
  * @brief Render the HTML document to the output file
  */
 int html_render(html_context* ctx);
 
 /**
  * @brief Create a new HTML element
  */
 html_element* html_create_element(const char* tagname, const char* attributes, const char* content);
 
 /**
  * @brief Free all resources used by an HTML element
  */
 void html_free_element(html_element* element);
 
 /**
  * @brief Set the current element in the context
  */
 int html_set_current_element(html_context* ctx, html_element* element);
 
 /**
  * @brief Add content to the current element
  */
 int html_add_content(html_context* ctx, const char* content);
 

 /**
 * @brief Begin a specific tag and set it as current
 * 
 * @param ctx The HTML context
 * @param tagname The name of the tag to begin
 * @param attributes The attributes string
 * @return int 0 on success, non-zero on failure
 */
int html_begin_tag(html_context* ctx, const char* tagname, const char* attributes);

/**
 * @brief End the current tag (returns to parent element as current)
 * 
 * @param ctx The HTML context
 * @return int 0 on success, non-zero on failure
 */
int html_end_tag(html_context* ctx);
 
 
 #endif /* HTML_H */
 
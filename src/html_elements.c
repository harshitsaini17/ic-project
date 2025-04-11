/**
 * @file html_elements.c
 * @brief Implementation of HTML element creation and manipulation functions
 *
 * This file contains functions for creating and manipulating HTML elements,
 * including specialized elements like divs, paragraphs, headings, and more.
 * It handles proper element creation, attribute management, and hierarchy.
 */

 #include "HTML.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 #include <ctype.h>
 
 /* ===== Element Creation and Management ===== */
 
 /**
  * @brief Create a new HTML element
  *
  * @param tagname The tag name
  * @param attributes The attributes string
  * @param content The inner content
  * @return html_element* The newly created element, NULL on failure
  */
 html_element* html_create_element(const char* tagname, const char* attributes, const char* content) {
     if (!tagname) return NULL;
     
     html_element* element = (html_element*)malloc(sizeof(html_element));
     if (!element) {
         html_set_error("Memory allocation failed for HTML element");
         return NULL;
     }
     
     // Initialize all fields to NULL/0
     memset(element, 0, sizeof(html_element));
     
     // Set the tag name
     element->tagname = html_strdup(tagname);
     if (!element->tagname) {
         free(element);
         return NULL;
     }
     
     // Set content if provided
     if (content) {
         element->content = html_strdup(content);
         if (!element->content) {
             free(element->tagname);
             free(element);
             return NULL;
         }
     }
     
     // Set attributes if provided
     if (attributes) {
         element->attributes = html_strdup(attributes);
         if (!element->attributes) {
             free(element->content);
             free(element->tagname);
             free(element);
             return NULL;
         }
         
         // Extract ID if present in attributes
         element->id = html_extract_id(attributes);
     }
     
     // Initialize children array
     element->children_capacity = 4; // Start with small capacity
     element->children = (html_element**)malloc(element->children_capacity * sizeof(html_element*));
     if (!element->children) {
         free(element->id);
         free(element->attributes);
         free(element->content);
         free(element->tagname);
         free(element);
         html_set_error("Memory allocation failed for children array");
         return NULL;
     }
     
     // Initialize children array with nulls
     memset(element->children, 0, element->children_capacity * sizeof(html_element*));
     
     return element;
 }
 
 /**
  * @brief Add a child element to a parent element
  *
  * @param ctx The HTML context
  * @param parent The parent element
  * @param tagname The tag name of the new element
  * @param attributes The attributes string
  * @param content The inner content
  * @return html_element* The newly created element, NULL on failure
  */
 html_element* html_add_child(html_context* ctx, html_element* parent, const char* tagname, const char* attributes, const char* content) {
     if (!ctx || !parent || !tagname) return NULL;
     
     // Check if the child tag is valid for the parent tag
     if (!html_is_valid_child(parent->tagname, tagname)) {
         html_set_error("Invalid child tag '%s' for parent '%s'", tagname, parent->tagname);
         return NULL;
     }
     
     // Create the new element
     html_element* child = html_create_element(tagname, attributes, content);
     if (!child) return NULL;
     
     // Set the parent reference
     child->parent = parent;
     
     // Check if we need to resize the children array
     if (parent->children_count >= parent->children_capacity) {
         int new_capacity = parent->children_capacity * 2;
         html_element** new_children = (html_element**)realloc(parent->children, 
                                       new_capacity * sizeof(html_element*));
         
         if (!new_children) {
             html_free_element(child);
             html_set_error("Memory allocation failed for resizing children array");
             return NULL;
         }
         
         // Initialize new slots with NULL
         for (int i = parent->children_capacity; i < new_capacity; i++) {
             new_children[i] = NULL;
         }
         
         parent->children = new_children;
         parent->children_capacity = new_capacity;
     }
     
     // Add the child to the parent's children array
     parent->children[parent->children_count++] = child;
     
     // Register the element by ID if it has one
     if (child->id) {
         html_register_element_by_id(ctx, child);
     }
     
     return child;
 }
 
 /**
  * @brief Free all resources used by an HTML element and its children
  *
  * @param element The element to free
  */
 void html_free_element(html_element* element) {
     if (!element) return;
     
     // Free all children recursively
     for (int i = 0; i < element->children_count; i++) {
         html_free_element(element->children[i]);
     }
     
     // Free the children array
     free(element->children);
     
     // Free all strings
     free(element->id);
     free(element->tagname);
     free(element->content);
     free(element->attributes);
     
     // Free the element itself
     free(element);
 }
 
 /**
  * @brief Set the content of an element
  *
  * @param element The element to modify
  * @param content The new content
  * @return int 0 on success, non-zero on failure
  */
 int html_set_element_content(html_element* element, const char* content) {
     if (!element) return -1;
     
     // Free existing content
     free(element->content);
     
     // Set new content if provided, otherwise NULL
     if (content) {
         element->content = html_strdup(content);
         if (!element->content) {
             html_set_error("Memory allocation failed for element content");
             return -1;
         }
     } else {
         element->content = NULL;
     }
     
     return 0;
 }
 
 /**
  * @brief Set an attribute on an element
  *
  * @param element The element to modify
  * @param name The attribute name
  * @param value The attribute value
  * @return int 0 on success, non-zero on failure
  */
 int html_set_element_attribute(html_element* element, const char* name, const char* value) {
     if (!element || !name || !value) return -1;
     
     // Create new attributes string with the attribute added/updated
     char* new_attributes = html_add_attribute(element->attributes, name, value);
     if (!new_attributes) {
         return -1;
     }
     
     // Free old attributes string
     free(element->attributes);
     
     // Update attributes
     element->attributes = new_attributes;
     
     // If the ID attribute was set, update the element's ID
     if (strcmp(name, "id") == 0) {
         free(element->id);
         element->id = html_strdup(value);
         if (!element->id) {
             html_set_error("Memory allocation failed for element ID");
             return -1;
         }
     }
     
     return 0;
 }
 
 /**
  * @brief Add a class to an element
  *
  * @param element The element to modify
  * @param classname The class to add
  * @return int 0 on success, non-zero on failure
  */
 int html_add_class(html_element* element, const char* classname) {
     if (!element || !classname) return -1;
     
     // Extract current class attribute if it exists
     char* current_class = html_extract_attribute(element->attributes, "class");
     
     char* new_class = NULL;
     if (current_class) {
         // Check if the class already exists in the list
         char* class_check = strstr(current_class, classname);
         int classname_len = strlen(classname);
         
         // Only consider it a match if it's a whole word (surrounded by spaces or end/start of string)
         if (class_check && 
             (class_check == current_class || isspace((unsigned char)*(class_check-1))) &&
             (*(class_check + classname_len) == '\0' || isspace((unsigned char)*(class_check + classname_len)))) {
             // Class already exists, do nothing
             free(current_class);
             return 0;
         }
         
         // Append the new class
         size_t total_len = strlen(current_class) + strlen(classname) + 2; // +2 for space and null terminator
         new_class = (char*)malloc(total_len);
         if (!new_class) {
             free(current_class);
             html_set_error("Memory allocation failed for class attribute");
             return -1;
         }
         
         sprintf(new_class, "%s %s", current_class, classname);
         free(current_class);
     } else {
         // No existing class attribute, just use the new class
         new_class = html_strdup(classname);
         if (!new_class) {
             html_set_error("Memory allocation failed for class attribute");
             return -1;
         }
     }
     
     // Set the class attribute
     int result = html_set_element_attribute(element, "class", new_class);
     free(new_class);
     
     return result;
 }
 
 /* ===== Specific Element Functions ===== */
 
 /**
  * @brief Add a div element
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @param content The inner content
  * @return int 0 on success, non-zero on failure
  */
 int html_add_div(html_context* ctx, const char* attributes, const char* content) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* div = html_add_child(ctx, ctx->current, "div", attributes, content);
     return div ? 0 : -1;
 }
 
 /**
  * @brief Add a paragraph element
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @param content The inner content
  * @return int 0 on success, non-zero on failure
  */
 int html_add_paragraph(html_context* ctx, const char* attributes, const char* content) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* p = html_add_child(ctx, ctx->current, "p", attributes, content);
     return p ? 0 : -1;
 }
 
 /**
  * @brief Add a heading element (h1-h6)
  *
  * @param ctx The HTML context
  * @param level The heading level (1-6)
  * @param content The inner content
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_add_heading(html_context* ctx, int level, const char* content, const char* attributes) {
     if (!ctx || !ctx->current || level < 1 || level > 6) return -1;
     
     // Create the tag name (h1, h2, etc.)
     char tagname[4] = "h";
     tagname[1] = '0' + level;
     tagname[2] = '\0';
     
     html_element* heading = html_add_child(ctx, ctx->current, tagname, attributes, content);
     return heading ? 0 : -1;
 }
 
 /**
  * @brief Begin a section (creates a div and sets it as current)
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_section(html_context* ctx, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* section = html_add_child(ctx, ctx->current, "div", attributes, NULL);
     if (!section) return -1;
     
     ctx->current = section;
     return 0;
 }
 
 /**
  * @brief End a section (returns to parent element as current)
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_section(html_context* ctx) {
     if (!ctx || !ctx->current || !ctx->current->parent) return -1;
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Add an image element
  *
  * @param ctx The HTML context
  * @param src The src attribute value
  * @param alt The alt attribute value
  * @param attributes Additional attributes
  * @return int 0 on success, non-zero on failure
  */
 int html_add_image(html_context* ctx, const char* src, const char* alt, const char* attributes) {
     if (!ctx || !ctx->current || !src) return -1;
     
     // Combine attributes with src and alt
     char* combined_attrs = NULL;
     
     // Start with src attribute
     char* temp = html_add_attribute(NULL, "src", src);
     if (!temp) return -1;
     combined_attrs = temp;
     
     // Add alt attribute if provided
     if (alt) {
         temp = html_add_attribute(combined_attrs, "alt", alt);
         free(combined_attrs);
         if (!temp) return -1;
         combined_attrs = temp;
     }
     
     // Add other attributes if provided
     if (attributes) {
         temp = (char*)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
         if (!temp) {
             free(combined_attrs);
             html_set_error("Memory allocation failed for combined attributes");
             return -1;
         }
         sprintf(temp, "%s %s", combined_attrs, attributes);
         free(combined_attrs);
         combined_attrs = temp;
     }
     
     // Create the image element
     html_element* img = html_add_child(ctx, ctx->current, "img", combined_attrs, NULL);
     free(combined_attrs);
     
     return img ? 0 : -1;
 }
 
 /**
  * @brief Add an anchor (a) element
  *
  * @param ctx The HTML context
  * @param href The href attribute value
  * @param content The inner content
  * @param attributes Additional attributes
  * @return int 0 on success, non-zero on failure
  */
 int html_add_anchor(html_context* ctx, const char* href, const char* content, const char* attributes) {
     if (!ctx || !ctx->current || !href) return -1;
     
     // Combine attributes with href
     char* combined_attrs = html_add_attribute(NULL, "href", href);
     if (!combined_attrs) return -1;
     
     // Add other attributes if provided
     if (attributes) {
         char* temp = (char*)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
         if (!temp) {
             free(combined_attrs);
             html_set_error("Memory allocation failed for combined attributes");
             return -1;
         }
         sprintf(temp, "%s %s", combined_attrs, attributes);
         free(combined_attrs);
         combined_attrs = temp;
     }
     
     // Create the anchor element
     html_element* anchor = html_add_child(ctx, ctx->current, "a", combined_attrs, content);
     free(combined_attrs);
     
     return anchor ? 0 : -1;
 }
 
 /**
  * @brief Begin an unordered list
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_unordered_list(html_context* ctx, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* ul = html_add_child(ctx, ctx->current, "ul", attributes, NULL);
     if (!ul) return -1;
     
     ctx->current = ul;
     return 0;
 }
 
 /**
  * @brief Begin an ordered list
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_ordered_list(html_context* ctx, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* ol = html_add_child(ctx, ctx->current, "ol", attributes, NULL);
     if (!ol) return -1;
     
     ctx->current = ol;
     return 0;
 }
 
 /**
  * @brief End a list
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_list(html_context* ctx) {
     if (!ctx || !ctx->current || !ctx->current->parent) return -1;
     
     // Only end if current element is a list
     if (strcmp(ctx->current->tagname, "ul") != 0 && strcmp(ctx->current->tagname, "ol") != 0) {
         html_set_error("Current element is not a list");
         return -1;
     }
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Add a list item
  *
  * @param ctx The HTML context
  * @param content The inner content
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_add_list_item(html_context* ctx, const char* content, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     // Check if current element is a list
     if (strcmp(ctx->current->tagname, "ul") != 0 && strcmp(ctx->current->tagname, "ol") != 0) {
         html_set_error("Current element is not a list");
         return -1;
     }
     
     html_element* li = html_add_child(ctx, ctx->current, "li", attributes, content);
     return li ? 0 : -1;
 }
 
 /**
  * @brief Add a table element
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_table(html_context* ctx, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     html_element* table = html_add_child(ctx, ctx->current, "table", attributes, NULL);
     if (!table) return -1;
     
     ctx->current = table;
     return 0;
 }
 
 /**
  * @brief End a table element
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_table(html_context* ctx) {
     if (!ctx || !ctx->current || !ctx->current->parent) return -1;
     
     // Only end if current element is a table
     if (strcmp(ctx->current->tagname, "table") != 0) {
         html_set_error("Current element is not a table");
         return -1;
     }
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Begin a table row element
  *
  * @param ctx The HTML context
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_table_row(html_context* ctx, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     // Check if current element is a table or tbody, thead, tfoot
     if (strcmp(ctx->current->tagname, "table") != 0 && 
         strcmp(ctx->current->tagname, "tbody") != 0 &&
         strcmp(ctx->current->tagname, "thead") != 0 &&
         strcmp(ctx->current->tagname, "tfoot") != 0) {
         html_set_error("Current element cannot contain table rows");
         return -1;
     }
     
     html_element* tr = html_add_child(ctx, ctx->current, "tr", attributes, NULL);
     if (!tr) return -1;
     
     ctx->current = tr;
     return 0;
 }
 
 /**
  * @brief End a table row element
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_table_row(html_context* ctx) {
     if (!ctx || !ctx->current || !ctx->current->parent) return -1;
     
     // Only end if current element is a table row
     if (strcmp(ctx->current->tagname, "tr") != 0) {
         html_set_error("Current element is not a table row");
         return -1;
     }
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Add a table cell element
  *
  * @param ctx The HTML context
  * @param content The cell content
  * @param attributes The attributes string
  * @param is_header Whether the cell is a header (th) or data cell (td)
  * @return int 0 on success, non-zero on failure
  */
 int html_add_table_cell(html_context* ctx, const char* content, const char* attributes, int is_header) {
     if (!ctx || !ctx->current) return -1;
     
     // Check if current element is a table row
     if (strcmp(ctx->current->tagname, "tr") != 0) {
         html_set_error("Current element is not a table row");
         return -1;
     }
     
     // Create the cell element
     const char* tagname = is_header ? "th" : "td";
     html_element* cell = html_add_child(ctx, ctx->current, tagname, attributes, content);
     
     return cell ? 0 : -1;
 }
 
 /**
  * @brief Add a form element
  *
  * @param ctx The HTML context
  * @param action The form action
  * @param method The form method
  * @param attributes Additional attributes
  * @return int 0 on success, non-zero on failure
  */
 int html_add_form(html_context* ctx, const char* action, const char* method, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     // Combine attributes
     char* combined_attrs = html_add_attribute(NULL, "action", action ? action : "");
     if (!combined_attrs) return -1;
     
     char* temp = html_add_attribute(combined_attrs, "method", method ? method : "get");
     free(combined_attrs);
     if (!temp) return -1;
     combined_attrs = temp;
     
     // Add other attributes if provided
     if (attributes) {
         temp = (char*)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
         if (!temp) {
             free(combined_attrs);
             html_set_error("Memory allocation failed for combined attributes");
             return -1;
         }
         sprintf(temp, "%s %s", combined_attrs, attributes);
         free(combined_attrs);
         combined_attrs = temp;
     }
     
     // Create the form element
     html_element* form = html_add_child(ctx, ctx->current, "form", combined_attrs, NULL);
     free(combined_attrs);
     
     if (!form) return -1;
     
     ctx->current = form;
     return 0;
 }
 
 /**
  * @brief End a form element
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_form(html_context* ctx) {
     if (!ctx || !ctx->current || !ctx->current->parent) return -1;
     
     // Only end if current element is a form
     if (strcmp(ctx->current->tagname, "form") != 0) {
         html_set_error("Current element is not a form");
         return -1;
     }
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Add an input element
  *
  * @param ctx The HTML context
  * @param type The input type
  * @param name The input name
  * @param value The input value
  * @param attributes Additional attributes
  * @return int 0 on success, non-zero on failure
  */
 int html_add_input(html_context* ctx, const char* type, const char* name, const char* value, const char* attributes) {
     if (!ctx || !ctx->current || !type) return -1;
     
     // Combine attributes
     char* combined_attrs = html_add_attribute(NULL, "type", type);
     if (!combined_attrs) return -1;
     
     if (name) {
         char* temp = html_add_attribute(combined_attrs, "name", name);
         free(combined_attrs);
         if (!temp) return -1;
         combined_attrs = temp;
     }
     
     if (value) {
         char* temp = html_add_attribute(combined_attrs, "value", value);
         free(combined_attrs);
         if (!temp) return -1;
         combined_attrs = temp;
     }
     
     // Add other attributes if provided
     if (attributes) {
         char* temp = (char*)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
         if (!temp) {
             free(combined_attrs);
             html_set_error("Memory allocation failed for combined attributes");
             return -1;
         }
         sprintf(temp, "%s %s", combined_attrs, attributes);
         free(combined_attrs);
         combined_attrs = temp;
     }
     
     // Create the input element
     html_element* input = html_add_child(ctx, ctx->current, "input", combined_attrs, NULL);
     free(combined_attrs);
     
     return input ? 0 : -1;
 }
 
 /**
  * @brief Add a button element
  *
  * @param ctx The HTML context
  * @param type The button type
  * @param content The button text
  * @param attributes Additional attributes
  * @return int 0 on success, non-zero on failure
  */
 int html_add_button(html_context* ctx, const char* type, const char* content, const char* attributes) {
     if (!ctx || !ctx->current) return -1;
     
     // Combine attributes
     char* combined_attrs = NULL;
     
     if (type) {
         combined_attrs = html_add_attribute(NULL, "type", type);
         if (!combined_attrs) return -1;
     }
     
     // Add other attributes if provided
     if (attributes) {
         if (combined_attrs) {
             char* temp = (char*)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
             if (!temp) {
                 free(combined_attrs);
                 html_set_error("Memory allocation failed for combined attributes");
                 return -1;
             }
             sprintf(temp, "%s %s", combined_attrs, attributes);
             free(combined_attrs);
             combined_attrs = temp;
         } else {
             combined_attrs = html_strdup(attributes);
             if (!combined_attrs) return -1;
         }
     }
     
     // Create the button element
     html_element* button = html_add_child(ctx, ctx->current, "button", combined_attrs, content);
     free(combined_attrs);
     
     return button ? 0 : -1;
 }
 
/**
 * @file html_context.c
 * @brief Implementation of HTML context management functions
 *
 * This file contains functions for initializing, managing, and finalizing
 * HTML contexts, which represent the overall state of an HTML document.
 * It handles document structure, element registration, and rendering.
 */

 #include "HTML.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 #include <ctype.h>
 
 /* ===== Context Creation and Initialization ===== */
 
 /**
  * @brief Initialize an HTML context with file output
  * 
  * @param filename The name of the output file
  * @param lang The language attribute for the html tag
  * @param title The title of the HTML document
  * @return html_context* Pointer to the initialized context, NULL on failure
  */
 html_context* html_init_file(const char* filename, const char* lang, const char* title) {
     if (!filename) {
         html_set_error("Filename cannot be NULL");
         return NULL;
     }
     
     // Open the output file
     FILE* file = fopen(filename, "w");
     if (!file) {
         html_set_error("Failed to open output file '%s'", filename);
         return NULL;
     }
     
     // Allocate the context
     html_context* ctx = (html_context*)malloc(sizeof(html_context));
     if (!ctx) {
         fclose(file);
         html_set_error("Memory allocation failed for HTML context");
         return NULL;
     }
     
     // Initialize context fields
     memset(ctx, 0, sizeof(html_context));
     ctx->output_file = file;
     ctx->indent_level = 0;
     
     // Set language and title
     if (lang) {
         ctx->language = html_strdup(lang);
         if (!ctx->language) {
             fclose(file);
             free(ctx);
             return NULL;
         }
     } else {
         ctx->language = html_strdup("en");
         if (!ctx->language) {
             fclose(file);
             free(ctx);
             return NULL;
         }
     }
     
     if (title) {
         ctx->title = html_strdup(title);
         if (!ctx->title) {
             free(ctx->language);
             fclose(file);
             free(ctx);
             return NULL;
         }
     } else {
         ctx->title = html_strdup("Untitled Document");
         if (!ctx->title) {
             free(ctx->language);
             fclose(file);
             free(ctx);
             return NULL;
         }
     }
     
     // Create ID map
     ctx->element_map = html_create_id_map(16);
     if (!ctx->element_map) {
         free(ctx->title);
         free(ctx->language);
         fclose(file);
         free(ctx);
         return NULL;
     }
     
     // Create the document structure
     if (!html_create_document_structure(ctx)) {
         html_finalize(ctx);
         return NULL;
     }
     
     return ctx;
 }
 
 /**
  * @brief Create the basic HTML document structure
  * 
  * @param ctx The HTML context
  * @return int 1 on success, 0 on failure
  */
 int html_create_document_structure(html_context* ctx) {
     if (!ctx) return 0;
     
     // Create the root (html) element
     char* html_attrs = NULL;
     if (ctx->language) {
         html_attrs = html_add_attribute(NULL, "lang", ctx->language);
         if (!html_attrs) return 0;
     }
     
     ctx->root = html_create_element("html", html_attrs, NULL);
     free(html_attrs);
     
     if (!ctx->root) return 0;
     
     // Create the head element
     html_element* head = html_add_child(ctx, ctx->root, "head", NULL, NULL);
     if (!head) {
         html_free_element(ctx->root);
         ctx->root = NULL;
         return 0;
     }
     
     // Add the title element to head
     if (ctx->title) {
         html_element* title = html_add_child(ctx, head, "title", NULL, ctx->title);
         if (!title) {
             html_free_element(ctx->root);
             ctx->root = NULL;
             return 0;
         }
     }
     
     // Create the body element
     html_element* body = html_add_child(ctx, ctx->root, "body", NULL, NULL);
     if (!body) {
         html_free_element(ctx->root);
         ctx->root = NULL;
         return 0;
     }
     
     // Set the current element to the body
     ctx->current = body;
     
     return 1;
 }
 
 /**
  * @brief Free all resources used by the HTML context
  * 
  * @param ctx The HTML context to finalize
  */
 void html_finalize(html_context* ctx) {
     if (!ctx) return;
     
     // Render the HTML to file if it hasn't been done yet
     if (ctx->output_file && ctx->root) {
         html_render(ctx);
     }
     
     // Free the root element (which will recursively free all children)
     if (ctx->root) {
         html_free_element(ctx->root);
         ctx->root = NULL;
     }
     
     // Free the ID map
     if (ctx->element_map) {
         html_free_id_map(ctx->element_map);
         ctx->element_map = NULL;
     }
     
     // Close the output file
     if (ctx->output_file) {
         fclose(ctx->output_file);
         ctx->output_file = NULL;
     }
     
     // Free strings
     free(ctx->language);
     free(ctx->title);
     
     // Free the context itself
     free(ctx);
 }
 
 /* ===== Element Registration and Lookup ===== */
 
 /**
  * @brief Register an element by ID in the hash map
  * 
  * @param ctx The HTML context
  * @param element The element to register
  * @return int 0 on success, non-zero on failure
  */
 int html_register_element_by_id(html_context* ctx, html_element* element) {
     if (!ctx || !ctx->element_map || !element || !element->id) return -1;
     
     // Check if we need to resize the hash map
     if (ctx->element_map->size >= ctx->element_map->capacity * 0.75) {
         if (html_resize_id_map(ctx->element_map) != 0) {
             return -1;
         }
     }
     
     // Calculate hash
     unsigned int hash = html_hash_string(element->id) % ctx->element_map->capacity;
     
     // Find the next available slot (linear probing)
     while (ctx->element_map->keys[hash] != NULL) {
         // Check if this ID is already in the map
         if (strcmp(ctx->element_map->keys[hash], element->id) == 0) {
             html_set_error("Duplicate element ID: '%s'", element->id);
             return -1;
         }
         
         hash = (hash + 1) % ctx->element_map->capacity;
     }
     
     // Store the ID and element
     ctx->element_map->keys[hash] = element->id;
     ctx->element_map->values[hash] = element;
     ctx->element_map->size++;
     
     return 0;
 }
 
 /**
  * @brief Get an element by its ID
  * 
  * @param ctx The HTML context
  * @param id The ID to look for
  * @return html_element* The element with the given ID, NULL if not found
  */
 html_element* html_get_element_by_id(html_context* ctx, const char* id) {
     if (!ctx || !ctx->element_map || !id) return NULL;
     
     // Calculate hash
     unsigned int hash = html_hash_string(id) % ctx->element_map->capacity;
     
     // Search for the ID (linear probing)
     int i = 0;
     while (ctx->element_map->keys[hash] != NULL && i < ctx->element_map->capacity) {
         if (ctx->element_map->keys[hash] && strcmp(ctx->element_map->keys[hash], id) == 0) {
             return ctx->element_map->values[hash];
         }
         
         hash = (hash + 1) % ctx->element_map->capacity;
         i++;
     }
     
     return NULL;
 }
 
 /* ===== Context State Management ===== */
 
 /**
  * @brief Set the current element in the context
  * 
  * @param ctx The HTML context
  * @param element The element to set as current
  * @return int 0 on success, non-zero on failure
  */
 int html_set_current_element(html_context* ctx, html_element* element) {
     if (!ctx || !element) return -1;
     
     // Verify the element belongs to this context
     html_element* current = element;
     while (current->parent && current != ctx->root) {
         current = current->parent;
     }
     
     if (current != ctx->root) {
         html_set_error("Element does not belong to this context");
         return -1;
     }
     
     ctx->current = element;
     return 0;
 }
 
 /**
  * @brief Add a style element to the head section
  * 
  * @param ctx The HTML context
  * @param style_content The CSS content
  * @return int 0 on success, non-zero on failure
  */
 int html_add_style(html_context* ctx, const char* style_content) {
     if (!ctx || !ctx->root || !style_content) return -1;
     
     // Find the head element
     html_element* head = html_find_head(ctx);
     if (!head) {
         html_set_error("Could not find head element");
         return -1;
     }
     
     // Save the current element
     html_element* saved_current = ctx->current;
     
     // Set head as current
     ctx->current = head;
     
     // Add the style element
     html_element* style = html_add_child(ctx, head, "style", NULL, style_content);
     
     // Restore the current element
     ctx->current = saved_current;
     
     return style ? 0 : -1;
 }
 
 /**
  * @brief Add a script element to the head section
  * 
  * @param ctx The HTML context
  * @param script_content The JavaScript content
  * @param is_external Whether the script is external (src attribute)
  * @return int 0 on success, non-zero on failure
  */
 int html_add_script(html_context* ctx, const char* script_content, int is_external) {
     if (!ctx || !ctx->root || !script_content) return -1;
     
     // Find the head element
     html_element* head = html_find_head(ctx);
     if (!head) {
         html_set_error("Could not find head element");
         return -1;
     }
     
     // Save the current element
     html_element* saved_current = ctx->current;
     
     // Set head as current
     ctx->current = head;
     
     // Add the script element with appropriate attributes
     html_element* script;
     if (is_external) {
         char* attrs = html_add_attribute(NULL, "src", script_content);
         script = html_add_child(ctx, head, "script", attrs, NULL);
         free(attrs);
     } else {
         script = html_add_child(ctx, head, "script", NULL, script_content);
     }
     
     // Restore the current element
     ctx->current = saved_current;
     
     return script ? 0 : -1;
 }
 
 /**
  * @brief Add a meta tag to the head section
  * 
  * @param ctx The HTML context
  * @param name The name attribute
  * @param content The content attribute
  * @return int 0 on success, non-zero on failure
  */
 int html_add_meta(html_context* ctx, const char* name, const char* content) {
     if (!ctx || !ctx->root || !name || !content) return -1;
     
     // Find the head element
     html_element* head = html_find_head(ctx);
     if (!head) {
         html_set_error("Could not find head element");
         return -1;
     }
     
     // Save the current element
     html_element* saved_current = ctx->current;
     
     // Set head as current
     ctx->current = head;
     
     // Create attributes
     char* attrs = html_add_attribute(NULL, "name", name);
     if (!attrs) {
         ctx->current = saved_current;
         return -1;
     }
     
     char* attrs_with_content = html_add_attribute(attrs, "content", content);
     free(attrs);
     
     if (!attrs_with_content) {
         ctx->current = saved_current;
         return -1;
     }
     
     // Add the meta element
     html_element* meta = html_add_child(ctx, head, "meta", attrs_with_content, NULL);
     free(attrs_with_content);
     
     // Restore the current element
     ctx->current = saved_current;
     
     return meta ? 0 : -1;
 }
 
 /**
  * @brief Add a link tag to the head section
  * 
  * @param ctx The HTML context
  * @param rel The rel attribute
  * @param href The href attribute
  * @param type The type attribute
  * @return int 0 on success, non-zero on failure
  */
 int html_add_link(html_context* ctx, const char* rel, const char* href, const char* type) {
     if (!ctx || !ctx->root || !rel || !href) return -1;
     
     // Find the head element
     html_element* head = html_find_head(ctx);
     if (!head) {
         html_set_error("Could not find head element");
         return -1;
     }
     
     // Save the current element
     html_element* saved_current = ctx->current;
     
     // Set head as current
     ctx->current = head;
     
     // Create attributes
     char* attrs = html_add_attribute(NULL, "rel", rel);
     if (!attrs) {
         ctx->current = saved_current;
         return -1;
     }
     
     char* attrs_with_href = html_add_attribute(attrs, "href", href);
     free(attrs);
     
     if (!attrs_with_href) {
         ctx->current = saved_current;
         return -1;
     }
     
     // Add type attribute if provided
     char* final_attrs = attrs_with_href;
     if (type) {
         final_attrs = html_add_attribute(attrs_with_href, "type", type);
         free(attrs_with_href);
         
         if (!final_attrs) {
             ctx->current = saved_current;
             return -1;
         }
     }
     
     // Add the link element
     html_element* link = html_add_child(ctx, head, "link", final_attrs, NULL);
     free(final_attrs);
     
     // Restore the current element
     ctx->current = saved_current;
     
     return link ? 0 : -1;
 }
 
 /* ===== HTML Rendering ===== */
 
 /**
  * @brief Render the HTML document to the output file
  * 
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_render(html_context* ctx) {
     if (!ctx || !ctx->root || !ctx->output_file) return -1;
     
     // Write the doctype
     fprintf(ctx->output_file, "<!DOCTYPE html>\n");
     
     // Render the root element and all its children
     ctx->indent_level = 0;
     return html_render_element(ctx, ctx->root);
 }
 
 /**
  * @brief Render an HTML element and its children to the output file
  * 
  * @param ctx The HTML context
  * @param element The element to render
  * @return int 0 on success, non-zero on failure
  */
 int html_render_element(html_context* ctx, html_element* element) {
     if (!ctx || !element || !ctx->output_file) return -1;
     
     // Get indentation string
     char* indent = html_generate_indent(ctx->indent_level);
     if (!indent) return -1;
     
     // Write opening tag
     fprintf(ctx->output_file, "%s<%s", indent, element->tagname);
     
     // Write attributes if any
     if (element->attributes && strlen(element->attributes) > 0) {
         fprintf(ctx->output_file, " %s", element->attributes);
     }
     
     // Handle self-closing tags
     if (html_is_self_closing(element->tagname)) {
         fprintf(ctx->output_file, " />\n");
         free(indent);
         return 0;
     }
     
     // Close opening tag
     fprintf(ctx->output_file, ">");
     
     // Determine if we need newlines (based on whether this is a block element)
     int is_block = html_is_block_element(element->tagname);
     
     // Write content or children
     if (element->content && strlen(element->content) > 0) {
         // If it's a block element, add a newline before content
         if (is_block) {
             fprintf(ctx->output_file, "\n%s  ", indent);
         }
         
         fprintf(ctx->output_file, "%s", element->content);
         
         // If it's a block element, add a newline after content
         if (is_block) {
             fprintf(ctx->output_file, "\n%s", indent);
         }
     } else if (element->children_count > 0) {
         // Add newline before children
         fprintf(ctx->output_file, "\n");
         
         // Increase indentation for children
         ctx->indent_level++;
         
         // Render all children
         for (int i = 0; i < element->children_count; i++) {
             html_render_element(ctx, element->children[i]);
         }
         
         // Decrease indentation after children
         ctx->indent_level--;
         
         // Add indentation before closing tag
         fprintf(ctx->output_file, "%s", indent);
     }
     
     // Write closing tag
     fprintf(ctx->output_file, "</%s>\n", element->tagname);
     
     free(indent);
     return 0;
 }
 
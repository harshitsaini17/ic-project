/**
 * @file html_gen.c
 * @brief Main implementation file for HTML generation library
 *
 * This file contains core functions for initializing, manipulating,
 * and finalizing HTML documents. It provides the main API functions
 * that users will interact with directly.
 */

 #include "HTML.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 
 /* ===== Version Information ===== */
 
 #define HTML_GEN_VERSION_MAJOR 1
 #define HTML_GEN_VERSION_MINOR 0
 #define HTML_GEN_VERSION_PATCH 0
 
 /**
  * @brief Get the library version as a string
  * 
  * @return const char* Version string in format "X.Y.Z"
  */
 const char* html_get_version(void) {
     static char version[16];
     snprintf(version, sizeof(version), "%d.%d.%d", 
              HTML_GEN_VERSION_MAJOR,
              HTML_GEN_VERSION_MINOR,
              HTML_GEN_VERSION_PATCH);
     return version;
 }
 
 /**
  * Forward declarations for functions defined in other files but used here
  */
 extern int html_create_document_structure(html_context* ctx);
 extern int html_render_element(html_context* ctx, html_element* element);
 
 /* ===== Core API Implementation ===== */  

 /**
  * @brief Initialize an HTML context with string output
  * 
  * Creates a new HTML context with a properly structured HTML document
  * that will be rendered to a string buffer when html_render_to_string is called.
  * 
  * @param lang The language attribute for the html tag
  * @param title The title of the HTML document
  * @return html_context* Pointer to the initialized context, NULL on failure
  */
 html_context* html_init_string(const char* lang, const char* title) {
     html_clear_error();
     
     // Allocate the context
     html_context* ctx = (html_context*)malloc(sizeof(html_context));
     if (!ctx) {
         html_set_error("Memory allocation failed for HTML context");
         return NULL;
     }
     
     // Initialize context fields
     memset(ctx, 0, sizeof(html_context));
     ctx->output_file = NULL; // No file output
     ctx->indent_level = 0;
     
     // Set language and title
     if (lang) {
         ctx->language = html_strdup(lang);
         if (!ctx->language) {
             free(ctx);
             return NULL;
         }
     } else {
         ctx->language = html_strdup("en");
         if (!ctx->language) {
             free(ctx);
             return NULL;
         }
     }
     
     if (title) {
         ctx->title = html_strdup(title);
         if (!ctx->title) {
             free(ctx->language);
             free(ctx);
             return NULL;
         }
     } else {
         ctx->title = html_strdup("Untitled Document");
         if (!ctx->title) {
             free(ctx->language);
             free(ctx);
             return NULL;
         }
     }
     
     // Create ID map
     ctx->element_map = html_create_id_map(16);
     if (!ctx->element_map) {
         free(ctx->title);
         free(ctx->language);
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
  * @brief Free all resources used by the HTML context
  * 
  * @param ctx The HTML context to finalize
  */


 /**
  * @brief Render the HTML document to a string
  * 
  * @param ctx The HTML context
  * @return char* Rendered HTML string (caller must free), NULL on failure
  */
 char* html_render_to_string(html_context* ctx) {
     html_clear_error();
     
     if (!ctx || !ctx->root) {
         html_set_error("Invalid HTML context or root element");
         return NULL;
     }
     
     // Create a temporary file to write to
     FILE* temp_file = tmpfile();
     if (!temp_file) {
         html_set_error("Failed to create temporary file for rendering");
         return NULL;
     }
     
     // Save the original output file and set the temporary file
     FILE* original_file = ctx->output_file;
     ctx->output_file = temp_file;
     
     // Render to the temporary file
     fprintf(temp_file, "<!DOCTYPE html>\n");
     ctx->indent_level = 0;
     int result = html_render_element(ctx, ctx->root);
     
     // Restore the original output file
     ctx->output_file = original_file;
     
     if (result != 0) {
         fclose(temp_file);
         return NULL;
     }
     
     // Get the size of the rendered HTML
     fseek(temp_file, 0, SEEK_END);
     long size = ftell(temp_file);
     rewind(temp_file);
     
     // Allocate memory for the string
     char* html_string = (char*)malloc(size + 1);
     if (!html_string) {
         fclose(temp_file);
         html_set_error("Memory allocation failed for HTML string");
         return NULL;
     }
     
     // Read the rendered HTML into the string
     size_t read_size = fread(html_string, 1, size, temp_file);
     fclose(temp_file);
     
     if (read_size != (size_t)size) {
         free(html_string);
         html_set_error("Failed to read rendered HTML");
         return NULL;
     }
     
     // Null-terminate the string
     html_string[size] = '\0';
     
     return html_string;
 }
 


 /**
  * @brief Begin a specific tag and set it as current
  * 
  * @param ctx The HTML context
  * @param tagname The name of the tag to begin
  * @param attributes The attributes string
  * @return int 0 on success, non-zero on failure
  */
 int html_begin_tag(html_context* ctx, const char* tagname, const char* attributes) {
     html_clear_error();
     
     if (!ctx || !ctx->current || !tagname) {
         html_set_error("Invalid parameters for beginning tag");
         return -1;
     }
     
     html_element* element = html_add_child(ctx, ctx->current, tagname, attributes, NULL);
     if (!element) return -1;
     
     ctx->current = element;
     return 0;
 }
 
 /**
  * @brief End the current tag (returns to parent element as current)
  * 
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_end_tag(html_context* ctx) {
     html_clear_error();
     
     if (!ctx || !ctx->current || !ctx->current->parent) {
         html_set_error("Cannot end tag: no current element or at root level");
         return -1;
     }
     
     ctx->current = ctx->current->parent;
     return 0;
 }
 
 /**
  * @brief Add content to the current element
  * 
  * @param ctx The HTML context
  * @param content The content to add
  * @return int 0 on success, non-zero on failure
  */
 int html_add_content(html_context* ctx, const char* content) {
     html_clear_error();
     
     if (!ctx || !ctx->current) {
         html_set_error("Invalid HTML context or current element");
         return -1;
     }
     
     if (!content) {
         return 0; // Nothing to add
     }
     
     // If the element already has content, append to it
     if (ctx->current->content) {
         char* new_content = (char*)malloc(strlen(ctx->current->content) + strlen(content) + 1);
         if (!new_content) {
             html_set_error("Memory allocation failed for content");
             return -1;
         }
         
         strcpy(new_content, ctx->current->content);
         strcat(new_content, content);
         
         free(ctx->current->content);
         ctx->current->content = new_content;
     } else {
         // Set the content directly
         ctx->current->content = html_strdup(content);
         if (!ctx->current->content) {
             html_set_error("Memory allocation failed for content");
             return -1;
         }
     }
     
     return 0;
 }
 
 /**
  * @brief Navigate to element identified by ID
  *
  * @param ctx The HTML context
  * @param id The ID of the element to navigate to
  * @return int 0 on success, non-zero on failure
  */
 int html_navigate_to_element(html_context* ctx, const char* id) {
     html_clear_error();
     
     if (!ctx || !id) {
         html_set_error("Invalid HTML context or ID");
         return -1;
     }
     
     html_element* element = html_get_element_by_id(ctx, id);
     if (!element) {
         html_set_error("No element found with ID: %s", id);
         return -1;
     }
     
     ctx->current = element;
     return 0;
 }
 
 /**
  * @brief Navigate to the body element
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_navigate_to_body(html_context* ctx) {
     html_clear_error();
     
     if (!ctx) {
         html_set_error("Invalid HTML context");
         return -1;
     }
     
     html_element* body = html_find_body(ctx);
     if (!body) {
         html_set_error("Body element not found");
         return -1;
     }
     
     ctx->current = body;
     return 0;
 }
 
 /**
  * @brief Navigate to the head element
  *
  * @param ctx The HTML context
  * @return int 0 on success, non-zero on failure
  */
 int html_navigate_to_head(html_context* ctx) {
     html_clear_error();
     
     if (!ctx) {
         html_set_error("Invalid HTML context");
         return -1;
     }
     
     html_element* head = html_find_head(ctx);
     if (!head) {
         html_set_error("Head element not found");
         return -1;
     }
     
     ctx->current = head;
     return 0;
 }
 
 /**
  * @brief Get the last error message
  *
  * @return const char* Error message or empty string if no error
  */
 const char* html_get_last_error(void) {
     return html_get_error();
 }
 
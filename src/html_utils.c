/**
 * @file html_utils.c
 * @brief Utility functions for HTML generation library
 *
 * This file contains utility functions for string manipulation, memory management,
 * error handling, and other supporting operations for the HTML generation library.
 */

 #include "HTML.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 #include <ctype.h>
 
 /* ===== Error Handling ===== */
 
 /**
  * Global variable to track last error
  */
 static char error_message[256] = {0};
 
 /**
  * @brief Set error message
  *
  * @param format Format string (printf style)
  * @param ... Variable arguments
  */
 void html_set_error(const char *format, ...) {
     va_list args;
     va_start(args, format);
     vsnprintf(error_message, sizeof(error_message), format, args);
     va_end(args);
 }
 
 /**
  * @brief Get last error message
  *
  * @return const char* Error message
  */
 const char* html_get_error(void) {
     return error_message;
 }
 
 /**
  * @brief Clear error message
  */
 void html_clear_error(void) {
     error_message[0] = '\0';
 }
 
 /* ===== String Manipulation ===== */
 
 /**
  * @brief Safe string duplication
  *
  * @param str String to duplicate
  * @return char* New string or NULL on failure
  */
 char* html_strdup(const char* str) {
     if (!str) return NULL;
     
     size_t len = strlen(str);
     char* new_str = (char*)malloc(len + 1);
     
     if (!new_str) {
         html_set_error("Memory allocation failed for string duplication");
         return NULL;
     }
     
     memcpy(new_str, str, len + 1);
     return new_str;
 }
 
 /**
  * @brief Safe string concatenation
  *
  * @param dest Destination string (will be reallocated)
  * @param src Source string to append
  * @return char* New concatenated string or NULL on failure
  */
 char* html_strcat(char* dest, const char* src) {
     if (!src) return dest;
     if (!dest) return html_strdup(src);
     
     size_t dest_len = strlen(dest);
     size_t src_len = strlen(src);
     size_t new_len = dest_len + src_len;
     
     char* new_str = (char*)realloc(dest, new_len + 1);
     if (!new_str) {
         html_set_error("Memory allocation failed for string concatenation");
         return dest; // Return original to avoid memory leak
     }
     
     memcpy(new_str + dest_len, src, src_len + 1);
     return new_str;
 }
 
 /**
  * @brief Escape HTML special characters in a string
  *
  * @param str String to escape
  * @return char* New escaped string or NULL on failure
  */
 char* html_escape_string(const char* str) {
     if (!str) return NULL;
     
     // Calculate required size by counting special characters
     size_t len = strlen(str);
     size_t new_len = len;
     
     for (size_t i = 0; i < len; i++) {
         switch (str[i]) {
             case '&': new_len += 4; break; // &amp;
             case '<': new_len += 3; break; // &lt;
             case '>': new_len += 3; break; // &gt;
             case '"': new_len += 5; break; // &quot;
             case '\'': new_len += 5; break; // &#39;
             default: break;
         }
     }
     
     // If no escaping needed, just duplicate
     if (new_len == len) return html_strdup(str);
     
     // Allocate new string
     char* new_str = (char*)malloc(new_len + 1);
     if (!new_str) {
         html_set_error("Memory allocation failed for string escaping");
         return NULL;
     }
     
     // Perform the escaping
     size_t j = 0;
     for (size_t i = 0; i < len; i++) {
         switch (str[i]) {
             case '&':
                 memcpy(new_str + j, "&amp;", 5);
                 j += 5;
                 break;
             case '<':
                 memcpy(new_str + j, "&lt;", 4);
                 j += 4;
                 break;
             case '>':
                 memcpy(new_str + j, "&gt;", 4);
                 j += 4;
                 break;
             case '"':
                 memcpy(new_str + j, "&quot;", 6);
                 j += 6;
                 break;
             case '\'':
                 memcpy(new_str + j, "&#39;", 5);
                 j += 5;
                 break;
             default:
                 new_str[j++] = str[i];
                 break;
         }
     }
     
     new_str[j] = '\0';
     return new_str;
 }
 
 /**
  * @brief Trim whitespace from both ends of a string
  *
  * @param str String to trim (modified in-place)
  * @return char* Pointer to the trimmed string (same as input)
  */
 char* html_trim_string(char* str) {
     if (!str) return NULL;
     
     // Trim leading whitespace
     char* start = str;
     while (isspace((unsigned char)*start)) start++;
     
     if (start != str) {
         memmove(str, start, strlen(start) + 1);
     }
     
     // Trim trailing whitespace
     char* end = str + strlen(str) - 1;
     while (end > str && isspace((unsigned char)*end)) end--;
     *(end + 1) = '\0';
     
     return str;
 }
 
 /* ===== Attribute Handling ===== */
 
 /**
  * @brief Extract attribute value from attributes string
  *
  * @param attributes The attributes string
  * @param name The attribute name to extract
  * @return char* The extracted value, NULL if not found
  */
 char* html_extract_attribute(const char* attributes, const char* name) {
     if (!attributes || !name) return NULL;
     
     size_t name_len = strlen(name);
     const char* attr_start = attributes;
     
     while ((attr_start = strstr(attr_start, name)) != NULL) {
         // Check if it's a complete attribute name (preceded by space or start of string)
         if (attr_start == attributes || isspace((unsigned char)*(attr_start-1))) {
             attr_start += name_len;
             
             // Skip whitespace
             while (isspace((unsigned char)*attr_start)) attr_start++;
             
             // Check for equals sign
             if (*attr_start == '=') {
                 attr_start++;
                 
                 // Skip whitespace
                 while (isspace((unsigned char)*attr_start)) attr_start++;
                 
                 // Extract quoted value
                 if (*attr_start == '"' || *attr_start == '\'') {
                     char quote = *attr_start;
                     attr_start++;
                     
                     const char* value_end = strchr(attr_start, quote);
                     if (value_end) {
                         size_t value_len = value_end - attr_start;
                         char* value = malloc(value_len + 1);
                         
                         if (!value) {
                             html_set_error("Memory allocation failed for attribute extraction");
                             return NULL;
                         }
                         
                         memcpy(value, attr_start, value_len);
                         value[value_len] = '\0';
                         return value;
                     }
                 } else {
                     // Extract unquoted value (until whitespace or end)
                     const char* value_end = attr_start;
                     while (*value_end && !isspace((unsigned char)*value_end)) value_end++;
                     
                     size_t value_len = value_end - attr_start;
                     char* value = malloc(value_len + 1);
                     
                     if (!value) {
                         html_set_error("Memory allocation failed for attribute extraction");
                         return NULL;
                     }
                     
                     memcpy(value, attr_start, value_len);
                     value[value_len] = '\0';
                     return value;
                 }
             }
         }
         
         // Move past this instance
         attr_start++;
     }
     
     return NULL;
 }
 
 /**
  * @brief Extract ID from attributes string
  *
  * @param attributes The attributes string
  * @return char* The extracted ID, NULL if not found
  */
 char* html_extract_id(const char* attributes) {
     return html_extract_attribute(attributes, "id");
 }
 
 /**
  * @brief Create a new attribute string with the given attribute added or updated
  *
  * @param attributes Original attributes string (can be NULL)
  * @param name Attribute name
  * @param value Attribute value
  * @return char* New attributes string with the attribute added/updated
  */
 char* html_add_attribute(const char* attributes, const char* name, const char* value) {
     if (!name || !value) return html_strdup(attributes ? attributes : "");
     
     // Calculate new string size
     size_t attr_len = attributes ? strlen(attributes) : 0;
     size_t name_len = strlen(name);
     size_t value_len = strlen(value);
     size_t new_attr_len = attr_len + name_len + value_len + 4; // +4 for space, equals, quotes
     
     char* new_attributes = (char*)malloc(new_attr_len + 1);
     if (!new_attributes) {
         html_set_error("Memory allocation failed for attribute addition");
         return NULL;
     }
     
     // Copy original attributes if they exist
     if (attributes && attr_len > 0) {
         strcpy(new_attributes, attributes);
         
         // Check if we need to add a space
         if (new_attributes[attr_len - 1] != ' ') {
             strcat(new_attributes, " ");
         }
     } else {
         new_attributes[0] = '\0';
     }
     
     // Add the new attribute
     strcat(new_attributes, name);
     strcat(new_attributes, "=\"");
     strcat(new_attributes, value);
     strcat(new_attributes, "\"");
     
     return new_attributes;
 }
 
 /* ===== Element Tree Operations ===== */
 
 /**
  * @brief Find the head element in the document
  *
  * @param ctx The HTML context
  * @return html_element* The head element or NULL if not found
  */
 html_element* html_find_head(html_context* ctx) {
     if (!ctx || !ctx->root) return NULL;
     
     // The head is typically a direct child of html (root)
     for (int i = 0; i < ctx->root->children_count; i++) {
         if (ctx->root->children[i] && 
             ctx->root->children[i]->tagname && 
             strcmp(ctx->root->children[i]->tagname, "head") == 0) {
             return ctx->root->children[i];
         }
     }
     
     return NULL;
 }
 
 /**
  * @brief Find the body element in the document
  *
  * @param ctx The HTML context
  * @return html_element* The body element or NULL if not found
  */
 html_element* html_find_body(html_context* ctx) {
     if (!ctx || !ctx->root) return NULL;
     
     // The body is typically a direct child of html (root)
     for (int i = 0; i < ctx->root->children_count; i++) {
         if (ctx->root->children[i] && 
             ctx->root->children[i]->tagname && 
             strcmp(ctx->root->children[i]->tagname, "body") == 0) {
             return ctx->root->children[i];
         }
     }
     
     return NULL;
 }
 
 /**
  * @brief Check if an element can be a child of another element
  *
  * @param parent_tag The parent tag name
  * @param child_tag The child tag name
  * @return int 1 if valid, 0 if invalid
  */
 int html_is_valid_child(const char* parent_tag, const char* child_tag) {
     if (!parent_tag || !child_tag) return 0;
     
     // Head element can only contain meta, title, link, style, script
     if (strcmp(parent_tag, "head") == 0) {
         return (strcmp(child_tag, "meta") == 0 || 
                 strcmp(child_tag, "title") == 0 || 
                 strcmp(child_tag, "link") == 0 || 
                 strcmp(child_tag, "style") == 0 || 
                 strcmp(child_tag, "script") == 0);
     }
     
     // Table can only contain specific elements
     if (strcmp(parent_tag, "table") == 0) {
         return (strcmp(child_tag, "thead") == 0 || 
                 strcmp(child_tag, "tbody") == 0 || 
                 strcmp(child_tag, "tfoot") == 0 || 
                 strcmp(child_tag, "tr") == 0 || 
                 strcmp(child_tag, "caption") == 0);
     }
     
     // tr can only contain th and td
     if (strcmp(parent_tag, "tr") == 0) {
         return (strcmp(child_tag, "th") == 0 || 
                 strcmp(child_tag, "td") == 0);
     }
     
     // ul and ol can only contain li
     if (strcmp(parent_tag, "ul") == 0 || strcmp(parent_tag, "ol") == 0) {
         return (strcmp(child_tag, "li") == 0);
     }
     
     // By default, most elements can contain most elements
     return 1;
 }
 
 /* ===== Indentation and Formatting ===== */
 
 /**
  * @brief Generate indentation string
  *
  * @param level Indentation level
  * @return char* Indentation string (spaces)
  */
 char* html_generate_indent(int level) {
     const int spaces_per_level = 2;
     int total_spaces = level * spaces_per_level;
     
     // Cap the indentation at a reasonable level
     if (total_spaces > 40) total_spaces = 40;
     
     char* indent = (char*)malloc(total_spaces + 1);
     if (!indent) {
         html_set_error("Memory allocation failed for indentation");
         return NULL;
     }
     
     memset(indent, ' ', total_spaces);
     indent[total_spaces] = '\0';
     
     return indent;
 }
 
 /**
  * @brief Check if an element should have its content on a new line
  *
  * @param tagname The tag name to check
  * @return int 1 if content should be on new line, 0 otherwise
  */
 int html_is_block_element(const char* tagname) {
     if (!tagname) return 0;
     
     // List of common block elements
     const char* block_elements[] = {
         "div", "p", "h1", "h2", "h3", "h4", "h5", "h6",
         "ul", "ol", "li", "table", "tr", "td", "th",
         "form", "fieldset", "header", "footer", "section",
         "article", "aside", "nav", "main", NULL
     };
     
     for (int i = 0; block_elements[i]; i++) {
         if (strcmp(tagname, block_elements[i]) == 0) {
             return 1;
         }
     }
     
     return 0;
 }
 
 /**
  * @brief Check if an element is self-closing
  *
  * @param tagname The tag name to check
  * @return int 1 if self-closing, 0 otherwise
  */
 int html_is_self_closing(const char* tagname) {
     if (!tagname) return 0;
     
     // List of self-closing elements
     const char* self_closing[] = {
         "area", "base", "br", "col", "embed", "hr", "img", 
         "input", "link", "meta", "param", "source", "track", "wbr", NULL
     };
     
     for (int i = 0; self_closing[i]; i++) {
         if (strcmp(tagname, self_closing[i]) == 0) {
             return 1;
         }
     }
     
     return 0;
 }
 
 /* ===== Hash Map Operations ===== */
 
 /**
  * @brief Calculate a simple hash for a string
  *
  * @param str The string to hash
  * @return unsigned int The hash value
  */
 unsigned int html_hash_string(const char* str) {
     if (!str) return 0;
     
     unsigned int hash = 5381;
     int c;
     
     while ((c = *str++)) {
         hash = ((hash << 5) + hash) + c; // hash * 33 + c
     }
     
     return hash;
 }
 
 /**
  * @brief Resize the ID hash map when it reaches capacity
  *
  * @param map The ID map to resize
  * @return int 0 on success, non-zero on failure
  */
 int html_resize_id_map(id_map* map) {
     if (!map) return -1;
     
     int new_capacity = map->capacity * 2;
     if (new_capacity < 8) new_capacity = 8;
     
     char** new_keys = (char**)calloc(new_capacity, sizeof(char*));
     html_element** new_values = (html_element**)calloc(new_capacity, sizeof(html_element*));
     
     if (!new_keys || !new_values) {
         free(new_keys);
         free(new_values);
         html_set_error("Memory allocation failed for ID map resize");
         return -1;
     }
     
     // Rehash all existing entries
     for (int i = 0; i < map->capacity; i++) {
         if (map->keys[i]) {
             unsigned int hash = html_hash_string(map->keys[i]) % new_capacity;
             
             // Find next available slot (linear probing)
             while (new_keys[hash]) {
                 hash = (hash + 1) % new_capacity;
             }
             
             new_keys[hash] = map->keys[i];
             new_values[hash] = map->values[i];
         }
     }
     
     // Free old arrays and update the map
     free(map->keys);
     free(map->values);
     
     map->keys = new_keys;
     map->values = new_values;
     map->capacity = new_capacity;
     
     return 0;
 }
 
 /**
  * @brief Create a new ID map
  *
  * @param initial_capacity Initial capacity of the map
  * @return id_map* Pointer to the new map, NULL on failure
  */
 id_map* html_create_id_map(int initial_capacity) {
     if (initial_capacity < 4) initial_capacity = 4;
     
     id_map* map = (id_map*)malloc(sizeof(id_map));
     if (!map) {
         html_set_error("Memory allocation failed for ID map");
         return NULL;
     }
     
     map->keys = (char**)calloc(initial_capacity, sizeof(char*));
     map->values = (html_element**)calloc(initial_capacity, sizeof(html_element*));
     
     if (!map->keys || !map->values) {
         free(map->keys);
         free(map->values);
         free(map);
         html_set_error("Memory allocation failed for ID map arrays");
         return NULL;
     }
     
     map->capacity = initial_capacity;
     map->size = 0;
     
     return map;
 }
 
 /**
  * @brief Free all resources used by an ID map
  *
  * @param map The ID map to free
  */
 void html_free_id_map(id_map* map) {
     if (!map) return;
     
     // Note: We don't free the keys or values because they are owned by the html_elements
     free(map->keys);
     free(map->values);
     free(map);
 }
 
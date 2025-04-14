#ifndef HTML_H
#define HTML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct html_element
{
    char *id;
    char *tagname;
    char *content;
    struct html_element *parent;
    struct html_element **children;
    int children_count;
    int children_capacity;
    char *attributes;
} html_element;

typedef struct
{
    char **keys;
    html_element **values;
    int capacity;
    int size;
} id_map;

typedef struct html_context
{
    html_element *root;
    html_element *current;
    id_map *element_map;
    FILE *output_file;
    char *title;
    int indent_level;
} html_context;

char *html_strdup(const char *str);

char *html_add_attribute(const char *attributes, const char *name, const char *value);

char *html_extract_attribute(const char *attributes, const char *name);

char *html_extract_id(const char *attributes);

char *html_generate_indent(int level);

void html_set_error(const char *format, ...);

const char *html_get_error(void);

const char *html_get_last_error(void);

void html_clear_error(void);

id_map *html_create_id_map(int initial_capacity);

void html_free_id_map(id_map *map);

int html_resize_id_map(id_map *map);

unsigned int html_code_string(const char *str);

int html_is_valid_child(const char *parent_tag, const char *child_tag);

int html_is_block_element(const char *tagname);

int html_is_self_closing(const char *tagname);

html_element *html_find_head(html_context *ctx);

html_element *html_find_body(html_context *ctx);

int html_navigate_to_body(html_context *ctx);

int html_navigate_to_head(html_context *ctx);

int html_create_document_structure(html_context *ctx);

int html_render_element(html_context *ctx, html_element *element);

html_context *html_init_file(const char *filename, const char *title);

void html_finalize(html_context *ctx);

int html_add_style(html_context *ctx, const char *style_content);

int html_add_script(html_context *ctx, const char *script_content, int is_external);

int html_add_div(html_context *ctx, const char *attributes, const char *content);

int html_add_paragraph(html_context *ctx, const char *attributes, const char *content);

int html_add_heading(html_context *ctx, int level, const char *content, const char *attributes);

int html_begin_section(html_context *ctx, const char *attributes);

int html_end_section(html_context *ctx);

int html_add_image(html_context *ctx, const char *src, const char *alt, const char *attributes);

int html_add_anchor(html_context *ctx, const char *href, const char *content, const char *attributes);

int html_begin_unordered_list(html_context *ctx, const char *attributes);

int html_begin_ordered_list(html_context *ctx, const char *attributes);

int html_end_list(html_context *ctx);

int html_add_list_item(html_context *ctx, const char *content, const char *attributes);

html_element *html_get_element_by_id(html_context *ctx, const char *id);

int html_set_element_content(html_element *element, const char *content);

int html_set_element_attribute(html_element *element, const char *name, const char *value);

int html_add_class(html_element *element, const char *classname);

int html_add_form(html_context *ctx, const char *action, const char *method, const char *attributes);

int html_add_input(html_context *ctx, const char *type, const char *name, const char *value, const char *attributes);

int html_add_button(html_context *ctx, const char *type, const char *content, const char *attributes);

int html_begin_table(html_context *ctx, const char *attributes);

int html_end_table(html_context *ctx);

int html_begin_table_row(html_context *ctx, const char *attributes);

int html_end_table_row(html_context *ctx);

int html_add_table_cell(html_context *ctx, const char *content, const char *attributes, int is_header);

int html_add_meta(html_context *ctx, const char *name, const char *content);

int html_add_link(html_context *ctx, const char *rel, const char *href, const char *type);

int html_register_element_by_id(html_context *ctx, html_element *element);

html_element *html_add_child(html_context *ctx, html_element *parent, const char *tagname, const char *attributes, const char *content);

int html_render(html_context *ctx);

html_element *html_create_element(const char *tagname, const char *attributes, const char *content);

void html_free_element(html_element *element);

int html_set_current_element(html_context *ctx, html_element *element);

int html_add_content(html_context *ctx, const char *content);

int html_begin_tag(html_context *ctx, const char *tagname, const char *attributes);

int html_end_tag(html_context *ctx);

#endif

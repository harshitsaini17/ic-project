#include "HTML.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

html_context *html_init_file(const char *filename, const char *title)
{
    if (!filename)
    {
        html_set_error("Filename cannot be NULL");
        return NULL;
    }

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        html_set_error("Failed to open output file '%s'", filename);
        return NULL;
    }

    html_context *ctx = (html_context *)malloc(sizeof(html_context));
    if (!ctx)
    {
        fclose(file);
        html_set_error("Memory allocation failed for HTML context");
        return NULL;
    }

    memset(ctx, 0, sizeof(html_context));
    ctx->output_file = file;
    ctx->indent_level = 0;


    if (title)
    {
        ctx->title = html_strdup(title);
        if (!ctx->title)
        {
            fclose(file);
            free(ctx);
            return NULL;
        }
    }
    else
    {
        ctx->title = html_strdup("Untitled Document");
        if (!ctx->title)
        {
            fclose(file);
            free(ctx);
            return NULL;
        }
    }

    ctx->element_map = html_create_id_map(16);
    if (!ctx->element_map)
    {
        free(ctx->title);
        fclose(file);
        free(ctx);
        return NULL;
    }

    if (!html_create_document_structure(ctx))
    {
        html_finalize(ctx);
        return NULL;
    }

    return ctx;
}

int html_create_document_structure(html_context *ctx)
{
    if (!ctx)
        return 0;

    char *html_attrs = NULL;

    ctx->root = html_create_element("html", html_attrs, NULL);
    free(html_attrs);

    if (!ctx->root)
        return 0;

    html_element *head = html_add_child(ctx, ctx->root, "head", NULL, NULL);
    if (!head)
    {
        html_free_element(ctx->root);
        ctx->root = NULL;
        return 0;
    }

    if (ctx->title)
    {
        html_element *title = html_add_child(ctx, head, "title", NULL, ctx->title);
        if (!title)
        {
            html_free_element(ctx->root);
            ctx->root = NULL;
            return 0;
        }
    }

    html_element *body = html_add_child(ctx, ctx->root, "body", NULL, NULL);
    if (!body)
    {
        html_free_element(ctx->root);
        ctx->root = NULL;
        return 0;
    }

    ctx->current = body;

    return 1;
}

void html_finalize(html_context *ctx)
{
    if (!ctx)
        return;

    if (ctx->output_file && ctx->root)
    {
        html_render(ctx);
    }

    if (ctx->root)
    {
        html_free_element(ctx->root);
        ctx->root = NULL;
    }

    if (ctx->element_map)
    {
        html_free_id_map(ctx->element_map);
        ctx->element_map = NULL;
    }

    if (ctx->output_file)
    {
        fclose(ctx->output_file);
        ctx->output_file = NULL;
    }
    free(ctx->title);

    free(ctx);
}

int html_register_element_by_id(html_context *ctx, html_element *element)
{
    if (!ctx || !ctx->element_map || !element || !element->id)
        return -1;

    if (ctx->element_map->size >= ctx->element_map->capacity * 0.75)
    {
        if (html_resize_id_map(ctx->element_map) != 0)
        {
            return -1;
        }
    }

    unsigned int hash = html_hash_string(element->id) % ctx->element_map->capacity;

    while (ctx->element_map->keys[hash] != NULL)
    {
        if (strcmp(ctx->element_map->keys[hash], element->id) == 0)
        {
            html_set_error("Duplicate element ID: '%s'", element->id);
            return -1;
        }
        hash = (hash + 1) % ctx->element_map->capacity;
    }

    ctx->element_map->keys[hash] = element->id;
    ctx->element_map->values[hash] = element;
    ctx->element_map->size++;

    return 0;
}

html_element *html_get_element_by_id(html_context *ctx, const char *id)
{
    if (!ctx || !ctx->element_map || !id)
        return NULL;

    unsigned int hash = html_hash_string(id) % ctx->element_map->capacity;

    int i = 0;
    while (ctx->element_map->keys[hash] != NULL && i < ctx->element_map->capacity)
    {
        if (ctx->element_map->keys[hash] && strcmp(ctx->element_map->keys[hash], id) == 0)
        {
            return ctx->element_map->values[hash];
        }
        hash = (hash + 1) % ctx->element_map->capacity;
        i++;
    }

    return NULL;
}

int html_set_current_element(html_context *ctx, html_element *element)
{
    if (!ctx || !element)
        return -1;

    html_element *current = element;
    while (current->parent && current != ctx->root)
    {
        current = current->parent;
    }

    if (current != ctx->root)
    {
        html_set_error("Element does not belong to this context");
        return -1;
    }

    ctx->current = element;
    return 0;
}

int html_add_style(html_context *ctx, const char *style_content)
{
    if (!ctx || !ctx->root || !style_content)
        return -1;

    html_element *head = html_find_head(ctx);
    if (!head)
    {
        html_set_error("Could not find head element");
        return -1;
    }

    html_element *saved_current = ctx->current;

    ctx->current = head;

    html_element *style = html_add_child(ctx, head, "style", NULL, style_content);

    ctx->current = saved_current;

    return style ? 0 : -1;
}

int html_add_script(html_context *ctx, const char *script_content, int is_external)
{
    if (!ctx || !ctx->root || !script_content)
        return -1;

    html_element *head = html_find_head(ctx);
    if (!head)
    {
        html_set_error("Could not find head element");
        return -1;
    }

    html_element *saved_current = ctx->current;

    ctx->current = head;

    html_element *script;
    if (is_external)
    {
        char *attrs = html_add_attribute(NULL, "src", script_content);
        script = html_add_child(ctx, head, "script", attrs, NULL);
        free(attrs);
    }
    else
    {
        script = html_add_child(ctx, head, "script", NULL, script_content);
    }

    ctx->current = saved_current;

    return script ? 0 : -1;
}

int html_add_meta(html_context *ctx, const char *name, const char *content)
{
    if (!ctx || !ctx->root || !name || !content)
        return -1;

    html_element *head = html_find_head(ctx);
    if (!head)
    {
        html_set_error("Could not find head element");
        return -1;
    }

    html_element *saved_current = ctx->current;

    ctx->current = head;

    char *attrs = html_add_attribute(NULL, "name", name);
    if (!attrs)
    {
        ctx->current = saved_current;
        return -1;
    }

    char *attrs_with_content = html_add_attribute(attrs, "content", content);
    free(attrs);

    if (!attrs_with_content)
    {
        ctx->current = saved_current;
        return -1;
    }

    html_element *meta = html_add_child(ctx, head, "meta", attrs_with_content, NULL);
    free(attrs_with_content);

    ctx->current = saved_current;

    return meta ? 0 : -1;
}

int html_add_link(html_context *ctx, const char *rel, const char *href, const char *type)
{
    if (!ctx || !ctx->root || !rel || !href)
        return -1;

    html_element *head = html_find_head(ctx);
    if (!head)
    {
        html_set_error("Could not find head element");
        return -1;
    }

    html_element *saved_current = ctx->current;

    ctx->current = head;

    char *attrs = html_add_attribute(NULL, "rel", rel);
    if (!attrs)
    {
        ctx->current = saved_current;
        return -1;
    }

    char *attrs_with_href = html_add_attribute(attrs, "href", href);
    free(attrs);

    if (!attrs_with_href)
    {
        ctx->current = saved_current;
        return -1;
    }

    char *final_attrs = attrs_with_href;
    if (type)
    {
        final_attrs = html_add_attribute(attrs_with_href, "type", type);
        free(attrs_with_href);

        if (!final_attrs)
        {
            ctx->current = saved_current;
            return -1;
        }
    }

    html_element *link = html_add_child(ctx, head, "link", final_attrs, NULL);
    free(final_attrs);

    ctx->current = saved_current;

    return link ? 0 : -1;
}

int html_render(html_context *ctx)
{
    if (!ctx || !ctx->root || !ctx->output_file)
        return -1;

    fprintf(ctx->output_file, "<!DOCTYPE html>\n");

    ctx->indent_level = 0;
    return html_render_element(ctx, ctx->root);
}

int html_render_element(html_context *ctx, html_element *element)
{
    if (!ctx || !element || !ctx->output_file)
        return -1;

    char *indent = html_generate_indent(ctx->indent_level);
    if (!indent)
        return -1;

    fprintf(ctx->output_file, "%s<%s", indent, element->tagname);

    if (element->attributes && strlen(element->attributes) > 0)
    {
        fprintf(ctx->output_file, " %s", element->attributes);
    }

    if (html_is_self_closing(element->tagname))
    {
        fprintf(ctx->output_file, " />\n");
        free(indent);
        return 0;
    }

    fprintf(ctx->output_file, ">");

    int is_block = html_is_block_element(element->tagname);

    if (element->content && strlen(element->content) > 0)
    {
        if (is_block)
        {
            fprintf(ctx->output_file, "\n%s  ", indent);
        }

        fprintf(ctx->output_file, "%s", element->content);

        if (is_block)
        {
            fprintf(ctx->output_file, "\n%s", indent);
        }
    }
    else if (element->children_count > 0)
    {
        fprintf(ctx->output_file, "\n");

        ctx->indent_level++;

        for (int i = 0; i < element->children_count; i++)
        {
            html_render_element(ctx, element->children[i]);
        }

        ctx->indent_level--;

        fprintf(ctx->output_file, "%s", indent);
    }

    fprintf(ctx->output_file, "</%s>\n", element->tagname);

    free(indent);
    return 0;
}
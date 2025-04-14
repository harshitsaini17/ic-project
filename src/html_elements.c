#include "HTML.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

html_element *html_create_element(const char *tagname, const char *attributes, const char *content)
{
    if (!tagname)
        return NULL;

    html_element *element = (html_element *)malloc(sizeof(html_element));
    if (!element)
    {
        html_set_error("Memory allocation failed for HTML element");
        return NULL;
    }

    memset(element, 0, sizeof(html_element));

    element->tagname = html_strdup(tagname);
    if (!element->tagname)
    {
        free(element);
        return NULL;
    }

    if (content)
    {
        element->content = html_strdup(content);
        if (!element->content)
        {
            free(element->tagname);
            free(element);
            return NULL;
        }
    }

    if (attributes)
    {
        element->attributes = html_strdup(attributes);
        if (!element->attributes)
        {
            free(element->content);
            free(element->tagname);
            free(element);
            return NULL;
        }

        element->id = html_extract_id(attributes);
    }

    element->children_capacity = 4;
    element->children = (html_element **)malloc(element->children_capacity * sizeof(html_element *));
    if (!element->children)
    {
        free(element->id);
        free(element->attributes);
        free(element->content);
        free(element->tagname);
        free(element);
        html_set_error("Memory allocation failed for children array");
        return NULL;
    }

    memset(element->children, 0, element->children_capacity * sizeof(html_element *));

    return element;
}

html_element *html_add_child(html_context *ctx, html_element *parent, const char *tagname, const char *attributes, const char *content)
{
    if (!ctx || !parent || !tagname)
        return NULL;

    if (!html_is_valid_child(parent->tagname, tagname))
    {
        html_set_error("Invalid child tag '%s' for parent '%s'", tagname, parent->tagname);
        return NULL;
    }

    html_element *child = html_create_element(tagname, attributes, content);
    if (!child)
        return NULL;

    child->parent = parent;

    if (parent->children_count >= parent->children_capacity)
    {
        int new_capacity = parent->children_capacity * 2;
        html_element **new_children = (html_element **)realloc(parent->children,
                                                               new_capacity * sizeof(html_element *));

        if (!new_children)
        {
            html_free_element(child);
            html_set_error("Memory allocation failed for resizing children array");
            return NULL;
        }

        for (int i = parent->children_capacity; i < new_capacity; i++)
        {
            new_children[i] = NULL;
        }

        parent->children = new_children;
        parent->children_capacity = new_capacity;
    }

    parent->children[parent->children_count++] = child;

    if (child->id)
    {
        html_register_element_by_id(ctx, child);
    }

    return child;
}

void html_free_element(html_element *element)
{
    if (!element)
        return;

    for (int i = 0; i < element->children_count; i++)
    {
        html_free_element(element->children[i]);
    }

    free(element->children);

    free(element->id);
    free(element->tagname);
    free(element->content);
    free(element->attributes);

    free(element);
}

int html_set_element_content(html_element *element, const char *content)
{
    if (!element)
        return -1;

    free(element->content);

    if (content)
    {
        element->content = html_strdup(content);
        if (!element->content)
        {
            html_set_error("Memory allocation failed for element content");
            return -1;
        }
    }
    else
    {
        element->content = NULL;
    }

    return 0;
}

int html_set_element_attribute(html_element *element, const char *name, const char *value)
{
    if (!element || !name || !value)
        return -1;

    char *new_attributes = html_add_attribute(element->attributes, name, value);
    if (!new_attributes)
    {
        return -1;
    }

    free(element->attributes);

    element->attributes = new_attributes;

    if (strcmp(name, "id") == 0)
    {
        free(element->id);
        element->id = html_strdup(value);
        if (!element->id)
        {
            html_set_error("Memory allocation failed for element ID");
            return -1;
        }
    }

    return 0;
}

int html_add_class(html_element *element, const char *classname)
{
    if (!element || !classname)
        return -1;

    char *current_class = html_extract_attribute(element->attributes, "class");

    char *new_class = NULL;
    if (current_class)
    {

        size_t total_len = strlen(current_class) + strlen(classname) + 2;
        new_class = (char *)malloc(total_len);
        if (!new_class)
        {
            free(current_class);
            html_set_error("Memory allocation failed for class attribute");
            return -1;
        }

        sprintf(new_class, "%s %s", current_class, classname);
        free(current_class);
    }
    else
    {

        new_class = html_strdup(classname);
        if (!new_class)
        {
            html_set_error("Memory allocation failed for class attribute");
            return -1;
        }
    }

    int result = html_set_element_attribute(element, "class", new_class);
    free(new_class);

    return result;
}

int html_add_div(html_context *ctx, const char *attributes, const char *content)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *div = html_add_child(ctx, ctx->current, "div", attributes, content);
    return div ? 0 : -1;
}

int html_add_paragraph(html_context *ctx, const char *attributes, const char *content)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *p = html_add_child(ctx, ctx->current, "p", attributes, content);
    return p ? 0 : -1;
}

int html_add_heading(html_context *ctx, int level, const char *content, const char *attributes)
{
    if (!ctx || !ctx->current || level < 1 || level > 6)
        return -1;

    char tagname[4] = "h";
    tagname[1] = '0' + level;
    tagname[2] = '\0';

    html_element *heading = html_add_child(ctx, ctx->current, tagname, attributes, content);
    return heading ? 0 : -1;
}

int html_begin_section(html_context *ctx, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *section = html_add_child(ctx, ctx->current, "div", attributes, NULL);
    if (!section)
        return -1;

    ctx->current = section;
    return 0;
}

int html_end_section(html_context *ctx)
{
    if (!ctx || !ctx->current || !ctx->current->parent)
        return -1;

    ctx->current = ctx->current->parent;
    return 0;
}

int html_add_image(html_context *ctx, const char *src, const char *alt, const char *attributes)
{
    if (!ctx || !ctx->current || !src)
        return -1;

    char *combined_attrs = NULL;

    char *temp = html_add_attribute(NULL, "src", src);
    if (!temp)
        return -1;
    combined_attrs = temp;

    if (alt)
    {
        temp = html_add_attribute(combined_attrs, "alt", alt);
        free(combined_attrs);
        if (!temp)
            return -1;
        combined_attrs = temp;
    }

    if (attributes)
    {
        temp = (char *)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
        if (!temp)
        {
            free(combined_attrs);
            html_set_error("Memory allocation failed for combined attributes");
            return -1;
        }
        sprintf(temp, "%s %s", combined_attrs, attributes);
        free(combined_attrs);
        combined_attrs = temp;
    }

    html_element *img = html_add_child(ctx, ctx->current, "img", combined_attrs, NULL);
    free(combined_attrs);

    return img ? 0 : -1;
}

int html_add_anchor(html_context *ctx, const char *href, const char *content, const char *attributes)
{
    if (!ctx || !ctx->current || !href)
        return -1;

    char *combined_attrs = html_add_attribute(NULL, "href", href);
    if (!combined_attrs)
        return -1;

    if (attributes)
    {
        char *temp = (char *)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
        if (!temp)
        {
            free(combined_attrs);
            html_set_error("Memory allocation failed for combined attributes");
            return -1;
        }
        sprintf(temp, "%s %s", combined_attrs, attributes);
        free(combined_attrs);
        combined_attrs = temp;
    }

    html_element *anchor = html_add_child(ctx, ctx->current, "a", combined_attrs, content);
    free(combined_attrs);

    return anchor ? 0 : -1;
}

int html_begin_unordered_list(html_context *ctx, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *ul = html_add_child(ctx, ctx->current, "ul", attributes, NULL);
    if (!ul)
        return -1;

    ctx->current = ul;
    return 0;
}

int html_begin_ordered_list(html_context *ctx, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *ol = html_add_child(ctx, ctx->current, "ol", attributes, NULL);
    if (!ol)
        return -1;

    ctx->current = ol;
    return 0;
}

int html_end_list(html_context *ctx)
{
    if (!ctx || !ctx->current || !ctx->current->parent)
        return -1;

    if (strcmp(ctx->current->tagname, "ul") != 0 && strcmp(ctx->current->tagname, "ol") != 0)
    {
        html_set_error("Current element is not a list");
        return -1;
    }

    ctx->current = ctx->current->parent;
    return 0;
}

int html_add_list_item(html_context *ctx, const char *content, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    if (strcmp(ctx->current->tagname, "ul") != 0 && strcmp(ctx->current->tagname, "ol") != 0)
    {
        html_set_error("Current element is not a list");
        return -1;
    }

    html_element *li = html_add_child(ctx, ctx->current, "li", attributes, content);
    return li ? 0 : -1;
}

int html_begin_table(html_context *ctx, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    html_element *table = html_add_child(ctx, ctx->current, "table", attributes, NULL);
    if (!table)
        return -1;

    ctx->current = table;
    return 0;
}

int html_end_table(html_context *ctx)
{
    if (!ctx || !ctx->current || !ctx->current->parent)
        return -1;

    if (strcmp(ctx->current->tagname, "table") != 0)
    {
        html_set_error("Current element is not a table");
        return -1;
    }

    ctx->current = ctx->current->parent;
    return 0;
}

int html_begin_table_row(html_context *ctx, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    if (strcmp(ctx->current->tagname, "table") != 0 &&
        strcmp(ctx->current->tagname, "tbody") != 0 &&
        strcmp(ctx->current->tagname, "thead") != 0 &&
        strcmp(ctx->current->tagname, "tfoot") != 0)
    {
        html_set_error("Current element cannot contain table rows");
        return -1;
    }

    html_element *tr = html_add_child(ctx, ctx->current, "tr", attributes, NULL);
    if (!tr)
        return -1;

    ctx->current = tr;
    return 0;
}

int html_end_table_row(html_context *ctx)
{
    if (!ctx || !ctx->current || !ctx->current->parent)
        return -1;

    if (strcmp(ctx->current->tagname, "tr") != 0)
    {
        html_set_error("Current element is not a table row");
        return -1;
    }

    ctx->current = ctx->current->parent;
    return 0;
}

int html_add_table_cell(html_context *ctx, const char *content, const char *attributes, int is_header)
{
    if (!ctx || !ctx->current)
        return -1;

    if (strcmp(ctx->current->tagname, "tr") != 0)
    {
        html_set_error("Current element is not a table row");
        return -1;
    }

    const char *tagname = is_header ? "th" : "td";
    html_element *cell = html_add_child(ctx, ctx->current, tagname, attributes, content);

    return cell ? 0 : -1;
}

int html_add_form(html_context *ctx, const char *action, const char *method, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    char *combined_attrs = html_add_attribute(NULL, "action", action ? action : "");
    if (!combined_attrs)
        return -1;

    char *temp = html_add_attribute(combined_attrs, "method", method ? method : "get");
    free(combined_attrs);
    if (!temp)
        return -1;
    combined_attrs = temp;

    if (attributes)
    {
        temp = (char *)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
        if (!temp)
        {
            free(combined_attrs);
            html_set_error("Memory allocation failed for combined attributes");
            return -1;
        }
        sprintf(temp, "%s %s", combined_attrs, attributes);
        free(combined_attrs);
        combined_attrs = temp;
    }

    html_element *form = html_add_child(ctx, ctx->current, "form", combined_attrs, NULL);
    free(combined_attrs);

    if (!form)
        return -1;

    ctx->current = form;
    return 0;
}

int html_end_form(html_context *ctx)
{
    if (!ctx || !ctx->current || !ctx->current->parent)
        return -1;

    if (strcmp(ctx->current->tagname, "form") != 0)
    {
        html_set_error("Current element is not a form");
        return -1;
    }

    ctx->current = ctx->current->parent;
    return 0;
}

int html_add_input(html_context *ctx, const char *type, const char *name, const char *value, const char *attributes)
{
    if (!ctx || !ctx->current || !type)
        return -1;

    char *combined_attrs = html_add_attribute(NULL, "type", type);
    if (!combined_attrs)
        return -1;

    if (name)
    {
        char *temp = html_add_attribute(combined_attrs, "name", name);
        free(combined_attrs);
        if (!temp)
            return -1;
        combined_attrs = temp;
    }

    if (value)
    {
        char *temp = html_add_attribute(combined_attrs, "value", value);
        free(combined_attrs);
        if (!temp)
            return -1;
        combined_attrs = temp;
    }

    if (attributes)
    {
        char *temp = (char *)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
        if (!temp)
        {
            free(combined_attrs);
            html_set_error("Memory allocation failed for combined attributes");
            return -1;
        }
        sprintf(temp, "%s %s", combined_attrs, attributes);
        free(combined_attrs);
        combined_attrs = temp;
    }

    html_element *input = html_add_child(ctx, ctx->current, "input", combined_attrs, NULL);
    free(combined_attrs);

    return input ? 0 : -1;
}

int html_add_button(html_context *ctx, const char *type, const char *content, const char *attributes)
{
    if (!ctx || !ctx->current)
        return -1;

    char *combined_attrs = NULL;

    if (type)
    {
        combined_attrs = html_add_attribute(NULL, "type", type);
        if (!combined_attrs)
            return -1;
    }

    if (attributes)
    {
        if (combined_attrs)
        {
            char *temp = (char *)malloc(strlen(combined_attrs) + strlen(attributes) + 2);
            if (!temp)
            {
                free(combined_attrs);
                html_set_error("Memory allocation failed for combined attributes");
                return -1;
            }
            sprintf(temp, "%s %s", combined_attrs, attributes);
            free(combined_attrs);
            combined_attrs = temp;
        }
        else
        {
            combined_attrs = html_strdup(attributes);
            if (!combined_attrs)
                return -1;
        }
    }

    html_element *button = html_add_child(ctx, ctx->current, "button", combined_attrs, content);
    free(combined_attrs);

    return button ? 0 : -1;
}

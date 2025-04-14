#include "HTML.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

//////////////error handling functions///////////////////////
static char error_message[500] = {0};

void html_set_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(error_message, sizeof(error_message), format, args);
    va_end(args);
}

const char *html_get_error()
{
    return error_message;
}

void html_clear_error() { error_message[0] = '\0'; }

//////////////string utilities functions///////////////////////
char *html_strdup(const char *str)
{
    int len = strlen(str);
    char *new_str = (char *)malloc(len + 1);
    if (!new_str)
    {
        html_set_error("memory allocation failed for string duplication");
        return NULL;
    }
    memcpy(new_str, str, len + 1);
    return new_str;
}


char *html_strcat(char *dest, const char *src)
{
    int dest_len = strlen(dest);
    int src_len = strlen(src);
    int new_len = dest_len + src_len;
    char *new_str = (char *)realloc(dest, new_len + 1);
    if (!new_str)
        return dest;

    memcpy(new_str + dest_len, src, src_len + 1);
    return new_str;
}

char *html_escape_string(const char *str)
{
    if (!str)
        return NULL;
    int len = strlen(str), new_len = len;

    for (int i = 0; i < len; i++)
    {
        switch (str[i])
        {
        case '&':
            new_len += 4;
            break;
        case '<':
            new_len += 3;
            break;
        case '>':
            new_len += 3;
            break;
        case '"':
            new_len += 5;
            break;
        case '\'':
            new_len += 4;
            break;
        }
    }

    if (new_len == len)
        return html_strdup(str);
    char *new_str = (char *)malloc(new_len + 1);
    if (!new_str)
    {
        html_set_error("memory allocation failed for string escaping");
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < len; i++)
    {
        switch (str[i])
        {
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
        }
    }
    new_str[j] = '\0';
    return new_str;
}

char *html_trim_string(char *str)
{
    if (!str)
        return NULL;
    char *start = str;
    while (isspace((unsigned char)*start))
        start++;
    if (start != str)
        memmove(str, start, strlen(start) + 1);

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';
    return str;
}

/////////////////html element functions///////////////////////

char *html_extract_attribute(const char *attributes, const char *name)
{
    if (!attributes || !name)
        return NULL;
    int name_len = strlen(name);
    const char *attr_start = attributes;
    while ((attr_start = strstr(attr_start, name)) != NULL)
    {

        if (attr_start == attributes || isspace((unsigned char)*(attr_start - 1)))
        {
            attr_start += name_len;
            while (isspace((unsigned char)*attr_start))
                attr_start++;

            if (*attr_start == '=')
            {
                attr_start++;

                while (isspace((unsigned char)*attr_start))
                    attr_start++;

                if (*attr_start == '"' || *attr_start == '\'')
                {
                    char quote = *attr_start++;
                    const char *value_end = strchr(attr_start, quote);

                    if (value_end)
                    {
                        int value_len = value_end - attr_start;
                        char *value = malloc(value_len + 1);

                        if (!value)
                        {
                            html_set_error("memory allocation failed for attribute extraction");
                            return NULL;
                        }

                        memcpy(value, attr_start, value_len);
                        value[value_len] = '\0';
                        return value;
                    }
                }
                else
                {
                    const char *value_end = attr_start;
                    while (*value_end && !isspace((unsigned char)*value_end))
                        value_end++;

                    int value_len = value_end - attr_start;
                    char *value = malloc(value_len + 1);

                    if (!value)
                    {
                        html_set_error("memory allocation failed for attribute extraction");
                        return NULL;
                    }

                    memcpy(value, attr_start, value_len);
                    value[value_len] = '\0';
                    return value;
                }
            }
        }
        attr_start++;
    }
    return NULL;
}

char *html_extract_id(const char *attributes)
{

    return html_extract_attribute(attributes, "id");
}

char *html_add_attribute(const char *attributes, const char *name, const char *value)
{
    if (!name || !value)
        return html_strdup(attributes ? attributes : "");

    int attr_len = attributes ? strlen(attributes) : 0;
    int name_len = strlen(name);
    int value_len = strlen(value);
    int new_attr_len = attr_len + name_len + value_len + 4;

    char *new_attributes = (char *)malloc(new_attr_len + 1);
    if (!new_attributes)
    {
        html_set_error("Memory allocation failed for attribute addition");
        return NULL;
    }

    if (attributes && attr_len > 0)
    {
        strcpy(new_attributes, attributes);
        if (new_attributes[attr_len - 1] != ' ')
            strcat(new_attributes, " ");
    }
    else
    {
        new_attributes[0] = '\0';
    }

    strcat(new_attributes, name);
    strcat(new_attributes, "=\"");
    strcat(new_attributes, value);
    strcat(new_attributes, "\"");

    return new_attributes;
}

html_element *html_find_head(html_context *ctx)
{
    if (!ctx || !ctx->root)
        return NULL;
    for (int i = 0; i < ctx->root->children_count; i++)
    {
        if (ctx->root->children[i] && ctx->root->children[i]->tagname && strcmp(ctx->root->children[i]->tagname, "head") == 0)
        {

            return ctx->root->children[i];
        }
    }

    return NULL;
}

html_element *html_find_body(html_context *ctx)
{
    if (!ctx || !ctx->root)
        return NULL;
    for (int i = 0; i < ctx->root->children_count; i++)
    {

        if (ctx->root->children[i] && ctx->root->children[i]->tagname && strcmp(ctx->root->children[i]->tagname, "body") == 0)
        {
            return ctx->root->children[i];
        }
    }

    return NULL;
}

int html_is_valid_child(const char *parent_tag, const char *child_tag)
{
    if (!parent_tag || !child_tag)
        return 0;
    // for head
    if (strcmp(parent_tag, "head") == 0)
    {
        return (strcmp(child_tag, "meta") == 0 ||
                strcmp(child_tag, "title") == 0 ||
                strcmp(child_tag, "link") == 0 ||
                strcmp(child_tag, "style") == 0 ||
                strcmp(child_tag, "script") == 0);
    }
    // for table
    if (strcmp(parent_tag, "table") == 0)
    {
        return (strcmp(child_tag, "thead") == 0 ||
                strcmp(child_tag, "tbody") == 0 ||
                strcmp(child_tag, "tfoot") == 0 ||
                strcmp(child_tag, "tr") == 0 ||
                strcmp(child_tag, "caption") == 0);
    }

    // for table
    if (strcmp(parent_tag, "tr") == 0)
    {
        return (strcmp(child_tag, "th") == 0 || strcmp(child_tag, "td") == 0);
    }

    // for list
    if (strcmp(parent_tag, "ul") == 0 || strcmp(parent_tag, "ol") == 0)
    {
        return (strcmp(child_tag, "li") == 0);
    }

    return 1;
}

char *html_generate_indent(int level)
{
    const int spaces_per_level = 2;

    int total_spaces = level * spaces_per_level;
    if (total_spaces > 40)
        total_spaces = 40;

    char *indent = (char *)malloc(total_spaces + 1);
    if (!indent)
    {
        html_set_error("memory allocation failed for indentation");
        return NULL;
    }

    memset(indent, ' ', total_spaces);
    indent[total_spaces] = '\0';

    return indent;
}

int html_is_block_element(const char *tagname)
{
    if (!tagname)
        return 0;
    const char *block_elements[] = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "fieldset", "header", "footer",
        "section", "article", "aside", "nav", "main", NULL};

    for (int i = 0; block_elements[i]; i++)
    {
        if (strcmp(tagname, block_elements[i]) == 0)
            return 1;
    }

    return 0;
}

int html_is_self_closing(const char *tagname)
{
    if (!tagname)
        return 0;

    const char *self_closing[] = {
        "area", "base", "br", "col", "embed", "hr", "img",
        "input", "link", "meta", "param", "source", "track", "wbr", NULL};

    for (int i = 0; self_closing[i]; i++)
    {

        if (strcmp(tagname, self_closing[i]) == 0)
            return 1;
    }

    return 0;
}

unsigned int html_code_string(const char *str)
{
    if (!str)
        return 0;

    int index = 0;
    while (*str)
    {
        index = +*str++;
    }

    return index;
}

int html_resize_id_map(id_map *map)
{
    if (!map)
        return -1;

    int new_capacity = map->capacity * 2;
    if (new_capacity < 8)
        new_capacity = 8;

    char **new_keys = (char **)calloc(new_capacity, sizeof(char *));
    html_element **new_values = (html_element **)calloc(new_capacity, sizeof(html_element *));

    if (!new_keys || !new_values)
    {
        free(new_keys);
        free(new_values);
        html_set_error("memory allocation failed for Id map resize");
        return -1;
    }

    for (int i = 0; i < map->capacity; i++)
    {
        if (map->keys[i])
        {
            unsigned int index = html_code_string(map->keys[i]) % new_capacity;
            while (new_keys[index])
            {
                index = (index + 1) % new_capacity;
            }
            new_keys[index] = map->keys[i];
            new_values[index] = map->values[i];
        }
    }

    free(map->keys);
    free(map->values);
    map->keys = new_keys;
    map->values = new_values;
    map->capacity = new_capacity;
    return 0;
}

id_map *html_create_id_map(int initial_capacity)
{
    if (initial_capacity < 4)
        initial_capacity = 4;

    id_map *map = (id_map *)malloc(sizeof(id_map));

    if (!map)
    {
        html_set_error("memory allocation failed for Id map");
        return NULL;
    }

    map->keys = (char **)calloc(initial_capacity, sizeof(char *));
    map->values = (html_element **)calloc(initial_capacity, sizeof(html_element *));

    if (!map->keys || !map->values)
    {
        free(map->keys);
        free(map->values);
        free(map);
        html_set_error("memory allocation failed for Id map arrays");
        return NULL;
    }

    map->capacity = initial_capacity;
    map->size = 0;
    return map;
}

void html_free_id_map(id_map *map)
{
    if (!map)
        return;

    free(map->keys);
    free(map->values);
    free(map);
}

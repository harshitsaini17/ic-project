#include <stdio.h>
#include <stdlib.h>
#include "HTML.h"

int main()
{
    html_context *ctx = html_init_file("simple_output.html", "Simple HTML Example");
    if (!ctx)
    {
        fprintf(stderr, "Failed to initialize HTML context: %s\n", html_get_last_error());
        return 1;
    }

    html_add_style(ctx, "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }\n"
                        "h1 { color: #2c3e50; }\n"
                        "p { line-height: 1.6; }\n"
                        ".container { max-width: 800px; margin: 0 auto; padding: 20px; }\n"
                        ".footer { margin-top: 30px; padding-top: 10px; border-top: 1px solid #eee; color: #7f8c8d; }");

    html_add_meta(ctx, "viewport", "width=device-width, initial-scale=1.0");

    html_navigate_to_body(ctx);

    html_add_div(ctx, "class='container'", "");

    html_begin_section(ctx, "id='main-content'");

    html_add_heading(ctx, 1, "Welcome to HTML Generation Library", "");

    html_add_paragraph(ctx, "id='intro'",
                       "This is a simple example demonstrating the HTML generation library. "
                       "The library allows you to programmatically create HTML documents with "
                       "proper structure and formatting.");

    html_add_div(ctx, "class='highlight'", "");
    html_begin_section(ctx, "class='highlight'");

    html_add_heading(ctx, 2, "Features Demonstrated", "");

    html_begin_unordered_list(ctx, "");

    html_add_list_item(ctx, "Creating HTML document structure", "");
    html_add_list_item(ctx, "Adding CSS styling", "");
    html_add_list_item(ctx, "Creating nested elements", "");
    html_add_list_item(ctx, "Adding paragraphs and headings", "");
    html_add_list_item(ctx, "Using classes and IDs", "");

    html_end_list(ctx);

    html_end_section(ctx);

    html_add_paragraph(ctx, "",
                       "The library maintains proper HTML structure and automatically handles "
                       "element nesting and indentation for readable output.");

    html_add_anchor(ctx, "https://github.com/your-username/html-gen-lib",
                    "View project on GitHub", "class='github-link'");

    html_add_div(ctx, "class='footer'", "");
    html_begin_section(ctx, "class='footer'");

    html_add_paragraph(ctx, "",
                       "Created with the HTML Generation Library in C. &copy; 2025");

    html_end_section(ctx);

    html_end_section(ctx);

    html_element *intro = html_get_element_by_id(ctx, "intro");
    if (intro)
    {
        html_add_class(intro, "important-text");
    }

    html_finalize(ctx);

    printf("HTML document successfully generated and saved to 'simple_output.html'\n");

    return 0;
}

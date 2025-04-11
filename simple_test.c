/**
 * @file simple.c
 * @brief Simple example demonstrating basic usage of the HTML generation library
 *
 * This example creates a simple HTML page with various elements including
 * headings, paragraphs, divs, and demonstrates basic styling and nesting.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "HTML.h"
 
 int main(int argc, char *argv[]) {
     // Initialize HTML context with output file, language, and title
     html_context* ctx = html_init_file("simple_output.html", "en", "Simple HTML Example");
     if (!ctx) {
         fprintf(stderr, "Failed to initialize HTML context: %s\n", html_get_last_error());
         return 1;
     }
 
     // Add CSS styling to the document
     html_add_style(ctx, "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }\n"
                        "h1 { color: #2c3e50; }\n"
                        "p { line-height: 1.6; }\n"
                        ".container { max-width: 800px; margin: 0 auto; padding: 20px; }\n"
                        ".highlight { background-color: #f9f9f9; padding: 15px; border-left: 4px solid #2c3e50; }\n"
                        ".footer { margin-top: 30px; padding-top: 10px; border-top: 1px solid #eee; color: #7f8c8d; }");
 
     // Add a meta tag for viewport
     html_add_meta(ctx, "viewport", "width=device-width, initial-scale=1.0");
 
     // Navigate to body element
     html_navigate_to_body(ctx);
 
     // Create a container div
     html_add_div(ctx, "class='container'", "");
 
     // Navigate to the container div (using the fact that it's now the current element)
     html_begin_section(ctx, "id='main-content'");
     
     // Add a heading
     html_add_heading(ctx, 1, "Welcome to HTML Generation Library","");
     
     // Add an introductory paragraph
     html_add_paragraph(ctx, "id='intro'", 
         "This is a simple example demonstrating the HTML generation library. "
         "The library allows you to programmatically create HTML documents with "
         "proper structure and formatting.");
     
     // Create a highlighted section
     html_add_div(ctx, "class='highlight'", "");
     html_begin_section(ctx, "class='highlight'");
     
     html_add_heading(ctx, 2, "Features Demonstrated","");
     
     // Begin an unordered list
     html_begin_unordered_list(ctx, "");
     
     // Add list items
     html_add_list_item(ctx, "Creating HTML document structure", "");
     html_add_list_item(ctx, "Adding CSS styling", "");
     html_add_list_item(ctx, "Creating nested elements", "");
     html_add_list_item(ctx, "Adding paragraphs and headings", "");
     html_add_list_item(ctx, "Using classes and IDs", "");
     
     // End the list
     html_end_list(ctx);
     
     // End the highlighted section
     html_end_section(ctx);
     
     // Add another paragraph
     html_add_paragraph(ctx, "", 
         "The library maintains proper HTML structure and automatically handles "
         "element nesting and indentation for readable output.");
     
     // Add a link
     html_add_anchor(ctx, "https://github.com/your-username/html-gen-lib", 
                     "View project on GitHub", "class='github-link'");
     
     // Create a footer
     html_add_div(ctx, "class='footer'", "");
     html_begin_section(ctx, "class='footer'");
     
     html_add_paragraph(ctx, "", 
         "Created with the HTML Generation Library in C. &copy; 2025");
     
     // End the footer
     html_end_section(ctx);
     
     // End the main content section
     html_end_section(ctx);
     
     // Find an element by ID and modify it
     html_element* intro = html_get_element_by_id(ctx, "intro");
     if (intro) {
         // Add a class to the intro paragraph
         html_add_class(intro, "important-text");
     }
     
     // Finalize and save the HTML document
     html_finalize(ctx);
     
     printf("HTML document successfully generated and saved to 'simple_output.html'\n");
     
     return 0;
 }
 
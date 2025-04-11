/**
 * @file complex_test.c
 * @brief Comprehensive test of the HTML generation library
 *
 * This example demonstrates advanced features of the HTML generation library
 * including tables, forms, JavaScript integration, complex nesting, and
 * programmatic content generation.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "HTML.h"
 
 // Function prototypes
 void generate_table(html_context* ctx, int rows, int cols);
 void create_contact_form(html_context* ctx);
 void create_navigation(html_context* ctx);
 void add_dynamic_content(html_context* ctx);
 void handle_error(const char* action);
 
 int main(int argc, char *argv[]) {
     printf("HTML Generation Library - Complex Test\n");
     printf("Version: %s\n\n", html_get_version());
     
     // Initialize HTML context with output file
     html_context* ctx = html_init_file("complex_output.html", "en", "Advanced HTML Example");
     if (!ctx) {
         handle_error("initialize HTML context");
         return 1;
     }
     
     // Add metadata
     html_add_meta(ctx, "viewport", "width=device-width, initial-scale=1.0");
     html_add_meta(ctx, "description", "A complex example demonstrating the HTML generation library capabilities");
     html_add_meta(ctx, "keywords", "HTML, C, library, generation, example");
     html_add_meta(ctx, "author", "HTML Generation Library");
     
     // Add external stylesheet
     html_add_link(ctx, "stylesheet", "https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css", "text/css");
     
     // Add custom CSS
     html_add_style(ctx, 
         "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; line-height: 1.6; color: #333; }\n"
         "header { background: linear-gradient(135deg, #6e8efb, #a777e3); color: white; padding: 2rem 0; }\n"
         ".container { max-width: 1200px; margin: 0 auto; padding: 0 20px; }\n"
         ".hero-text { font-size: 1.2rem; max-width: 600px; margin: 1rem 0; }\n"
         ".card { box-shadow: 0 4px 8px rgba(0,0,0,0.1); border-radius: 8px; padding: 20px; margin: 20px 0; }\n"
         ".feature-section { display: flex; flex-wrap: wrap; gap: 20px; justify-content: space-between; }\n"
         ".feature-card { flex: 1; min-width: 300px; background-color: #f9f9f9; }\n"
         ".highlight { background-color: #fffde7; border-left: 4px solid #ffd600; padding: 15px; }\n"
         "table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n"
         "th, td { padding: 12px 15px; text-align: left; border-bottom: 1px solid #ddd; }\n"
         "th { background-color: #f2f2f2; }\n"
         "tr:hover { background-color: #f5f5f5; }\n"
         "footer { background-color: #2c3e50; color: white; padding: 40px 0; margin-top: 40px; }\n"
         ".form-group { margin-bottom: 15px; }\n"
         "label { display: block; margin-bottom: 5px; font-weight: bold; }\n"
         "input, textarea, select { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; }\n"
         "button { background-color: #4CAF50; color: white; border: none; padding: 10px 20px; cursor: pointer; }\n"
         "button:hover { background-color: #45a049; }\n"
     );
     
     // Add JavaScript in head
     html_add_script(ctx,
         "function validateForm() {\n"
         "  var name = document.getElementById('name').value;\n"
         "  var email = document.getElementById('email').value;\n"
         "  if (name === '' || email === '') {\n"
         "    alert('Name and email are required fields');\n"
         "    return false;\n"
         "  }\n"
         "  return true;\n"
         "}\n"
         "\n"
         "document.addEventListener('DOMContentLoaded', function() {\n"
         "  document.getElementById('currentYear').textContent = new Date().getFullYear();\n"
         "});\n",
         0 // 0 indicates inline script, not external
     );
     
     // Add external JavaScript
     html_add_script(ctx, "https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js", 1);
     
     // Navigate to body element
     html_navigate_to_body(ctx);
     
     // Create header
     html_begin_tag(ctx, "header", "");
     html_begin_tag(ctx, "div", "class='container'");
     html_add_heading(ctx, 1, "HTML Generation Library","");
     html_add_paragraph(ctx, "class='hero-text'", 
         "A powerful C library for programmatically generating structured HTML documents "
         "with proper element nesting, formatting, and attribute management.");
     html_end_tag(ctx); // container
     html_end_tag(ctx); // header
     
     // Create navigation menu
     create_navigation(ctx);
     
     // Main content section
     html_begin_tag(ctx, "main", "class='container'");
     
     // Introduction section
     html_begin_section(ctx, "id='intro' class='card'");
     html_add_heading(ctx, 2, "Introduction", "");
     html_add_paragraph(ctx, "", 
         "This example demonstrates the advanced capabilities of the HTML generation library, "
         "including complex element nesting, table generation, form creation, and dynamic content.");
     
     html_add_div(ctx, "class='highlight'", "");
     html_begin_section(ctx, "class='highlight'");
     html_add_paragraph(ctx, "style='font-weight: bold;'", 
         "This library enables C developers to generate HTML documents programmatically while ensuring "
         "proper structure, tag placement, and attribute management.");
     html_end_section(ctx); // highlight
     
     html_end_section(ctx); // intro section
     
     // Features section
     html_begin_section(ctx, "id='features' class='card'");
     html_add_heading(ctx, 2, "Key Features","");
     
     html_add_div(ctx, "class='feature-section'", "");
     html_begin_section(ctx, "class='feature-section'");
     
     // Feature cards
     const char* features[3][2] = {
         {"Proper HTML Structure", "Automatically maintains proper HTML document structure with correct nesting of elements."},
         {"Intelligent Tag Placement", "Places tags in the appropriate document sections (e.g., styles in head, divs in body)."},
         {"Memory Management", "Comprehensive memory management that prevents leaks and ensures clean resource handling."}
     };
     
     for (int i = 0; i < 3; i++) {
         html_add_div(ctx, "class='feature-card card'", "");
         html_begin_section(ctx, "class='feature-card'");
         html_add_heading(ctx, 3, features[i][0],"");
         html_add_paragraph(ctx, "", features[i][1]);
         html_end_section(ctx); // feature card
     }
     
     html_end_section(ctx); // feature section
     html_end_section(ctx); // features card
     
     // Data Table Section
     html_begin_section(ctx, "id='data-section' class='card'");
     html_add_heading(ctx, 2, "API Function Examples","");
     html_add_paragraph(ctx, "", "The following table shows key functions from the HTML generation library:");
     
     // Generate table programmatically
     generate_table(ctx, 5, 3);
     
     html_end_section(ctx); // data section
     
     // Form Section
     html_begin_section(ctx, "id='contact-section' class='card'");
     html_add_heading(ctx, 2, "Contact Form Example","");
     html_add_paragraph(ctx, "", "This demonstrates form generation capabilities:");
     
     create_contact_form(ctx);
     
     html_end_section(ctx); // contact section
     
     // Dynamically generated content
     html_begin_section(ctx, "id='dynamic-section' class='card'");
     html_add_heading(ctx, 2, "Dynamically Generated Content","");
     
     add_dynamic_content(ctx);
     
     html_end_section(ctx); // dynamic section
     
     html_end_tag(ctx); // main
     
     // Footer
     html_begin_tag(ctx, "footer", "");
     html_begin_tag(ctx, "div", "class='container'");
     html_add_heading(ctx, 3, "HTML Generation Library","");
     html_add_paragraph(ctx, "", "A comprehensive library for generating HTML documents with C.");
     html_add_paragraph(ctx, "", "Copyright &copy; <span id='currentYear'>2025</span> HTML Generation Library");
     html_end_tag(ctx); // container
     html_end_tag(ctx); // footer
     
     // Finalize and output
     html_finalize(ctx);
     printf("Complex HTML example successfully generated to 'complex_output.html'\n");
     
     return 0;
 }
 
 /**
  * Generate a table with specified rows and columns
  */
 void generate_table(html_context* ctx, int rows, int cols) {
     if (!ctx) return;
     
     // Function names and descriptions for the table
     const char* functions[][2] = {
         {"html_init_file", "Initializes an HTML document with file output"},
         {"html_add_div", "Adds a div element to the document"},
         {"html_begin_section", "Creates a new container and sets it as current"},
         {"html_add_heading", "Adds a heading (h1-h6) to the document"},
         {"html_add_style", "Adds CSS styling to the document head"},
         {"html_add_script", "Adds JavaScript to the document head"},
         {"html_add_meta", "Adds metadata to the document head"},
         {"html_add_form", "Creates a form element"},
         {"html_add_input", "Adds an input element to a form"},
         {"html_finalize", "Renders the document and releases resources"}
     };
     
     // Table headers
     const char* headers[] = {"Function", "Description", "Category"};
     const char* categories[] = {"Initialization", "Content", "Content", "Content", 
                                "Styling", "Scripting", "Metadata", "Forms", "Forms", "Finalization"};
     
     // Begin table
     html_begin_table(ctx, "class='function-table'");
     
     // Table header row
     html_begin_table_row(ctx, "");
     for (int i = 0; i < cols; i++) {
         html_add_table_cell(ctx, headers[i], "", 1); // 1 for header cell
     }
     html_end_table_row(ctx);
     
     // Table body
     int max_rows = rows < 10 ? rows : 10; // Cap at 10 rows
     for (int i = 0; i < max_rows; i++) {
         html_begin_table_row(ctx, "");
         
         // Function name cell
         html_add_table_cell(ctx, functions[i][0], "style='font-family: monospace;'", 0);
         
         // Description cell
         html_add_table_cell(ctx, functions[i][1], "", 0);
         
         // Category cell
         html_add_table_cell(ctx, categories[i], "", 0);
         
         html_end_table_row(ctx);
     }
     
     html_end_table(ctx);
 }
 
 /**
  * Create a complex contact form
  */
 void create_contact_form(html_context* ctx) {
     if (!ctx) return;
     
     // Start form
     html_add_form(ctx, "#", "post", "id='contact-form' onsubmit='return validateForm()'");
     html_begin_section(ctx, "class='contact-form'");
     
     // Name field
     html_add_div(ctx, "class='form-group'", "");
     html_begin_section(ctx, "class='form-group'");
     html_begin_tag(ctx, "label", "for='name'");
     html_add_content(ctx, "Name:");
     html_end_tag(ctx); // label
     html_add_input(ctx, "text", "name", "", "id='name' placeholder='Your name' required");
     html_end_section(ctx); // form-group
     
     // Email field
     html_add_div(ctx, "class='form-group'", "");
     html_begin_section(ctx, "class='form-group'");
     html_begin_tag(ctx, "label", "for='email'");
     html_add_content(ctx, "Email:");
     html_end_tag(ctx); // label
     html_add_input(ctx, "email", "email", "", "id='email' placeholder='Your email' required");
     html_end_section(ctx); // form-group
     
     // Subject field with dropdown
     html_add_div(ctx, "class='form-group'", "");
     html_begin_section(ctx, "class='form-group'");
     html_begin_tag(ctx, "label", "for='subject'");
     html_add_content(ctx, "Subject:");
     html_end_tag(ctx); // label
     
     html_begin_tag(ctx, "select", "name='subject' id='subject'");
     html_begin_tag(ctx, "option", "value='general'");
     html_add_content(ctx, "General Inquiry");
     html_end_tag(ctx); // option
     html_begin_tag(ctx, "option", "value='support'");
     html_add_content(ctx, "Technical Support");
     html_end_tag(ctx); // option
     html_begin_tag(ctx, "option", "value='feature'");
     html_add_content(ctx, "Feature Request");
     html_end_tag(ctx); // option
     html_begin_tag(ctx, "option", "value='bug'");
     html_add_content(ctx, "Bug Report");
     html_end_tag(ctx); // option
     html_end_tag(ctx); // select
     
     html_end_section(ctx); // form-group
     
     // Message field
     html_add_div(ctx, "class='form-group'", "");
     html_begin_section(ctx, "class='form-group'");
     html_begin_tag(ctx, "label", "for='message'");
     html_add_content(ctx, "Message:");
     html_end_tag(ctx); // label
     html_begin_tag(ctx, "textarea", "name='message' id='message' rows='5' placeholder='Your message' required");
     html_end_tag(ctx); // textarea
     html_end_section(ctx); // form-group
     
     // Subscription checkbox
     html_add_div(ctx, "class='form-group'", "");
     html_begin_section(ctx, "class='form-group'");
     html_begin_tag(ctx, "label", "style='display: inline-flex; align-items: center;'");
     html_add_input(ctx, "checkbox", "subscribe", "yes", "id='subscribe' style='width: auto; margin-right: 10px;'");
     html_add_content(ctx, "Subscribe to newsletter");
     html_end_tag(ctx); // label
     html_end_section(ctx); // form-group
     
     // Submit button
     html_add_button(ctx, "submit", "Send Message", "class='submit-btn'");
     
     html_end_section(ctx); // form
 }
 
 /**
  * Create navigation menu
  */
 void create_navigation(html_context* ctx) {
     if (!ctx) return;
     
     html_begin_tag(ctx, "nav", "class='navbar navbar-expand-lg navbar-light bg-light'");
     html_begin_tag(ctx, "div", "class='container'");
     
     // Brand
     html_add_anchor(ctx, "#", "HTML Gen", "class='navbar-brand'");
     
     // Navigation items
     html_begin_tag(ctx, "div", "class='collapse navbar-collapse'");
     html_begin_tag(ctx, "ul", "class='navbar-nav ms-auto mb-2 mb-lg-0'");
     
     // Create nav items with a loop
     const char* nav_items[] = {"Home", "Features", "Documentation", "Examples", "Contact"};
     const char* nav_links[] = {"#", "#features", "#docs", "#examples", "#contact-section"};
     
     for (int i = 0; i < 5; i++) {
         html_begin_tag(ctx, "li", "class='nav-item'");
         html_add_anchor(ctx, nav_links[i], nav_items[i], "class='nav-link'");
         html_end_tag(ctx); // li
     }
     
     html_end_tag(ctx); // ul
     html_end_tag(ctx); // div (collapse)
     
     html_end_tag(ctx); // container
     html_end_tag(ctx); // nav
 }
 
 /**
  * Add dynamically generated content
  */
 void add_dynamic_content(html_context* ctx) {
     if (!ctx) return;
     
     html_add_paragraph(ctx, "", "This content is dynamically generated at runtime:");
     
     // Start an ordered list
     html_begin_ordered_list(ctx, "");
     
     // Generate 5 list items with randomized content
     char buffer[100];
     for (int i = 1; i <= 5; i++) {
         snprintf(buffer, sizeof(buffer), "Dynamically generated item #%d: Value = %d", 
                  i, rand() % 100);
         html_add_list_item(ctx, buffer, "");
     }
     
     html_end_list(ctx);
     
     // Generate a timestamp
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     
     char timestamp[64];
     strftime(timestamp, sizeof(timestamp), 
              "This document was generated on %B %d, %Y at %H:%M:%S", t);
     
     html_add_div(ctx, "class='timestamp' style='font-style: italic; margin-top: 20px;'", timestamp);
 }
 
 /**
  * Error handler for consistent error reporting
  */
 void handle_error(const char* action) {
     fprintf(stderr, "ERROR: Failed to %s: %s\n", action, html_get_last_error());
 }
 
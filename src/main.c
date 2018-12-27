#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "countries.h"
#include "cgic.h"

int cgiMain()
{
   short i, order = ascending, process = -1;
   char target[50], option[25], flag[25];
   struct record *rec;

   struct record *(*search)(char *);

   setlocale(LC_NUMERIC, "en_US.utf8");

   cgiFormString("target", target, sizeof(target));
   cgiFormString("option", option, sizeof(option));
   cgiFormString("order", flag, sizeof(flag));

   printf("Content-Type: text/html\n\n");
   puts("<!DOCTYPE html>");
   puts("<head>");
   puts("<meta name='viewport' charset='UTF-8' content='width=device-width, initial-scale=1.0'>");
   puts(" <link rel='stylesheet' media='all' href='/includes/gradienttable.css'>");
   puts("<title>C International Data Retrieval</title>");
   puts("</head>");
   puts("<body>");

   puts("<header><p>C International Data Retrieval</p></header>");
   print_html_navlinks();
   puts("<h1>C International Data Retrieval</h1>");
   print_html_form(target, option, flag);
   puts("<br>");

   if ( strcmp(option, "country_search") == 0 ) {
      process = retrieve;
      search = &binary_search;
   } else if ( strcmp(option, "capital_search") == 0 ) {
      process = retrieve;
      search = &linear_search;
   } else if ( strcmp(option, "country_list") == 0 ) {
      process = country;
   } else if ( strcmp(option, "capital_list") == 0 ) {
      process = capital;
   } else if ( strcmp(option, "population") == 0 ) {
      process = pop;
   } else if ( strcmp(option, "area") == 0 ) {
      process = area;
   } else if ( strcmp(option, "gdp") == 0 ) {
      process = gdp;
   } else if ( strcmp(option, "per_capita") == 0 ) {
      process = per_capita;
   } else {
      puts("<p>invalid option</p>");
   }

   if ( strcmp(flag, "descending") == 0 )
      order = descending;

   if (process == retrieve) {
      rec = search(target);
      if (rec != NULL) {
         puts("<table class='gradienttable-left-justify'>");
         print_html_table_header(retrieve);            
         print_record_html(rec, retrieve);
         puts("</table>");
      } else {
         puts("<p>target not found</p>");
      }
   } else {
      print_html_table(process, order);
   }

   print_html_navlinks();
   puts("<footer><p>copyright \u00A9 2018 Josh Roybal</p></footer>");
   puts("</body>");
   puts("</html>");
   return(0);
}

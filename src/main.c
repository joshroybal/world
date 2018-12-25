#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "countries.h"
#include "cgic.h"

enum { retrieve, list, tabulate };
enum { ascending, descending };

int cgiMain()
{
   short i, process = -1;
   char target[50], option[25], order[25], index_file[25];
   unsigned *keys;
   unsigned char *idx;
   /* struct record *rec = (struct record *) malloc(sizeof(struct record)); */
   struct record *rec;
   FILE *fp;

   struct record *(*search)(char *);

   setlocale(LC_NUMERIC, "en_US.utf8");

   cgiFormString("target", target, sizeof(target));
   cgiFormString("option", option, sizeof(option));
   cgiFormString("order", order, sizeof(order));

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
   print_html_form(target, option, order);

   if ( strcmp(option, "country_search") == 0 ) {
      process = retrieve;
      search = &binary_search;
   } else if ( strcmp(option, "capital_search") == 0 ) {
      process = retrieve;
      search = &linear_search;
   } else if ( strcmp(option, "country_list") == 0 ) {
      process = list;
   } else if ( strcmp(option, "capital_list") == 0 ) {
      strcpy(index_file, "world/capital.dat");
   } else if ( strcmp(option, "population") == 0 ) {
      strcpy(index_file, "world/population.dat");
   } else if ( strcmp(option, "area") == 0 ) {
      strcpy(index_file, "world/area.dat");
   } else if ( strcmp(option, "gdp") == 0 ) {
      strcpy(index_file, "world/gdp.dat");
   } else if ( strcmp(option, "per_capita") == 0 ) {
      process = tabulate;
   } else {
      puts("<p>invalid option</p>");
   }

   puts("<br>");

   if (process == retrieve) {
      rec = search(target);
      if (rec != NULL) {
         puts("<table class='gradienttable-left-justify'>");
         print_html_table_header();            
         print_record_html(rec);
         puts("</table>");
      } else {
         puts("<p>target not found</p>");
      }
   } else if (process == list) {
      rec = (struct record *) malloc(sizeof(struct record));
      puts("<table class='gradienttable-left-justify'>");
      print_html_table_header();
      fp = fopen("world/countries.dat", "rb");
      if ( strcmp(order, "ascending") == 0) {
         for (i = 0; i < NORECS; ++i) {
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      } else {
         for (i = NORECS - 1; i >= 0; --i) {
            fseek(fp, i * sizeof(struct record), SEEK_SET);
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      }
      puts("</table>");
      fclose(fp);
      free(rec);
   } else if (process == tabulate) {
      keys = (unsigned *) malloc(NORECS * sizeof(unsigned));
      idx = (unsigned char *) malloc(NORECS * sizeof(unsigned char));
      rec = (struct record *) malloc(sizeof(struct record));
      fp = fopen("world/countries.dat", "rb");
      for (i = 0; i < NORECS; ++i) {
         fread(rec, sizeof(struct record), 1, fp);
         keys[i] = 1000000 * (float)rec->gdp / rec->population;
      }
      fclose(fp);
      shell_sort_idx(keys, idx, NORECS);
      free(keys);
      puts("<table class='gradienttable-left-justify'>");
      print_html_table_header();
      fp = fopen("world/countries.dat", "rb");
      if ( strcmp(order, "ascending") == 0 ) {
         for (i = 0; i < NORECS; ++i) {
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      } else {
         for (i = NORECS -1 ; i >= 0; --i) {
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      }
      puts("</table>");
      fclose(fp);
      free(rec);
      free(idx);
   } else {
      rec = (struct record *) malloc(sizeof(struct record));
      idx = (unsigned char *) malloc(NORECS * sizeof(unsigned char));
      fp = fopen(index_file, "rb");
      fread(idx, sizeof(unsigned char), NORECS, fp);
      fclose(fp);
      puts("<table class='gradienttable-left-justify'>");
      print_html_table_header();
      fp = fopen("world/countries.dat", "rb");
      if ( strcmp(order, "ascending") == 0 ) {
         for (i = 0; i < NORECS; ++i) {
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      } else {
         for (i = NORECS -1 ; i >= 0; --i) {
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
            fread(rec, sizeof(struct record), 1, fp);
            print_record_html(rec);
         }
      }
      puts("</table>");
      fclose(fp);
      free(idx);
      free(rec);
   }

   print_html_navlinks();
   puts("<footer><p>copyright \u00A9 2018 Josh Roybal</p></footer>");
   puts("</body>");
   puts("</html>");
   return(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "countries.h"

struct record *binary_search(char *target)
{
   unsigned m, l = 0, r;
   static struct record rec;
   FILE *fp;

   if ( strcmp(target, "") == 0) /* nothing to do */
      return(NULL);

   fp = fopen("world/countries.dat", "rb");
   fseek(fp, 0, SEEK_END);
   r = ftell(fp) / sizeof(struct record) - 1;
   while (l <= r) {
      m = (l + r) / 2;
      fseek(fp, m * sizeof(struct record), SEEK_SET);
      fread((char *)&rec, sizeof(struct record), 1, fp);
      if ( strcmp(target, rec.country) < 0 )
         r = m - 1;
      else if ( strcmp(target, rec.country) > 0 )
         l = m + 1;
      else {
         fclose(fp);
         return(&rec);
      }
   }
   fclose(fp);
   return(NULL);
}

struct record *linear_search(char *target)
{
   static struct record rec;
   FILE* fp = fopen("world/countries.dat", "rb");

   fread((char *)&rec, sizeof(struct record), 1, fp);
   while ( !feof(fp) ) {
      if ( strcmp(target, rec.capital ) == 0 ) {
         fclose(fp);
         return(&rec);
      }
      fread((char *)&rec, sizeof(struct record), 1, fp);
   }
   fclose(fp);
   return(NULL);
}

void print_record_html(struct record *rec, short n)
{
   printf("<tr>");
   if (n > 0)
      printf("<td style='text-align:right;'>%d</td>", n);
   printf("<td><img src='/countries/%s'> %s</td>", rec->flag, rec->country);
   printf("<td>%s</td>", rec->capital);
   printf("<td style='text-align:right;'>%'d</td>", rec->population);
   printf("<td style='text-align:right;'>%'d</td>", rec->area);
   printf("<td style='text-align:right;'>$%'d</td>", rec->gdp);
   printf("<td style='text-align:right;'>$%'lld</td>", 1000000LL*rec->gdp/rec->population);
   printf("</tr>\n");
}

void print_html_table_header(short flag)
{
   printf("<tr>");
   if (flag)
      printf("<th>Rank</th>");
   printf("<th style='text-align:center;'>Country</th>");
   printf("<th style='text-align:center;'>Capital</th>");
   printf("<th style='text-align:center;'>Population</th>");
   printf("<th style='text-align:center;'>Total Area km\u00B2</th>");
   printf("<th style='text-align:center;'>Nominal GDP USD Millions 2017</th>");
   printf("<th style='text-align:center;'>GDP per capita</th>");
   printf("<tr>\n");

}

void print_html_table(short process, short order)
{
   short i;
   unsigned char *idx;
   unsigned *keys;
   struct record *rec = (struct record *) malloc(sizeof(struct record));
   FILE *fp;

   if (process != country) {
      idx = (unsigned char *) malloc(NORECS * sizeof(unsigned char));
      if (process != per_capita) {
         if (process == capital) {
            fp = fopen("world/capital.dat", "rb");
         } else if (process == pop) {
            fp = fopen("world/population.dat", "rb");
         } else if (process == area) {
            fp = fopen("world/area.dat", "rb");
         } else if (process == gdp) {
            fp = fopen("world/gdp.dat", "rb");
         }
         fread(idx, sizeof(unsigned char), NORECS, fp);
         fclose(fp);    
      } else {
         keys = (unsigned *) malloc(NORECS * sizeof(unsigned));
         fp = fopen("world/countries.dat", "rb");
         for (i = 0; i < NORECS; ++i) {
            fread(rec, sizeof(struct record), 1, fp);
            keys[i] = 1000000 * (float)rec->gdp / rec->population;
         }
         fclose(fp);
         shell_sort_idx(keys, idx, NORECS);
         free(keys);
      }
   }

   puts("<table class='gradienttable-left-justify'>");
   print_html_table_header(process);
   fp = fopen("world/countries.dat", "rb");
   if (order == ascending) {
      for (i = 0; i < NORECS; ++i) {
         if (process != country)
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
         fread(rec, sizeof(struct record), 1, fp);
         print_record_html(rec, i+1);
      }
   } else {
      for (i = NORECS - 1; i >= 0; --i) {
         if (process == country)
            fseek(fp, i * sizeof(struct record), SEEK_SET);
         else
            fseek(fp, idx[i] * sizeof(struct record), SEEK_SET);
         fread(rec, sizeof(struct record), 1, fp);
         print_record_html(rec, NORECS-i);
      }
   }   
   fclose(fp);
   free(rec);
   puts("</table>");
}

void print_html_navlinks()
{
   puts("<p><a href='/index.html'>Home</a> | <a href='/world.html'>Back</a></p>");
}

void print_html_form(char *target, char *option, char *flag)
{
   puts("<form action = \"world.cgi\" method = \"POST\" accept-charset=\"UTF-8\">");
   printf("<div>target: <input type = \"text\" name = \"target\" size=\"50\" value = \"%s\"></div>\n", target);
   puts("<br>");

   /* search options */
   puts("<div><label>search: </label>");   
   if ( strcmp(option, "country_search") == 0 ) {
      puts("<label><input type=\"radio\" name=\"option\" value=\"country_search\" checked>country</label>");
      puts("<label><input type=\"radio\" name=\"option\" value=\"capital_search\">capital</label></div>");
   } else {
      puts("<label><input type=\"radio\" name=\"option\" value=\"country_search\">country</label>");
      puts("<label><input type=\"radio\" name=\"option\" value=\"capital_search\" checked>capital</label></div>");
   }
   
   /* list options */
   puts("<div><label>list: </label>");   
   if ( strcmp(option, "country_list") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"country_list\" checked>country</label>");
   else     
      puts("<label><input type=\"radio\" name=\"option\" value=\"country_list\">country</label>");
   
   if ( strcmp(option, "capital_list") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"capital_list\" checked>capital</label>");
   else
      puts("<label><input type=\"radio\" name=\"option\" value=\"capital_list\">capital</label>");
   
   if ( strcmp(option, "population") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"population\" checked>population</label>");
   else
      puts("<label><input type=\"radio\" name=\"option\" value=\"population\">population</label>");
   
   if ( strcmp(option, "area") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"area\" checked>area</label>");
   else
      puts("<label><input type=\"radio\" name=\"option\" value=\"area\">area</label>");
   
   if ( strcmp(option, "gdp") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"gdp\" checked>GDP</label>");
   else
      puts("<label><input type=\"radio\" name=\"option\" value=\"gdp\">GDP</label>");

   if ( strcmp(option, "per_capita") == 0 )
      puts("<label><input type=\"radio\" name=\"option\" value=\"per_capita\" checked>GDP per capita</label></div>");
   else
      puts("<label><input type=\"radio\" name=\"option\" value=\"per_capita\">GDP per capita</label></div>");   
   
   /* order options */
   puts("<div><label>order: </label>");   
   if ( strcmp(flag, "ascending") == 0 ) {
      puts("<label><input type=\"radio\" name=\"order\" value=\"ascending\" checked>ascending</label>");
      puts("<label><input type=\"radio\" name=\"order\" value=\"descending\">descending</label></div>");
   } else {
      puts("<label><input type=\"radio\" name=\"order\" value=\"ascending\">ascending</label>");
      puts("<label><input type=\"radio\" name=\"order\" value=\"descending\" checked>descending</label></div>");
   }

   puts("<br>");
   puts("<div><input type='submit' name='search' value='submit'></div>");
   puts("</form>");
}

void shell_sort_idx(unsigned *keys, unsigned char *idx, unsigned char n)
{
   unsigned char i, j, inc = 1;
   unsigned char *idxval = (unsigned char *) malloc(sizeof(unsigned char));
   unsigned v;

   for (i = 0; i < n; ++i) /* initialize index to natural order */
      idx[i] = i;

   inc = n*5/11;
   while (inc > 0) {
      for (i = inc; i < n; i++)     {
         v = keys[idx[i]];
         j = i;
         *idxval = idx[i];
         while ( keys[idx[j-inc]] > v) {  /* asscending order sort */
            idx[j] = idx[j-inc];
            j -= inc;
            if (j < inc) break;
         }
         idx[j] = *idxval;
      }
      inc = (inc == 2) ? 1 : inc*5/11;
   }
   free(idxval);
}

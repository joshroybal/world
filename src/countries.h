#ifndef COUNTRIES_H
#define COUNTRIES_H

#define NORECS 204   /* bad, bad, bad */

enum { retrieve, country, capital, pop, area, gdp, per_capita };
enum { ascending, descending };

struct record {
   unsigned population;
   unsigned area;
   unsigned gdp;
   char flag[37];
   char country[33];
   char capital[26];
};

struct record *binary_search(char *);
struct record *linear_search(char *);
void print_record_html(struct record *, short);
void print_html_table_header(short);
void print_html_table(short, short);
void print_html_navlinks();
void print_html_form(char *, char *, char *);
void shell_sort_idx(unsigned *, unsigned char *, unsigned char);

#endif

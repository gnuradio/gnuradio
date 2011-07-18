/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: generate_codebook.c
  AUTHOR......: Bruce Perens                                        
  DATE CREATED: 29 Sep 2010                                                     
                                                                             
  Generate header files containing LSP quantisers, runs at compile time.

\*---------------------------------------------------------------------------*/

/*
  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

static const char usage[] =
"Usage: %s filename array_name [filename ...]\n"
"\tCreate C code for codebook tables.\n";

static const char format[] =
"The table format must be:\n"
"\tTwo integers describing the dimensions of the codebook.\n"
"\tThen, enough numbers to fill the specified dimensions.\n";

static const char header[] =
"/* THIS IS A GENERATED FILE. Edit generate_codebook.c and its input */\n\n"
"/*\n"
" * This intermediary file and the files that used to create it are under \n"
" * The LGPL. See the file COPYING.\n"
" */\n\n"
"#include \"defines.h\"\n\n";

struct codebook {
  unsigned int	k;
  unsigned int	log2m;
  unsigned int	m;
  float * cb;
};

static void
dump_array(const struct codebook * b, int index)
{
  int	limit = b->k * b->m;
  int	i;

  printf("static const float codes%d[] = {\n", index);
  for ( i = 0; i < limit; i++ ) {
    printf("  %g", b->cb[i]);
    if ( i < limit - 1 )
      printf(",");

    /* organise VQs by rows, looks prettier */
    if ( ((i+1) % b->k) == 0 )
	printf("\n");
  }
  printf("};\n");
}

static void
dump_structure(const struct codebook * b, int index)
{
  printf("  {\n");
  printf("    %d,\n", b->k);
  printf("    %g,\n", log(b->m) / log(2));
  printf("    %d,\n", b->m);
  printf("    codes%d\n", index);
  printf("  }");
}

float
get_float(FILE * in, const char * name, char * * cursor, char * buffer,
 int size)
{
  for ( ; ; ) {
    char *	s = *cursor;
    char	c;

    while ( (c = *s) != '\0' && !isdigit(c) && c != '-' && c != '.' )
      s++;
     
    /* Comments start with "#" and continue to the end of the line. */
    if ( c != '\0' && c != '#' ) {
      char *	end = 0;
      float	f = 0;

      f = strtod(s, &end);

      if ( end != s )
        *cursor = end;
        return f;
    }

    if ( fgets(buffer, size, in) == NULL ) {
      fprintf(stderr, "%s: Format error. %s\n", name, format);
      exit(1);
    }
    *cursor = buffer;
  }
}

static struct codebook *
load(FILE * file, const char * name)
{
  char			line[1024];
  char *		cursor = line;
  struct codebook *	b = malloc(sizeof(struct codebook));
  int			i;
  int			size;

  *cursor = '\0';

  b->k = (int)get_float(file, name, &cursor, line, sizeof(line));
  b->m = (int)get_float(file, name ,&cursor, line, sizeof(line));
  size = b->k * b->m;

  b->cb = (float *)malloc(size * sizeof(float));

  for ( i = 0; i < size; i++ )
    b->cb[i] = get_float(file, name, &cursor, line, sizeof(line));

  return b;
}

int
main(int argc, char * * argv)
{
  struct codebook * *	cb = malloc(argc * sizeof(struct codebook *));
  int			i;

  if ( argc < 2 ) {
    fprintf(stderr, usage, argv[0]);
    fprintf(stderr, format);
    exit(1);
  }

  for ( i = 0; i < argc - 2; i++ ) {
    FILE *	in = fopen(argv[i + 2], "r");

    if ( in == NULL ) {
      perror(argv[i + 2]);
      exit(1);
    }

    cb[i] = load(in, argv[i + 2]);

    fclose(in);
  }

  printf(header);
  for ( i = 0; i < argc - 2; i++ ) {
    printf("  /* %s */\n", argv[i + 2]);
    dump_array(cb[i], i);
  }
  printf("\nconst struct lsp_codebook %s[] = {\n", argv[1]);
  for ( i = 0; i < argc - 2; i++ ) {
    printf("  /* %s */\n", argv[i + 2]);
    dump_structure(cb[i], i);
    printf(",\n");
  }
  printf("  { 0, 0, 0, 0 }\n");
  printf("};\n");

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_INPUT 1024

struct input
{
  size_t capacity;
  size_t len;
  char *data;
};
int command_not_found (char *input);
int echo (struct input *input);
void parse_input (struct input *input);
int
main (int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  struct input *i = calloc (sizeof (struct input), 1);
  i->capacity = MAX_INPUT;
  i->data = calloc (sizeof (char), i->capacity);
  while (1)
    {
      fprintf (stdout, "$ ");
      memset (i->data, 0, i->capacity);
      i->len = 0;

      parse_input (i);
      if (*(i->data) == '\0')
	continue;
      if (strncmp ("exit", (i->data), strlen ("exit")) == 0)
	break;
      if (strncmp ("echo", (i->data), strlen ("echo")) == 0)
	{
	  echo (i);
	  continue;
	}
    }
  command_not_found ((i->data));
  free (i->data);
  free (i);
  return 0;

}

int
command_not_found (char *input)
{
  char *end_input = strchr (input, ' ');
  if (end_input == NULL)
    {
      fprintf (stdout, "%s", input);
    }
  else
    {
      fprintf (stdout, "%.*s", (int) (end_input - input), input);
    }
  fprintf (stdout, ": command not found\n");
  return EXIT_FAILURE;
}

int
echo (struct input *in)
{
  char *data = in->data;
  data += strlen (data) + 1;
  if (*data == '\0')
    return EXIT_FAILURE;
  while (*data != '\0')
    {
      fprintf (stdout, "%s ", data);
      data += strlen (data) + 1;
    }
  fputc ('\n', stdout);
  return EXIT_SUCCESS;
}

void
parse_input (struct input *i)
{
  int double_quote = 0;
  int single_quote = 0;
  int is_echap = 0;
  int c = 0;
  while (1)
    {
      if (i->len >= i->capacity)
	{
	  memset (i->data, 0, i->capacity);
	  i->len = 0;
	  fprintf (stderr, "too much characters on input line.\n");
	  break;
	}
      c = fgetc (stdin);
      if (c == EOF)
	{
    fputc('\n',stdout);
    clearerr(stdin);
	  return;
	}
      if (c == '\\' && !is_echap)
	{
	  is_echap = 1;
	  continue;
	}
      if (c == '"' && single_quote % 2 == 0 && !is_echap)
	{
	  double_quote++;
	  if (double_quote % 2 == 0)
	    {
	      (i->data)[i->len] = '\0';
	      (i->len)++;
	    }
	  continue;
	}
      if (c == '\'' && double_quote % 2 == 0 && !is_echap)
	{
	  single_quote++;
	  if (single_quote % 2 == 0)
	    {
	      (i->data)[i->len] = '\0';
	      (i->len)++;
	    }
	  continue;
	}
      if (c == ' ' && single_quote % 2 == 0 && double_quote % 2 == 0
	  && !is_echap)
	{
	  if (i->len != 0 && (i->data)[i->len - 1] != '\0')
	    {
	      (i->data)[i->len] = '\0';
	      (i->len)++;
	    }
	  continue;
	}
      if (c == '\n' && double_quote % 2 == 0 && single_quote % 2 == 0
	  && !is_echap)
	{
	  double_quote = 0;
	  single_quote = 0;
	  break;
	}
      if (c != '\n' || !is_echap)
	{
	  (i->data)[(i->len)] = (char) c;
	  (i->len)++;
	}
      is_echap = 0;
      if (c == '\n')
	putc ('>', stdout);
    }
}

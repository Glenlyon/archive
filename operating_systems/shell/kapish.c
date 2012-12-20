/******************************************************************************
* Copyright (C) 2012 David Rusk
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

struct command_token
{
  char *current_token;
  struct command_token *next_token;
};

typedef struct command_token cmd_tok;

void print_prompt()
{
  printf("? ");
}

cmd_tok *create_cmd_tok(char *token)
{
  cmd_tok *new_cmd_tok = (cmd_tok *)malloc(sizeof(cmd_tok));
  new_cmd_tok->current_token = token;
  new_cmd_tok->next_token = NULL;
  return new_cmd_tok;
}

int getln(char s[], int lim)
{
  int c, i;
  
  for (i = 0; i < lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
    {
      s[i] = c;
    }

  if(i == 0 && c == '\n')
    {
      s[i] = c;
      i++;
    }
  /*
  if (c == '\n')
    {
      s[i] = c;
      ++i;
    }n
  */
  s[i] = '\0';
  return i;
}

void debug_print_tokens(cmd_tok *current_tok_ptr)
{
  printf("Tokenized command:\n");
  while(current_tok_ptr != NULL)
    {
      printf("%s\n", current_tok_ptr->current_token);
      current_tok_ptr = current_tok_ptr->next_token;
    }
}

int get_num_tokens(cmd_tok *current_cmd)
{
  int num_tokens = 0;
  while(current_cmd != NULL)
    {
      num_tokens++;
      current_cmd = current_cmd->next_token;
    }
  return num_tokens;
}

char **create_args(cmd_tok *current_cmd)
{
  int num_args = get_num_tokens(current_cmd);
  char **args = (char **)malloc((num_args + 1)*sizeof(char *));

  int i;
  for(i = 0; i < num_args; i++)
    {
      args[i] = current_cmd->current_token;
      current_cmd = current_cmd->next_token;
    }

  /* execvp requires last arg to be NULL */
  args[i] = NULL;
  return args;
}

void print_cmd_not_found(char *cmd)
{
  printf("kapish: %s: command not found\n", cmd);
}

void destroy_cmd_toks(cmd_tok *current_command)
{
  cmd_tok *next_tok = NULL;
  while(current_command != NULL)
    {
      next_tok = current_command->next_token;
      free(current_command);
      current_command = next_tok;
    }
}

void handle_signal(int sig_num)
{
  printf("\n");
  print_prompt();
  fflush(stdout);
}

void child_terminate_handler(int sig_num)
{
  printf("\n");
}

#define MAXLINE 513

int main(int argc, char **argv)
{
  signal(SIGINT, handle_signal);

  char line[MAXLINE]; /* The current input line */

  /* Points to the beginning of the list of current command tokens */
  cmd_tok *current_cmd = NULL;

  /* Points to a token within the command tokens list */
  cmd_tok *current_tok_ptr = NULL;

  print_prompt();
  int length = 0;
  while((length = getln(line, MAXLINE)) > 0)
    {
      if (length == 1 && line[0] == '\n')
	{
	  /* They pushed enter without any text */
	  print_prompt();
	  continue;
	}
      char *token = strtok(line, " ");
      current_cmd = create_cmd_tok(token);
      current_tok_ptr = current_cmd;
      token = strtok(NULL, " ");

      /* Continue to tokenize input line */
      while (token != NULL)
	{
	  current_tok_ptr->next_token = create_cmd_tok(token);
	  current_tok_ptr = current_tok_ptr->next_token;
	  token = strtok(NULL, " ");
	}

      /* Process the command */

      /* Check if it was one of the internal commands: exit, cd */
      if (strcmp(current_cmd->current_token, "exit") == 0)
	{
	  destroy_cmd_toks(current_cmd);
	  return 0;
	}
      else if (strcmp(current_cmd->current_token, "cd") == 0)
	{
	  char *directory = NULL;
	  int num_args = get_num_tokens(current_cmd);
	  if (num_args == 1)
	    {
	      /* The user just typed "cd".  Go to home directory. */
	      directory = getenv("HOME");
	    }
	  else
	    {
	      /* Get the token after "cd" */
	      directory = current_cmd->next_token->current_token;
	    }

	  int cd_return_val = chdir(directory);
	  
	  /* chdir returns 0 if successful */
	  if (cd_return_val != 0)
	    {
	      /* Print error messages */
	      char *error_msg = 
		(char *)malloc(sizeof(argv[0]) +
			       sizeof(directory) + 7*sizeof(char));
	      strcpy(error_msg, argv[0]);
	      strcat(error_msg, ": cd: ");
	      strcat(error_msg, directory);
	      perror(error_msg);
	      free(error_msg);
	    }
	}
      else
	{
	  /* Assume input was an external executable command */
	  char **args = create_args(current_cmd);

	  /* In case executable is not on path, allocate memory for 
	     args[0] with ./ prepended */
	  char *arg0 = NULL;
	  int child_pid = fork();
	  if (child_pid == 0)
	    {
	      execvp(args[0], args);

	      /* execvp only returns if it fails */

	      /* Didn't find command on path, 
		 try looking in current directory */
	      arg0 = (char *)malloc(sizeof(args[0]) + 3*sizeof(char));
	      strcpy(arg0, "./");
	      strcat(arg0, args[0]);
	      execvp(arg0, args);

	      /* Couldn't find command */
	      print_cmd_not_found(args[0]);
	      return 0;
	    }
	  else
	    {
	      signal(SIGINT, child_terminate_handler);
	      wait(NULL);
	    }
	  signal(SIGINT, handle_signal);
	  free(args);
	  free(arg0);
	}

      /* Free and reset command pointers */
      destroy_cmd_toks(current_cmd);
      current_cmd = NULL;
      current_tok_ptr = NULL;

      print_prompt();
    }

  /* In case they exit with control-D, 
     want prompt to show up on new line */
  printf("\n");

  return 0;
}

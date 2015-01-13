// UCLA CS 111 Lab 1 command reading

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
#define MAX_SIZE_OF_COMMAND 10000
#define MAX_LINE_OF_SINGLE_COMMAND 100
/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
enum operator_type
{
    IF, THEN, ELSE, FI, COLON, PIPE, WHILE, UNTIL, DONE, DO, LEFT_PAREN, RIGHT_PAREN
};

struct command_stream
{
    command_t command;
    command_stream_t next;
};

command_stream_t initiate_command_stream(){
  command_stream_t cmd_stream = (command_stream_t) malloc(sizeof(struct command_stream));
  cmd_stream->command = NULL;
  cmd_stream->next = NULL;
  return cmd_stream;
}

command_t initiate_command(){
  command_t cmd = (command_t) malloc(sizeof(struct command));
  cmd->status = 0;
  cmd->input = NULL;
  cmd->output= NULL;
  return cmd;
}


enum command_type determineType(char cmd[][MAX_SIZE_OF_COMMAND]){
  return SIMPLE_COMMAND;
};
/* Parse a string of command into struct type command */
command_t parseCmd(char cmd[][MAX_SIZE_OF_COMMAND]){
  command_t c = initiate_command();
  c->type = determineType(cmd);
  switch (c->type){
     case SIMPLE_COMMAND:
          c->u.word = (void *)malloc(MAX_LINE_OF_SINGLE_COMMAND);
          *(c->u.word) = (char *)malloc(strlen(*cmd)+1);
          strcpy(*(c->u.word),*cmd);
         // printf("%d\n",c->u.word);
          //char **w = c->u.word;
//          printf ("%*s%s", 2, "", *w);
     //*(c->u.word+1) = NULL;
     // pipe: c->command[0] = parseCmd(cmd[leftpart]);
     //       c->command[1] = parseCmd(cmd[rightpart]);
          break;
     case IF_COMMAND:
          break;
     case PIPE_COMMAND:
          break;
     case SEQUENCE_COMMAND:
          break;
     case SUBSHELL_COMMAND:
          break;
     case UNTIL_COMMAND:
          break;
     case WHILE_COMMAND:
          break;
  }
  return c;
}
 
int get_next_command(int (*get_next_byte) (void *),
         void *get_next_byte_argument,char cmd[][MAX_SIZE_OF_COMMAND]){
    int c, last = 0, complete_command = 1;
    int line = 0;

    while ((c = get_next_byte(get_next_byte_argument)) != EOF)
    {
        if (c == '\n')
        {
             cmd[line][last] = '\0';
             if (complete_command){
                cmd[line+1][0] = '\0';
                return 0;
             }
             else{
                last = 0;
                line++;
             }
        }
        else{
             cmd[line][last++] = c;
        }
    }
    cmd[line][last] = '\0';
    cmd[line+1][0] = '\0';
    return 1;
}
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  // char cmd_string[MAX_LINE_OF_SINGLE_COMMAND][MAX_SIZE_OF_COMMAND];
  // int reachEOF;
  // command_t cmd;
  // command_stream_t head = initiate_command_stream();
  // command_stream_t cur_cmd = head;
  // bzero(cmd_string,MAX_LINE_OF_SINGLE_COMMAND*MAX_SIZE_OF_COMMAND);
  // do{
  //     /* seperate the cmd by newline for now */
  //     reachEOF = get_next_command(get_next_byte,get_next_byte_argument,cmd_string);
  //     /* parse the command from string to command_t */
  //     //printf("cmd %s\n",cmd_string);
  //     cmd = parseCmd(cmd_string);

  //     /* append the new command_t to linked list command_stream_t */
  //     cur_cmd->next = initiate_command_stream();
  //     cur_cmd = cur_cmd->next;
  //     cur_cmd->command = cmd;
  // }while(reachEOF == 0);
  // //error (1, 0, "command reading not yet implemented");
  
  enum operator_type op_stack[MAX_SIZEOF_STACK];
  command_t cmd_stack[MAX_SIZEOF_STACK];

  char *c;
  while (c = get_next_token(get_next_byte, get_next_byte_argument))
  {
      printf("%s\n",c);
  } 

  return head;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
  //return 0;
  //printf("cmd %d next %d",s->command, s->next);

    if (s->next) 
    {
        command_t command = s->next->command;
        s->next = s->next->next;
        return command;
    }
    return NULL;
    
}

/*
*  To-do:
      1. separate cmd;
      2. determine cmd;
      3. deal with each cmd type;
      
*/

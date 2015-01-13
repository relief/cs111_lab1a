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
#define MAX_SIZE_OF_COMMAND 1000
#define MAX_SIZE_OF_SIMPLE_COMMAND 1000
#define MAX_SIZEOF_STACK 1000
const char IF_STR[] = "if";
/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
enum operator_type
{
    OTHERS, IF, THEN, ELSE, FI, SEMICOLON, PIPE, WHILE, UNTIL, DONE, DO, LEFT_PAREN, RIGHT_PAREN, NEWLINE, STDIN, STDOUT
};

struct command_stream
{
    command_t command;
    command_stream_t next;
};

enum operator_type op_stack[MAX_SIZEOF_STACK];
command_t cmd_stack[MAX_SIZEOF_STACK];
int op_stack_top = -1;
int cmd_stack_top = -1;

command_stream_t initiate_command_stream(){
  command_stream_t cmd_stream = (command_stream_t) malloc(sizeof(struct command_stream));
  cmd_stream->command = NULL;
  cmd_stream->next = NULL;
  return cmd_stream;
}

command_t initiate_command(enum command_type type){
  command_t cmd = (command_t) malloc(sizeof(struct command));
  cmd->type = type;
  cmd->status = 0;
  cmd->input = NULL;
  cmd->output= NULL;
  return cmd;
}

/* Parse a string of command into struct type command */
// command_t parseCmd(char cmd[][MAX_SIZE_OF_COMMAND]){
//   command_t c = initiate_command();
//   c->type = determineType(cmd);
//   switch (c->type){
//      case SIMPLE_COMMAND:
//           c->u.word = (void *)malloc(MAX_LINE_OF_SINGLE_COMMAND);
//           *(c->u.word) = (char *)malloc(strlen(*cmd)+1);
//           strcpy(*(c->u.word),*cmd);
//          // printf("%d\n",c->u.word);
//           //char **w = c->u.word;
// //          printf ("%*s%s", 2, "", *w);
//      //*(c->u.word+1) = NULL;
//      // pipe: c->command[0] = parseCmd(cmd[leftpart]);
//      //       c->command[1] = parseCmd(cmd[rightpart]);
//           break;
//      case IF_COMMAND:
//           break;
//      case PIPE_COMMAND:
//           break;
//      case SEQUENCE_COMMAND:
//           break;
//      case SUBSHELL_COMMAND:
//           break;
//      case UNTIL_COMMAND:
//           break;
//      case WHILE_COMMAND:
//           break;
//   }
//   return c;
// }

//next step: ignore tokens that are single spaces, interpret just one newline as a ';'
char *get_next_token(int (*get_next_byte) (void *),
               void *get_next_byte_argument)
{
    char token[MAX_SIZE_OF_COMMAND];
    int i = 0;
    static char operator = '\0';
    //printf("Operator = %c\n", operator);    
	
    if (operator == ' ') {
        // ignore spaces
        operator = '\0';
    }
    else if (operator != '\0') {
        token[0] = operator;
        token[1] = '\0';
        operator = '\0';
        //printf("About to return operator: %s\n", token);
        return token;
    }

    char c = get_next_byte(get_next_byte_argument);

    if (c == EOF)
        return 0;

    if (c == '#') {
        //ignore comments
        while (c != '\n' && c != EOF) {
            c = get_next_byte(get_next_byte_argument);
        }
        c = get_next_byte(get_next_byte_argument);
    }

    while (c != EOF) {
	//printf("At character: %c\n", c);
        if (c == ' ' || c == '\n' || c == ';' || c == '|' || c == ':' || c == '>' || c == '<' || c == '(' || c == ')') {
            token[i++] = '\0';
            operator = c; // we also need to return the current operator char
            return token;
        }
        else {
            token[i++] = c;
            c = get_next_byte(get_next_byte_argument);
        }
    }
    
    token[i] = '\0';
    return token;
}

enum operator_type get_token_type(char *token){
    if (strncmp(token,"if",2) == 0)
        return IF;
    if (strncmp(token,"then",4) == 0)
        return THEN;
    if (strncmp(token,"else",4) == 0)
        return ELSE;
    if (strncmp(token,"fi",2) == 0)
        return FI;
    if (strncmp(token,";",1) == 0)
        return SEMICOLON;
    if (strncmp(token,"|",1) == 0)
        return PIPE;
    if (strncmp(token,"while",5) == 0)
        return WHILE;
    if (strncmp(token,"until",5) == 0)
        return UNTIL;
    if (strncmp(token,"done",4) == 0)
        return DONE;
    if (strncmp(token,"do",2) == 0)
        return DO;
    if (strncmp(token,"(",1) == 0)
        return LEFT_PAREN;
    if (strncmp(token,")",1) == 0)
        return RIGHT_PAREN;
    if (strncmp(token,"\n",1) == 0)
        return NEWLINE;
    if (strncmp(token,"<",1) == 0)
        return STDIN;
    if (strncmp(token,">",1) == 0)
        return STDOUT;
    
    return OTHERS;
}

void push_to_cmd_stack(command_t ele){
    cmd_stack_top = cmd_stack_top + 1;
    cmd_stack[cmd_stack_top] = ele;
}
command_t top_of_cmd_stack(){
    if (cmd_stack_top >= 0)
      return cmd_stack[cmd_stack_top];
    return NULL;
}
command_t pop_cmd_stack(){
    cmd_stack_top -= 1;
    return cmd_stack[cmd_stack_top + 1];
}
void push_to_op_stack(enum operator_type ele){
    op_stack_top = op_stack_top + 1;
    op_stack[op_stack_top] = ele;
}
enum operator_type pop_op_stack(){
    op_stack_top = op_stack_top - 1;
    return op_stack[op_stack_top + 1];
}
enum operator_type top_of_op_stack(){
    if (op_stack_top < 0)
        return -1;
    return op_stack[op_stack_top];
}
command_t parse_as_simple(command_t cmd0, char* new_word){
    char *new_word_part = (char *)malloc(strlen(new_word)+1);
    strcpy(new_word_part,new_word);

    if (cmd0){
        char **w = cmd0->u.word;
 
        while (*++w) ;
      
        *w = new_word_part;
      
        return cmd0;
    }else{
        command_t cmd = initiate_command(SIMPLE_COMMAND);
        cmd->u.word = (void *)malloc(MAX_SIZE_OF_SIMPLE_COMMAND*sizeof(void*));
        *(cmd->u.word) = new_word_part;
        
        return cmd;
    }
}
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  enum operator_type last_token_type,token_type;
  char *token;
  command_stream_t head = initiate_command_stream();
  command_stream_t cmd_stream = head;
  

  while (token = get_next_token(get_next_byte, get_next_byte_argument))
  {
      if (*token == ' ' || token[0] == '\0')
          continue; //ignore spaces
      printf("Got token: %s\n",token);
      token_type = get_token_type(token);
      printf("token_type %d\n",token_type);
      //printf("token_type: %d",token_type);
      if (token_type == OTHERS)
      {
          if (cmd_stack_top < 0)
          {
              command_t tmp = parse_as_simple(NULL, token);
              push_to_cmd_stack(tmp);
          }
          else
          {
              if (last_token_type == OTHERS){
                  parse_as_simple(top_of_cmd_stack(), token);
              }else{
                  command_t tmp = parse_as_simple(NULL, token);
                  push_to_cmd_stack(tmp);
              }
          }
      }
      else
      {
          if (op_stack_top < 0)
              push_to_op_stack(token_type);
          else{
              switch(token_type)
              {
                  case LEFT_PAREN:
                  case IF:
                  case WHILE:
                  case UNTIL:
                      push_to_op_stack(token_type);
                      break;
                  case NEWLINE:         
                      if (top_of_op_stack() == NEWLINE && op_stack_top == 0 && cmd_stack_top == 0)
                      {
                          pop_op_stack(op_stack,&op_stack_top);
                          cmd_stream->next = initiate_command_stream();
                          cmd_stream = cmd_stream->next;
                          cmd_stream->command = pop_cmd_stack();
                      }

                      break;
             }
          }
      }
      printf("top of op stack: %d\n", op_stack_top);
      printf("top of cmd stack: %d\n", cmd_stack_top);
      last_token_type = token_type;
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

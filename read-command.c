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
#define MAX_SIZEOF_STACK 1000
const char IF_STR[] = "if";
/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
enum operator_type
{
    OTHERS, IF, THEN, ELSE, FI, COLON, PIPE, WHILE, UNTIL, DONE, DO, LEFT_PAREN, RIGHT_PAREN
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
    // //int a = strncmp(token,IF_STR,1);
    // // printf("adsfsdf");
    if (strncmp(token,"if",2) == 0)
        return IF;

    // // if (strcmp(token,"else") == 0)
    // //     return ELSE;
    // // if (strcmp(token,"then") == 0)
    // //     return THEN;
    // printf("fine");
    return OTHERS;
}

void push_to_cmd_stack(command_t s[], int *top, command_t ele){
    *top = *top + 1;
    s[*top] = ele;
}
command_t pop_cmd_stack(command_t s[], int *top){
    *top = *top - 1;
    return s[*top + 1];
}
command_t parse_as_simple(command_t cmd0, char* new_word){
    if (cmd0){
        char **word = cmd0->u.word;
        while (*++word)
          continue;
        *word = new_word;
    }else{
        command_t cmd = initiate_command();
        *(cmd->u.word) = new_word;
    }
}
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  enum operator_type op_stack[MAX_SIZEOF_STACK];
  command_t cmd_stack[MAX_SIZEOF_STACK];
  int op_stack_top = -1;
  int cmd_stack_top = -1;
  enum operator_type last_token_type,token_type;
  char *token;

  while (token = get_next_token(get_next_byte, get_next_byte_argument))
  {
      if (*token == ' ' || token[0] == '\0')
          continue; //ignore spaces
      printf("Got token: %s\n",token);
      token_type = get_token_type(token);
      printf("token_type %d\n",token_type);
      //printf("token_type: %d",token_type);
      // if (token_type == OTHERS)
      // {
      //     if (cmd_stack_top < 0)
      //     {
      //         command_t tmp = parse_as_simple(NULL, token);
      //         push_to_cmd_stack(cmd_stack, cmd_stack_top, tmp);
      //     }
      //     else
      //     {
      //         // if (last_token_type == OTHERS){
      //         //     append_to_cmd_stack_top;
      //         // }else{
      //         //     push_to_cmd_stack;
      //         // }
      //     }
      // }
      // else
      // {
      //     if (op_stack_top < 0)
      //         push_to_op_stack;
      //     else{
      //         switch(token_type){
      //             case LEFT_PAREN:
      //             case IF:
      //             case WHILE:
      //             case UNTIL:
      //                 push_to_op_stack(token_type);
      //                 break;
      //             case PIPE:
      //                 if (top_of_op_stack() == PIPE)
      //                     parse_as_pipe;
      //                 push_to_op_stack(token_type);
      //                 break;
      //             case COLON:
      //                 if (top_of_op_stack() == PIPE)
      //                     parse_as_pipe;
      //                 else
      //                   if (top_of_op_stack() == COLON && last_token_type == OTHER)
      //                     parse_as_colon;
      //                 push_to_op_stack(token_type);
      //                 break;
      //             case THEN:
      //                 switch(top_of_op_stack()){
      //                     case PIPE:
      //                         parse_as_pipe;
      //                         break;
      //                     case COLON:
      //                         parse_as_colon;
      //                         break;
      //                     case If:
      //                         //fine here!
      //                         break;
      //                     default:
      //                         //may be error
      //                 }
      //                 break;
      //             case ELSE:
      //                 switch(top_of_op_stack()){
      //                     case PIPE:
      //                         parse_as_pipe;
      //                         break;
      //                     case COLON:
      //                         parse_as_colon;
      //                         break;
      //                     case THEN:
      //                         //fine here!
      //                         break;
      //                     default:
      //                         //may be error
      //                 }
      //                 break;
      //             case FI:
      //                 switch(top_of_op_stack()){
      //                     case PIPE:
      //                         parse_as_pipe;
      //                         break;
      //                     case COLON:
      //                         parse_as_colon;
      //                         break;
      //                     case THEN:
      //                     case ELSE:
      //                         //fine here!
      //                         break;
      //                     default:
      //                         //may be error
      //                 }
      //                 if (top_of_op_stack() == ELSE)
      //                     parse_as_if_then_else_fi();
      //                 else
      //                     parse_as_if_then_fi();
                                                  
      //                 break;
      //             case DO:
      //                 switch(top_of_op_stack()){
      //                     case PIPE:
      //                         parse_as_pipe;
      //                         break;
      //                     case COLON:
      //                         parse_as_colon;
      //                         break;
      //                     case WHILE:
      //                     case UNTIL:
      //                         //fine here!
      //                         break;
      //                     default:
      //                         //may be error
      //                 }
      //                 break;
      //             case DONE:
      //                 switch(top_of_op_stack()){
      //                     case PIPE:
      //                         parse_as_pipe;
      //                         break;
      //                     case COLON:
      //                         parse_as_colon;
      //                         break;
      //                     case DO:
      //                         //fine here!
      //                         break;
      //                     default:
      //                         //may be error
      //                 }
      //                 parse_as_while_do_done();
      //                 parse_as_until_do_done();
      //                 break;

                      

      //         }
      //     }
      // }
      last_token_type = token_type;
  } 

  return initiate_command_stream();
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

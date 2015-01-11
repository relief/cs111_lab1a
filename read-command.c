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

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream
{
      
};
command_t parseCmd(char * cmd){
  command_t c = new command();
  c->type = determineType(cmd);
  switch (c->type){
    pipe: c->command[0] = parseCmd(cmd[leftpart]);
          c->command[1] = parseCmd(cmd[rightpart]);
  }
  return c;
}
command_stream_t initiateCmd(){
  // finish this later
  command_stream_t cmd = new struct command_stream();
  cmd->
}
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
    //error (1, 0, "command reading not yet implemented");
    
  char cmd[10000];
  int leng = 0;
  command_stream_t head;
  while (1){
      /* seperate the cmd by newline for now */
      leng = 0;
      while (c = get_next_byte(get_next_byte_argument) != NEW_LINE){
          if (c == EOF) 
              break;
          cmd[leng] = c;
      }
      stream->command = parseCmd(cmd);
      stream->next = ;
  }
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

/*
*  To-do:
      1. separate cmd;
      2. determine cmd;
      3. deal with each cmd type;
      
*/

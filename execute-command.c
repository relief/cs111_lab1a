// UCLA CS 111 Lab 1 command execution

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
#include "sys/types.h"
#include "fcntl.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
prepare_profiling (char const *name)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (0, 0, "warning: profiling not yet implemented");
  return -1;
}

int
command_status (command_t c)
{
  return c->status;
}

int exec_simple_command(command_t c){
	pid_t pid;
    if ((pid = fork()) < 0) {
        error (1, 0, "Forking a child process failed");
        return -1;
    }
    else if (pid == 0) {
            if (execvp(*c->u.word,c->u.word) < 0) {
                error (1, 0, "Execvp for SIMPLE failed");
                return -1;
            }
    }
    else {
        while (wait(&(c->status)) != pid)
            ;
    }
    return 0;
}

int exec_pipe_command(command_t c, int profiling){
    pid_t pid;
    if ((pid = fork()) < 0) {
        error (1, 0, "Forking a child process failed");
        return -1;
    }
    else if (pid == 0) {
        execute_command(c->u.command[0], profiling);
        //somehow get output
    }
    else {
        while (wait(&(c->status)) != pid)
            ;
    }
    // Redirect input from pipe
    int in = open(output, O_RDONLY);
    dup2(in, 0);
    close(in);
    
    execute_command(c->u.command[1],profiling);
    return 0;
}

void
execute_command (command_t c, int profiling)
{
  /* FIXME: Replace this with your implementation, like 'prepare_profiling'.  */
    
    // Handle standard input/output redirection
    if (c->input) {
        int in = open(c->input, O_RDONLY);
        dup2(in, 0);
        close(in);
    }
    if (c->input) {
        int in = open(c->output, O_WONLY | O_APPEND | O_CREAT);
        dup2(out, 1);
        close(out);
    }
    
    switch (c->type){
    	case SIMPLE_COMMAND:
    		if (exec_simple_command(c) < 0)
    			exit(1);
    		break;
    	case SEQUENCE_COMMAND:
    		execute_command(c->u.command[0],profiling);
    		execute_command(c->u.command[1],profiling);
    		c->status = c->u.command[1]->status;
    		break;
    	case SUBSHELL_COMMAND:
    		execute_command(c->u.command[0],profiling);
    		break;
    	case IF_COMMAND:
    		// IF a THEN b ELSE c FI
    		execute_command(c->u.command[0],profiling);
    		if (c->u.command[0]->status == 0)		// a is true
    		{
    			execute_command(c->u.command[1],profiling);
    			c->status = c->u.command[1]->status;
    		}
    		else if (c->u.command[2])				// if a is false and there is else clause
    		{
    			execute_command(c->u.command[2],profiling);
    			c->status = c->u.command[2]->status;
    		}
    		break;
        case WHILE_COMMAND:
            // WHILE a DO b DONE
            execute_command(c->u.command[0],profiling);
            while (c->u.command[0]->status == 0)		// a is true
            {
                execute_command(c->u.command[1],profiling);
                c->status = c->u.command[1]->status;
            }
            break;
        case UNTIL_COMMAND:
            // UNTIL a DO b DONE
            execute_command(c->u.command[0],profiling);
            while (! (c->u.command[0]->status == 0)	)	// a is false
            {
                execute_command(c->u.command[1],profiling);
                c->status = c->u.command[1]->status;
            }
            break;
        case PIPE:
            if (exec_pipe_command(c, profiling) < 0)
                exit(1);
            break;
	}
    
    
  //error (1, 0, "command execution not yet implemented");
}

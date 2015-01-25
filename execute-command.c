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
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

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

int exec_simple_command(command_t c, int profiling){
    pid_t pid;
    
    //start time
    struct timespec start, finish;
    struct rusage usage;
    clock_gettime(CLOCK_REALTIME, &start);
    
    if ((pid = fork()) < 0) {
        error (1, 0, "Forking a child process failed");
        return -1;
    }
    else if (pid == 0) {
    	if (strcmp(*c->u.word	,":") == 0){
    		c->status = 0;
    	}else
	        if (execvp(*c->u.word,c->u.word) < 0) {
	                //error (1, 0, "Execvp for SIMPLE failed");
			char **w = c->u.word;
			printf("%s: command not found\n", *w);
	                return -1;
	        }
        exit(0);
    }
    else {
        while (wait(&(c->status)) != pid)
            ;
        //finish time
        clock_gettime(CLOCK_REALTIME, &finish);
        
        if (profiling == 0) { // write profiling info to log
            int exec_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
            getusage(RUSAGE_CHILDREN, &usage)
            log.write(finish, exec_time, usage.ru_utime, usage.ru_stime, c->u.word);
        }
    }
  //  printf("I am at the end of simple command with c status %d\n",c->status);
    return 0;
}

int exec_pipe_command(command_t c, int profiling){
    pid_t pid,childpid;
    int fd[2];
    int stdin_copy = dup(0);
    
    //start time
    struct timespec start, finish;
    struct rusage usage;
    clock_gettime(CLOCK_REALTIME, &start);
    
    pipe(fd);
    if ((pid = fork()) < 0) {
        error (1, 0, "Forking a child process failed");
        return -1;
    }

    if (pid == 0) {
        		//close(1);
    		//	printf("a start---------------\n");
        		dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                
                /* Send "string" through the output side of pipe */
                //printf("a start\n");
             	execute_command(c->u.command[0],profiling);
             	//printf("a end\n");
           
             	//printf("a end -------------\n");
                exit(1);
    }
    else{
        /* Parent process closes up output side of pipe */
        		//close(0);
    			close(0);
                dup2(fd[0],0);
                close(fd[0]);
                close(fd[1]);

              //printf("b start\n");
                //printf("b's type%d\n",c->u.command[1]->type);
                execute_command(c->u.command[1],profiling);
                //printf("b end\n");
                c->status = c->u.command[1]->status;                
                //printf("b end c1 status%d\n", c->u.command[1]->status);
    }

	//printf("parents wait begin\n");
	//close(fd[0]);
	//close(fd[1]);
    wait(pid);
    
    //finish time - profile after a process finishes
    clock_gettime(CLOCK_REALTIME, &finish);
    
    if (profiling == 0) { // write profiling info to log
        int exec_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
        getusage(RUSAGE_CHILDREN, &usage)
        log.write(finish, exec_time, usage.ru_utime, usage.ru_stime, c->u.word);
    }
    
	dup2(stdin_copy,0);
	close(stdin_copy);
    //printf("c0 status:%d\n",c->u.command[0]->status);
    //printf("c1 status:%d\n",c->u.command[1]->status);
    //printf("c  status:%d\n",c->status);
        //c->status = c->u.command[1]->status;
    // Redirect input from pipe    
    // execute_command(c->u.command[1],profiling);
    return 0;
}

void
execute_command (command_t c, int profiling)
{
  /* FIXME: Replace this with your implementation, like 'prepare_profiling'.  */
    pid_t pid;
    int status = 0;
    // Handle standard input/output redirection

    int stdin_copy  = dup(0);
    int stdout_copy = dup(1);
    int in = -1;
    int out = -1;
    if (c->input){
        close(0);
        in = open(c->input, O_RDONLY);
    } 
	if (c->output){
		close(1);
		out = open(c->output, O_WRONLY | O_APPEND | O_CREAT, 0666);
	}
		
    if (profiling == 0) {
        
    }
   
    switch (c->type){
	    	case SIMPLE_COMMAND:
	    		if (exec_simple_command(c, profiling) < 0)
	    			exit(1);
//	    		printf("simple command return %d\n",c->status);
	    		break;
	    	case SEQUENCE_COMMAND:
	    		execute_command(c->u.command[0],profiling);
	    		execute_command(c->u.command[1],profiling);
	    		c->status = c->u.command[1]->status;
	    		break;
	    	case SUBSHELL_COMMAND:
	    		execute_command(c->u.command[0],profiling);
	    		c->status = c->u.command[0]->status;
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
	        case PIPE_COMMAND:
	            if (exec_pipe_command(c, profiling) < 0)
	                ;//exit(1);
	            break;
	}

	if (in >= 0){
		close(in);
		dup2(stdin_copy,0);
	}
	if (out >=0){
		close(out);
		dup2(stdout_copy,1);
	}
	close(stdin_copy);
	close(stdout_copy);
}

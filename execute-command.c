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
#include <unistd.h>
#include <error.h>
#include <time.h>
#include "sys/time.h"
#include "sys/resource.h"

/* FIXME: You may need to add #include directives, macro definitions,
 static function definitions, etc.  */

struct timespec start;

int
prepare_profiling (char const *name)
{
    /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
    int f = open(name, O_WRONLY | O_APPEND | O_CREAT, 0666);
    clock_gettime(CLOCK_REALTIME, &start);
    return f;
}

int
command_status (command_t c)
{
    return c->status;
}

void getCmd(char** w,char* str){
    if (strncmp(*w,":") == 0){
        sprintf(str,"[%d]",getpid());
    }else{    
        sprintf(str,"%s",*w);
        while(*++w){
            sprintf(str,"%s %s",str,*w);
        }
    }
}


// Returns the time of a timeval in seconds
double time_in_sec(struct timeval x)
{
    double x_sec = x.tv_sec + x.tv_usec/1000000.0;
    return x_sec;
}

// Returns the time difference in seconds
double time_diff(struct timespec x , struct timespec y)
{
    double x_sec , y_sec , diff;
    x_sec = (double)x.tv_sec + (double)x.tv_nsec/1000000000;
    y_sec = (double)y.tv_sec + (double)y.tv_nsec/1000000000;
    diff = y_sec - x_sec;
    
    return diff;
}

void print_profiling (int profiling, struct timespec start, char *cmd) {
    struct timespec finish;
    struct rusage usage;
    char output[50000];

    clock_gettime(CLOCK_REALTIME, &finish);
    
    double finish_time = (double)finish.tv_sec + (double)finish.tv_nsec/1000000000;
    double exec_time = time_diff(start, finish);
    if (cmd == NULL) 
	    getrusage(RUSAGE_SELF, &usage);
    else
    	getrusage(RUSAGE_CHILDREN, &usage);
    double user_time = time_in_sec(usage.ru_utime);
    double system_time = time_in_sec(usage.ru_stime);
    
    sprintf(output,"%.2lf %.3lf %.3lf %.3lf", finish_time, exec_time, user_time, system_time);
    if (cmd == NULL) {
	    int pid = getpid();
    	sprintf(output,"%s [%d]\n",output,pid);
    }
    else {
	    sprintf(output,"%s %s\n",output,cmd);
    }
    write(profiling,output,strlen(output) > 1023? 1023 : strlen(output));
   // printf("%s",output);
}

int
finish_profiling (int fd) {
    print_profiling(fd, start, NULL);
    close(fd);
}

int exec_simple_command(command_t c, int profiling){
    pid_t pid;
    char cmd[10000];
    struct timespec start;
    struct timespec finish;
    //struct rusage usage;
    if (profiling >= 0)
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
    			char **w = c->u.word;
    			//printf("%s: command not found\n", *w);
    	                return -1;
	        }
        exit(0);
    }else {
        while (wait(&(c->status)) != pid)
             ;
        
        getCmd(c->u.word,cmd);
	    if (profiling >= 0) {
		   print_profiling(profiling, start, cmd);
	    }
    }
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
    int status;

    pipe(fd);
    if ((pid = fork()) < 0) {
        error (1, 0, "Forking a child process failed");
        return -1;
    }

    if (pid == 0) {
                dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                
             	execute_command(c->u.command[0],profiling);
             	
                exit(1);
    }
    else{
    			close(0);
                dup2(fd[0],0);
                close(fd[0]);
                close(fd[1]);

                execute_command(c->u.command[1],profiling);

                c->status = c->u.command[1]->status;                

    }
    waitpid(pid,NULL,0);
	dup2(stdin_copy,0);
	close(stdin_copy);
    return 0;
}

void
execute_command (command_t c, int profiling)
{
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
		
    if (profiling >= 0) {
        
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
	            {

                }
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

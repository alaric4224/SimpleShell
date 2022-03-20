#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "myshell_parser.h"

int main(int argc, char** argv){

char* comline = malloc(MAX_LINE_LENGTH + 1);
int comsize = MAX_LINE_LENGTH + 1;
struct pipeline* pipeline;
struct pipeline_command* comm;
int* status = NULL;
int silent = 1;
int pid;
int pipes[2];
int stdin_copy = dup(0);
int stdout_copy = dup(1);
int piping = 0;
int infile = 0;
int outfile = 0;
int indirect = 0;
int pipestore = -1;
int tmppipe = -1;
    
if(argc == 1) silent = 0; //Temp fix

while(1){

	if(!silent){
		printf("my_shell$");
	}

	if(fgets(comline, comsize, stdin) == NULL){
        close(stdin_copy);
        close(stdout_copy);
        fflush(stdin);
        free(comline);
        fflush(stdin);
        waitpid(-1, NULL, 0);
        return 0;
    }
    else{

	pipeline = pipeline_build(comline);

	//TODO: Piping logic with file tables

	comm = pipeline->commands;

    if(comm == NULL){free(comline);continue;}//empty input
    
    if(comm->redirect_in_path != NULL){//redirect in
        infile = open(comm->redirect_in_path, O_RDONLY);
        if(infile == -1){
            perror("ERROR: ");
            continue;
        }
        close(0);
        dup2(infile, 0);
        close(infile);
        indirect = 1;
    }
    
    if(comm->next != NULL){//piping
        piping = 1;
        pipe(pipes);
    }
        
    
	while(comm->next != NULL){
        
        pipe(pipes);
        
        if(piping == 2){
            tmppipe = pipes[0];
            pipes[0] = pipestore;
            pipestore = tmppipe;
        }
        
        if((pid = fork()) > 0){
        //parent
            if(piping == 1){
                pipestore = pipes[0];
            }
            
            if(pipeline->is_background){
                comm = comm->next;
                continue;
            } 
            waitpid(pid, status, 0);
        
        }
        else{ 
        //child
            if(piping == 1){
                close(1);
                dup2(pipes[1], 1);
                close(pipes[1]);
                close(pipes[0]);
            }
            
            if(piping == 2){
                close(0);
                dup2(pipes[0], 0);
                close(1);
                dup2(pipes[1], 1);
                close(pipes[1]);
                close(pipes[0]);
            }
            
            execvp(comm->command_args[0], comm->command_args);
            
            perror("ERROR: ");
        
        }
        
        if(indirect){
            dup2(stdin_copy, 0);
            indirect = 0;
        }
        
		comm = comm->next;
        if(piping){piping = 2;}
	}

    if(comm->redirect_out_path != NULL){
        outfile = open(comm->redirect_out_path, O_RDWR|O_CREAT, 0644);
        if(outfile == -1){
            perror("ERROR: ");
            continue;
        }
        close(1);
        dup2(outfile, 1);
        close(outfile);
    }
        
    if((pid = fork()) > 0){
        //parent
        if(pipeline->is_background){
                pipeline_free(pipeline);
                continue;
        }
        
        waitpid(pid, status, 0);
        
    }
    else{ 
        //child
        
        if(pipestore != -1){
            close(0);
            dup2(pipes[0], 0);
        }
        
        execvp(comm->command_args[0], comm->command_args);
         
        perror("ERROR: ");
        
    }
   
    if(indirect){
            dup2(stdin_copy, 0);
            indirect = 0;
    }    
    if(comm->redirect_out_path != NULL){
        dup2(stdout_copy, 1);
    }
        
    fflush(stdin);
	pipeline_free(pipeline);
    close(pipes[0]);
    close(pipes[1]);
    piping = 0;
    indirect = 0;
    pipestore = -1;
    tmppipe = -1;
    }
}

return 0;

}

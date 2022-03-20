#include "myshell_parser.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct pipeline *pipeline_build(const char *command_line)
{
	struct pipeline* pipe = (struct pipeline *) malloc(sizeof(struct pipeline));

	pipe->is_background = 0; //Initialize pipeline with default values
	pipe->commands = NULL;
    
    if(command_line[0] == '\0'){return pipe;}
    
	int j;
	const char* d1 = "|", *d2 = " \n\t", *d3 = "<> \n\t", *d4 = "<>";
	char* wptr, *pptr;
	char* inbt_comm = malloc(2*MAX_LINE_LENGTH);
    char* inbt_comm2 = malloc(2*MAX_LINE_LENGTH);
    char* inbt_comm3;
    strcpy(inbt_comm2, command_line);
    inbt_comm3 = inbt_comm2;
    char* fix;
    int first = 1;
    //char* outptr;
    //int redirect_out = 0;
    
    struct pipeline_command* curr_comm;
    //Create first command
	struct pipeline_command* new_comm = malloc(sizeof(struct pipeline_command));
	pipe->commands = new_comm;
	curr_comm = new_comm; //Current command pointer. Useful for iterating through to create multiple commands in one pipeline
	new_comm->command_args[0] = NULL;
	new_comm->redirect_in_path = NULL;
	new_comm->redirect_out_path = NULL;
	new_comm->next = NULL;
	char* instbit = malloc(2*MAX_LINE_LENGTH); //token holder for bits of instructions
    
    if((fix = strchr(inbt_comm3, '&')) != NULL){ //Parse background commands
        strcpy(fix, fix + 1);
        pipe->is_background = 1;
    }
 
    strcpy(inbt_comm, inbt_comm3);
    free(inbt_comm2);
    
    if((strchr(inbt_comm, '<')) && (strchr(inbt_comm, '>')) && !(strchr(inbt_comm, '|'))){ //For single commands with a redirect in and a redirect out
        char* inarg = malloc(sizeof(strlen(inbt_comm)));
        char* outarg = malloc(sizeof(strlen(inbt_comm)));
        char* inst = strtok_r(inbt_comm, d4, &wptr);
        int j = 0;
        //while(inst != NULL){////
            if(first){first = 0;free(instbit);}
            char* instbit = malloc(2*MAX_LINE_LENGTH);
            instbit = strtok_r(inst, d2, &pptr); 
            while(instbit != NULL){
                char* normarg = malloc(sizeof(strlen(inbt_comm)));
                strcpy(normarg, instbit);
                curr_comm->command_args[j] = normarg;
                j++;
                instbit = strtok_r(NULL, d2, &pptr); 
            }
            curr_comm->command_args[j] = NULL;
            inst = strtok_r(NULL, d4, &wptr);
            if(inst[0] == ' '){inst++;}
            if(inst[strlen(inst) - 1] == ' ' || inst[strlen(inst) - 1] == '\n' || inst[strlen(inst) - 1] == '\t'){inst[strlen(inst) - 1] = '\0';}
            strcpy(inarg, inst);
            curr_comm->redirect_in_path = inarg;
            inst = strtok_r(NULL, d4, &wptr);
            if(inst[0] == ' '){inst++;}
            if(inst[strlen(inst) - 1] == ' ' || inst[strlen(inst) - 1] == '\n' || inst[strlen(inst) - 1] == '\t'){inst[strlen(inst) - 1] = '\0';}
            strcpy(outarg, inst);
            curr_comm->redirect_out_path = outarg;
           //if(inst != NULL){}  
        //}
        
        //free(inst);   
    }
    else{
        
    char* inst = strtok_r(inbt_comm, d1, &wptr);
        
	while(inst != NULL){ //This will loop until the end of time-- I mean, the end of the entered command line argument, signaling that we have finished entering commands into our pipeline
        if(first){first = 0;free(instbit);}
        char* instbit = malloc(2*MAX_LINE_LENGTH);
		strcpy(instbit, inst);
		instbit = strtok_r(instbit, d2, & pptr);
        //char* copy = instbit;
		j = 0; //j will help to track our input arguments so that we can make the next one in the array NULL

		while(1){
            
			if(!strcmp(instbit, "\n") || strlen(instbit) == 0 || !strcmp(instbit, " ")){
			
			}
            else if((fix = strchr(instbit, '<')) != NULL){
                if(instbit[0] == '<' && instbit[1] == '\0'){
                    instbit = strtok_r(NULL, d2, &pptr);
                    char* inptr = malloc(strlen(instbit) + 1);
                    strcpy(inptr, instbit);
                    curr_comm->redirect_in_path = inptr;   
                }
                else if(instbit[0] == '<'){
                    instbit++;
                    char* inptr = malloc(strlen(instbit) + 1);
                    strcpy(inptr, instbit);
                    curr_comm->redirect_in_path = inptr;                     
                }
                else if(fix[1] == '\0'){
                    char* newarg = malloc((strlen(instbit))*sizeof(char));
                    strcpy(newarg, strtok(instbit, d3));
                    curr_comm->command_args[j] = newarg;
                    j++;
                    instbit = strtok_r(NULL, d3, &pptr);
                    char* inptr = malloc(strlen(instbit) + 1);
                    strcpy(inptr, instbit);
                    curr_comm->redirect_in_path = inptr; 
                }
                else{
                    char* newarg = malloc((strlen(instbit))*sizeof(char));
                    char* inptr = malloc(strlen(instbit) + 1);
                    strcpy(newarg, strtok(instbit, d3));
                    curr_comm->command_args[j] = newarg;
                    j++;
                    strcpy(inptr, fix+1);
                    curr_comm->redirect_in_path = inptr;
                }
			}
            else if((fix = strchr(instbit, '>')) != NULL){
                if(instbit[0] == '>' && instbit[1] == '\0'){
                    instbit = strtok_r(NULL, d2, &pptr);
                    char* outptr = malloc(strlen(instbit) + 1);
                    strcpy(outptr, instbit);
                    curr_comm->redirect_out_path = outptr;   
                }
                else if(instbit[0] == '>'){
                    instbit++;
                    char* outptr = malloc(strlen(instbit) + 1);
                    strcpy(outptr, instbit);
                    curr_comm->redirect_out_path = outptr;  
                }
                else if(fix[1] == '\0'){
                    char* newarg = malloc((strlen(instbit))*sizeof(char));
                    strcpy(newarg, strtok(instbit, d3));
                    curr_comm->command_args[j] = newarg;
                    j++;
                    instbit = strtok_r(NULL, d3, &pptr);
                    char* outptr = malloc(strlen(instbit) + 1);
                    strcpy(outptr, instbit);
                    curr_comm->redirect_out_path = outptr; 
                }
                else{
                    char* newarg = malloc((strlen(instbit))*sizeof(char));
                    char* outptr = malloc(strlen(instbit) + 1);
                    strcpy(newarg, strtok(instbit, d3));
                    curr_comm->command_args[j] = newarg;
                    j++;
                    strcpy(outptr, fix+1);
                    curr_comm->redirect_out_path = outptr;
                }
			}
			else{
				char* newarg = malloc((strlen(instbit)+1)*sizeof(char));
				strcpy(newarg, instbit);
				curr_comm->command_args[j] = newarg;
				j++;
			} //logic to (hopefully) fill out the command according to the instruction. I am tired.
            
			instbit = strtok_r(NULL, d2, &pptr);
            if(instbit == NULL){break;}//free(copy); break;}
		}
        
		curr_comm->command_args[j] = NULL;

		inst = strtok_r(NULL, d1, &wptr);

		if(inst != NULL){	
			struct pipeline_command* new_comm = malloc(sizeof(struct pipeline_command));
			curr_comm->next = new_comm;
			curr_comm = new_comm;
			new_comm->command_args[0] = NULL;
			new_comm->redirect_in_path = NULL;
			new_comm->redirect_out_path = NULL;
			new_comm->next = NULL;	
		} //Create new command (if needed) and initialize it

	}
    
    //if(redirect_out){
    //    curr_comm->redirect_out_path = outptr;
    //}
    
	free(inst);
    }
    free(inbt_comm);
	//free(instbit);
	return pipe;
}

void pipeline_free(struct pipeline *pipeline)
{

	int j;
	struct pipeline_command* com, *tmp;

	if(pipeline->commands != NULL){
	
		com = pipeline->commands;
	
		while(com->next != NULL){
		
			j = 0;
		
			if(com->redirect_out_path != NULL){
				free(com->redirect_out_path);
			}
			if(com->redirect_in_path != NULL){
				free(com->redirect_in_path);
			}

			while(com->command_args[j] != NULL){
				free(com->command_args[j]);
				j++;
			}
			
			tmp = com;
			com = com->next;
			free(tmp);

		}
	
	
		j = 0;
		
		if(com->redirect_out_path != NULL){
			free(com->redirect_out_path);
		}
		if(com->redirect_in_path != NULL){
			free(com->redirect_in_path);
		}

		while(com->command_args[j] != NULL){
			free(com->command_args[j]);
			j++;
		}

		free(com);

	}

	free(pipeline);

}

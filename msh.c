#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
								// so we need to define what delimits our tokens
								// In this case  white space
								// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 10    // Mav shell only supports 10 arguments.

///---REQUIREMENT 12 FUNCTION CALL---///

//functon name: handle_signal
//Expects integer constant value of the signal
//Description: checks if the signal value is from
//one of the predefined functions.
static void handle_signal (int sig )  

{
  /*
   Determine which of the three signals were caught including SIGCONT 
   and create cases for SIGINT & SIGTSTP which prints prompt(msh>) in 
the next line.
  */
  switch( sig )
  {
	case SIGINT: 
	  printf("\n");
	break;

	case SIGTSTP: 
	  printf("\n");
	break;

  }
}

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
   /* Dynamically allocate command
	 line string tsize 255 */   
	                                                     
  pid_t pid; // Datatype pid_t to represent process ID
  int ret_status;
  int hc=1;
  int h,n,pc;
  int p=0;
  pid_t pids[15]; // To store upto 15 process IDs
  char history[50][255]; // 2-D Array to save last 50 strings of Max 255 size.
  struct sigaction act;

   // Zero out the sigaction struct //    
  memset (&act, '\0', sizeof(act));

  // Set the handler to use the function handle_signal() //  
  act.sa_handler = &handle_signal; 

  // Install the handler for SIGINT,SIGTSTP and check the return value 

  if (sigaction(SIGINT , &act, NULL) < 0)   
  {
	perror ("sigaction: ");
	return 1;
  }

  if (sigaction(SIGTSTP , &act, NULL) < 0) 
  {
	perror ("sigaction: ");
	return 1;
  }
  while( 1 )
  {
	// Print out the msh prompt

	printf ("msh> ");

	// Read the command from the commandline.  The
	// Maximum command that will be read is MAX_COMMAND_SIZE
	// This while command will wait here until the user
	// inputs something since fgets returns NULL when there
	// is no input
	while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

	/* Parse input */
	char *token[MAX_NUM_ARGUMENTS];

	int   token_count = 0;                                 
														   
	// Pointer to point to the token
	// Parsed by strsep
	char *arg_ptr;                                         
														   
	char *working_str  = strdup( cmd_str );                

	// we are going to move the working_str pointer so
	// keep track of its original value so we can deallocate
	// the correct amount at the end
	char *working_root = working_str;

	// Tokenize the input strings with whitespace used as the delimiter
	// Also check for token_count to not exceed max arguments
	while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
			  (token_count < MAX_NUM_ARGUMENTS))
	{
	  token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
	  if( strlen( token[token_count] ) == 0 )
	  {
		token[token_count] = NULL;
		token_count--;
	  }
	  token_count++;
	}
	if (token_count==0) continue; //use continue and return to whileloop 
								  //Avoid segfault as we assign it NULL 
								  //and access token[0] in the next line.

	////----------------MY LOGIC STARTS HERE------------------/////

	///---REQUIREMENT 14---///

	//strcmp returns 0 value so use ! in if
	//compares the Tokenized input with string listpids
	//if equal moves ahead in the loop
	//prints only saved process IDs from the pids array
	//prints no more than 15 pids

	 if(!strcmp(token[0],"listpids"))
	  {for(pc=0; pc<p; pc++)
		{
	  if(pc<15)
		 { 
		printf("%d:%d\n",pc,pids[pc]);
		 }
		}  
	continue;

	  }
	  ///---REQUIREMENT 8---///
	  //Compares the Tokenized input with string input bg
	  //if equal strcmp returns 0 and enters loop
	  //Installs the handler for SIGACTION and checks for return value
	 if(!strcmp(token[0],"bg"))
	{
	  sigaction(SIGCONT,&act,NULL);
	 continue;
	}
	///---REQUIREMENT 15 !n ---///
	// If token '0' has ! as its first character, enters the if loop
	if(token[0][0]=='!')
	{ 
	//newtok here saves the second character 
	// n lies between 1 to 15 so assign it's array size 2.
	  char newtok[2]; 
	  // To skip '!' when in while loop give ati = 1
	  int ati=1;
	  int atix = 0;
	  //checking in the while loop if the string is NULL terminated
	  // IF NULL terminated exits loop
	  // We need this to take input for only 2 digits after '!'
	  // save the input after ! in newtok array
	  while(token[0][ati]!='\0')
	  {   
		newtok[atix]=token[0][ati];
		ati++;
		atix++;
	  }
	  // converting array of newtok to integer and saving as n
	  n = atoi(newtok);
	  // As my list of history starts with 1 i added these condition
		if(n<=0 || n>(hc-1))
	  {
		if(atix>2 || n > 50)
		{
		  printf("command not executable beyond last 50!\n" );
		  continue;
		}
		printf("command not in history\n" );
		continue;
	  }
	  //---Executes history[n]---//
	  // By using the same concept above for tokenization
	  // implement the same for history array and duplicate it into token array
	  // as this is in iftoken[0][0] loop and continue is called, it runs execvp with token[0
	  // as history[1].
	  int xtoken = 0;
	  char * history_str = (char*) malloc( MAX_COMMAND_SIZE );
	  history_str = strdup(history[n]); // storing string from history
	  while ( ( (arg_ptr = strsep(&history_str, WHITESPACE ) ) != NULL) && (xtoken < MAX_NUM_ARGUMENTS))
	  {
		 token[xtoken] = strndup( arg_ptr, MAX_COMMAND_SIZE );
		 
		if( strlen( token[xtoken] ) == 0 )
		{
		  token[xtoken] = NULL;
		  xtoken--;
		}
		  xtoken++;
	  }
	 
	}
	// copy command line into history array
	// prints history upto 50 times
	strcpy(history[hc++],cmd_str);
	if(hc>50) hc=1;
	
	///---REQUIREMENT 15---///
	//Compares token[0] with the string history
	//prints the above sabve history array
	if(!strcmp(token[0],"history"))
	{
	  for(h=1; h<hc; h++)
	  {
		printf("%d:%s",h,history[h]);
	  }
	  continue; 
	}
	///---REQUIREMENT 13---/// 
	//compares token[0] with string cd
	//calls inbuilt function chdir to change directory to next token

	if(!strcmp(token[0],"cd"))
	{
	   chdir(token[1]);
	 continue;
	}

	if (!strcmp(token[0], "exit" )) exit(0); // returns 0, and exits
	if (!strcmp(token[0], "quit" )) exit(0); 
	pid = fork(); 
	// creates a new process
	// if pid is a postive integer i.e parent , initialize wait
	// else system call execvp
	if (pid)
	{   
		pids[p++]=pid;
		 waitpid(pid,&ret_status,0);
	}
	 else 
	  {   
		 execvp(token[0],token ); 
		{
		   while(-1)          
		  { 
		   printf("%s: command not found\n",*token); 
		   return 0;         
		  } 
		  
		}        
		  exit(0);          
	  }  
		 //Dealloc the working root which contains our duplicated input string
		 // better to deallocate as it saves in HEAP.
	   free( working_root );    
  }
  return 0;
}



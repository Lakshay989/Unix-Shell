#include "shelpers.hpp"

int main (int argc, char *argv[]) 
{
	//fork;
	std::string line; 
	while(std::getline(std::cin, line))
	{
	    std::vector<std::string> input ;
		std::vector<Command> commands ;
		input = tokenize(line) ;		
		commands = getCommands(input) ;


        std::vector<pid_t> backgroundPids; // To store the pids of the background processes
        std::vector<pid_t> foregroundPids; // To store the pids of the foreground processes

		for (int i = 0; i < commands.size(); i++) 
		{
            if(commands[i].exec == "exit")
		    {
	    	    // Clean up
			    exit(1) ;
		    }

            if (commands[i].exec == "cd")
            {
                const char* directory ; // = getenv("HOME") ; // string directory
                if(commands[i].argv.size() > 2)
                {
                    directory = commands[i].argv[1] ;
                }
                else
                {
                    directory = getenv("HOME");
                }
            
                int rc = chdir(directory) ;
                if (rc < 0 )
                {
                    perror("chdir failed") ;
                    exit(1) ;    
                }
            }

			pid_t pid = fork();

            foregroundPids.push_back(pid) ;

			if(pid == -1)
			{
				perror("Fork failed");
				return 1 ;
			}
			else if (pid == 0)
			{
				if(commands[i].fdStdin != 0 )
				{
					if(dup2(commands[i].fdStdin, 0) < 0)
					{
						perror("dup2() failed");
						exit(1) ;
					}
				}

				if(commands[i].fdStdout != 1 )
				{
					if(dup2(commands[i].fdStdout, 1) < 0)
					{
						perror("dup2() failed");
						exit(1) ;
					}	
				}

                if (commands[i].fdStdin != 0) 
				{
                    int close_syscall1 = close(commands[i].fdStdin);
					if(close_syscall1 == -1)
					{
						perror("close failed");
						exit(1) ;
					}
            	}

	            if (commands[i].fdStdout != 1) 
				{
    	            int close_syscall2 = close(commands[i].fdStdout);
					if(close_syscall2 == -1)
					{
						perror("close failed");
						exit(1) ;
					}
        	    }
        	

		        char* args[commands[i].argv.size() +1 ];		// +1	// Creating a new char* array

    		    for (int j = 0; j < commands[i].argv.size(); j++)  	// Copy the contents of the vector into the array
	    		{
		    		args[j] = const_cast<char*>(commands[i].argv[j]);
			    }	

    			int exec_call = execvp( commands[i].exec.c_str(), args) ; // const_cast<char*>(commands[0].argv) // const_cast<char*>(commands[0].argv.data()

	    		if((exec_call) == -1)
		    	{
			    	perror("execvp failed") ;
				    // How to clear all ?
				    return 1 ;
			    }
			    return 0 ;
            }
			
		    else
            {
                if (commands[i].background) 
                {
                    backgroundPids.push_back(pid);
                    std::cout << "pid = " << pid << " is running in the background" << std::endl;
                }

                if(!commands[i].background)
                {		            
                    for(pid_t pid : foregroundPids){
                        int status ;
		                // Wait for children to finish executing	
		                //int wait_syscall = waitpid(pid, &status, WUNTRACED);
		                int wait_syscall = waitpid(pid, &status, WNOHANG);
                        if(wait_syscall < 0) 			    
                        {
		                    perror("Error occured while waiting for children processes");
                            exit(1);
    	                }
                    }
	            }
                else
                {
                    for (int i = 0; i < backgroundPids.size(); i++) 
                    {
                        pid_t pid = backgroundPids[i];
                        int status;
                        if (waitpid(pid, &status, WNOHANG) >= 0) 
                        { 
                            // process has completed
                            std::cout << "Background process with PID " << pid << " has completed" << std::endl;
                            backgroundPids.erase(backgroundPids.begin() + i);
                        }
                        else
                        {
                            perror("wait failed");
                            exit(1) ;
                        }
                    }
                }

                if (commands[i].fdStdin != 0) 
			    {
                    int close_syscall1 = close(commands[i].fdStdin);
			        if(close_syscall1 == -1)
			        {
            	        perror("close failed");
				        exit(1) ;
			        }
      	        }

                if (commands[i].fdStdout != 1) 
                {
    	            int close_syscall2 = close(commands[i].fdStdout);
			        if(close_syscall2 == -1)
			        {
    			        perror("close failed");
				        exit(1) ;
			        }
	            }
            }
        }
    }
	return 0 ;
}

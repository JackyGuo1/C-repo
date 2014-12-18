#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

void open_url(char* url)
{
	char launch[255];
	sprintf(launch,"CMD /c start %s", url);
	system(launch);
	sprintf(launch,"x-www-browser '%s' &",url);
	system(launch);
	sprintf(launch,"open '%s'",url);
	system(launch);
}


int main(int argc,char * argv[])
{
	char *phrase = argv[1];
	char *vars[] ={"RSS_FEED=http://www.chinadaily.com.cn/rss/entertainment_rss.xml",NULL};
//                        "http://www.chinadaily.com.cn/rss/bizchina_rss.xml",
//                       "http://www.chinadaily.com.cn/rss/world_rss.xml"};

	int fd[2];
	if(pipe(fd) ==-1){
		error("Can't create the pipe");
	}
	
	printf("fd: %d %d\n",fd[0],fd[1]);

	pid_t pid = fork();
	
	if(pid == -1){
		error("Can't fork process");
	}

	if(!pid){ //child process
		dup2(fd[1],1);
		close(fd[0]);
		printf( "Child start\n");
		if(execle("/usr/bin/python","/usr/bin/python","./rssgossip.py","-u",
                                        phrase,NULL,vars) ==-1){
			error("Can't run script");
		}
	}
	
	dup2(fd[0],0);
	close(fd[1]);

	char line[255];
	while(fgets(line,255,stdin)){ // stdin = fd[0]
		printf("%s\n",line);
		if(line[0] == '\t')
			open_url(line+1);
	}
	int pid_status;
	if (waitpid(pid,&pid_status,0) ==-1){
		error("Error waiting for child process");
	}

	return 0;
}

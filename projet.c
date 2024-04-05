#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define clear() printf("\033[H\033[J")


#define INPUT 0
#define OUTPUT 1
#define APPEND 2
int i=0;


void init_shell()
{
clear();
printf("\n\n\n\n******************"
"**********************");
printf("\n\n\n\t****MY SHELL****");
printf("\n\n\n\n*******************"
"*********************");
char* username = getenv("USER");
printf("\n\n\nUSER is: @%s", username);
printf("\n");
sleep(1);
clear();
}

void removeSpace(char* buf)
{
    if(buf[strlen(buf)-1]==' ' || buf[strlen(buf)-1]=='\n')
        buf[strlen(buf)-1]='\0';
    if(buf[0]==' ' || buf[0]=='\n')
        memmove(buf, buf+1, strlen(buf));
}



void tokenize_buffer(char** param,int *nr,char *buf,const char *c)
{
    char *token;
    token=strtok(buf,c);
    int pc=-1;
    while(token)
    {
        param[++pc]=malloc(sizeof(token)+1);
        strcpy(param[pc],token);
        removeSpace(param[pc]);
        token=strtok(NULL,c);
    }
    param[++pc]=NULL;
    *nr=pc;
}



void executeBasic(char** argv)
{
    if(fork()>0) 
    {
        wait(NULL);
    }
    else 
    {
        execvp(argv[0],argv);
            
            perror("invalid input (commande n'est pas valide ) "   "\n");
        exit(1);
    }
}


void executePiped(char** buf,int nr) 
{
    if(nr>10) return;

    int fd[10][2],i,pc;
    char *argv[100];

    for(i=0; i<nr; i++)
    {
        tokenize_buffer(argv,&pc,buf[i]," ");
        if(i!=nr-1)
        {
            if(pipe(fd[i])<0)
            {
                perror("pipe creating was not successfull\n");
                return;
            }
        }
        if(fork()==0) 
        {
            if(i!=nr-1)
            {
                dup2(fd[i][1],1);
                close(fd[i][0]);
                close(fd[i][1]);
            }

            if(i!=0)
            {
                dup2(fd[i-1][0],0);
                close(fd[i-1][1]);
                close(fd[i-1][0]);
            }
            execvp(argv[0],argv);
            perror("invalid input ");
            exit(1);
        }
        
        if(i!=0) 
        {
            close(fd[i-1][0]);
            close(fd[i-1][1]);
        }
        wait(NULL);
    }
}


void executeAsync(char** buf,int nr)
{
    int i,pc;
    char *argv[100];
    for(i=0; i<nr; i++)
    {
        tokenize_buffer(argv,&pc,buf[i]," ");
        if(fork()==0)
        {
            execvp(argv[0],argv);
            perror("invalid input ");
            exit(1);
        }
          else 
    {
        wait(NULL);
    }
    }


}




void cmdOrcmd(char** buf,int nr){
int i,pc;
    char *argv[100];
    
        tokenize_buffer(argv,&pc,buf[0]," ");
        if(fork()==0)
        {
       int ok=0;
          if (execvp(argv[0],argv) !=-1){
             ok=1;
           }
           
            tokenize_buffer(argv,&pc,buf[1]," ");
            if (ok==0){
            	if(execvp(argv[0],argv)==-1)
            	{
          perror("les deux commandes sont fausses ");
            exit(1);
             }
            	
             }
        }
    
    
        wait(NULL);
    
}

void cmdAndcmd(char** buf,int nr){
int i,pc;
    char *argv[100];
    
        tokenize_buffer(argv,&pc,buf[0]," ");
        if(fork()==0)
        {
       int ok=0;
          if (execvp(argv[0],argv) ==-1){
             ok=1;
           }
           
            tokenize_buffer(argv,&pc,buf[1]," ");
            if (ok==0){
            	if(execvp(argv[0],argv)==-1)
            	{
          perror("les deux commandes sont fausses ");
            exit(1);
             }
            	
             }
        }
    
    
        wait(NULL);
    
}





void executeRedirect(char** buf,int nr,int mode)
{
    int pc,fd;
    char *argv[100];
    removeSpace(buf[1]);
    tokenize_buffer(argv,&pc,buf[0]," ");
    if(fork()==0)
    {

        switch(mode)
        {
       
        case OUTPUT:
            fd=open(buf[1],O_CREAT | O_WRONLY,0777);
            break;
        
        default:
            return;
        }

        if(fd<0)
        {
            perror("cannot open file\n");
            return;
        }

        switch(mode)
        {
        
        case OUTPUT:
            dup2(fd,1);
            break;
        
        default:
            return;
        }
        execvp(argv[0],argv);
        perror("invalid input ");
        exit(1);
    }
    wait(NULL);
}




int main(char** argv,int argc)
{
    char buf[500],*buffer[100],buf2[500],buf3[500], *params1[100],*params2[100],*token,cwd[1024];
    int nr=0;

   init_shell();

    while(1)
    {

       
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf( "%s%%  " , cwd);
        else 	perror("getcwd failed\n");

        
        fgets(buf, 500, stdin);

        
        if(strstr(buf,"||")) 
        {
            tokenize_buffer(buffer,&nr,buf,"||");
           // if(nr==2)
            cmdOrcmd(buffer,nr);
           // else 
           // printf("Incorrect output redirection!(has to to be in this form: command >> file)");
        }
        
      
      
        else if(strchr(buf,';')) 
        {
            tokenize_buffer(buffer,&nr,buf,";");
            executeAsync(buffer,nr);
        }
        
        else if(strchr(buf,'>')) 
        {
            tokenize_buffer(buffer,&nr,buf,">");
            if(nr==2)
            executeRedirect(buffer,nr, OUTPUT);
            else
            printf("Incorrect output redirection!(has to to be in this form: command > file)");
        }
        
          else if(strchr(buf,'|')) 
        {
            tokenize_buffer(buffer,&nr,buf,"|");
            executePiped(buffer,nr);
        }
        else if(strstr(buf,"&&")) 
        {
            tokenize_buffer(buffer,&nr,buf,"&&");
            if(nr==2)
            cmdAndcmd(buffer,nr);
            else 
            printf("Incorrect output redirection!(has to to be in this form: command >> file)");
        }
        
        else if(strstr(buf,"./")){

                tokenize_buffer(params1,&nr,buf," ");

                FILE* temp1;
                if(temp1=fopen(params1[0],"r"))
                { printf("Mode shell:\n");
                char ch[200];

                while(fgets(ch,sizeof ch,temp1)!=NULL)
                {
                    printf("%d. %s\n",++i,ch);

                }
                   executeBasic(params1);
                }
                else
              {
               perror(   "invalid input (fichier batsh n'existe pas) "   );
              exit(0);
              }
		}

        
        else 
        {
               
            tokenize_buffer(params1,&nr,buf," ");


            FILE* temp=fopen("/home/lassaad/history.txt","a+");
            fprintf(temp,"%s\n",params1[0]);
            fclose(temp);





            if(strstr(params1[0],"cd"))
            {
                chdir(params1[1]);
            }

            else if(strstr(params1[0],"history"))
            {

                temp=fopen("/home/lassaad/history.txt","r");
                char ch[10];

                while(fscanf(temp,"%s\n",ch)==1)
                {
                    printf("%d. %s\n",++i,ch);
                }

            }
           
            else if(strstr(params1[0],"quit")) 
            {
                exit(0);
            }
            else executeBasic(params1);
        }
    }

    return 0;
}






























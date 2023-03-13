#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#define OK	0
#define NOT_OK	-1

int archivos(char * path, ino_t d_ino)
{
    DIR     *dirp;
    struct dirent   *dp;
    struct stat     buf;
    int     i,k;
    char fich[512];
    const char* actual = ".";
    const char* padre = "..";

    if ((dirp = opendir(path)) == NULL)
    {
        perror (path) ;
        return NOT_OK ;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        sprintf (fich, "%s/%s",path, dp->d_name) ;
	if (strcmp((const char *)dp->d_name,actual)&&strcmp((const char *)dp->d_name,padre))
        {
        	if   (stat(fich,&buf) == OK)
        	{
            		if  ((buf.st_mode & S_IFMT) == S_IFDIR)
            		{   
                		if  (archivos(fich, d_ino) == NOT_OK) 
                		{   
                    			fprintf (stderr, "archivos: llamada archivos (fich=%s, d_ino=%d) resulto NOT_OK\n", fich, (int)d_ino);
                		}
            		}
            		else
            		{
                		if  (buf.st_ino  ==  d_ino)
					printf ("\t%s\n",fich);
            		}
        	}
        	else 
        	{ 
            		fprintf (stderr ,"archivos llamada stat(fich=%s,&buf) resulto NOT_OK\n",  fich); 
         	   	return NOT_OK;
        	}
	}
    }
    closedir(dirp) ;
}


int main (int argc, char *argv[])
{
    int i;
    char path[512]= ".";
    struct stat buf;

    if (argc < 3)
    {
        fprintf (stderr, "Sintaxis: %s enlaces fich1 fich2 path\n", argv[0]) ; 
        exit (1);
    }
    else if(stat(argv[argc-1],&buf) == OK)
        	if ((buf.st_mode & S_IFMT) != S_IFDIR)
        	{
            		fprintf (stderr, "path %s no es un directorio\n", argv[argc-1]) ; 
           	 	fprintf (stderr, "Sintaxis: enlaces fich1 fich2 path\n", argv[0]); 
            		exit(1);
        	}

    for (i=1; i<argc-1; i++)
    {
        if(stat(argv[i],&buf) == OK)
        {
            if ((buf.st_mode & S_IFMT) != S_IFDIR)
            {
                if  (buf.st_nlink > 1)
                {   printf ("El numero de enlaces al fich %s son %d\n" , argv[i] , buf.st_nlink) ;
                    printf("Su numero de i-nodo es: %d y el nombre de los archivos\n",(int)buf.st_ino);
                    printf ("enlazados a partir del directorio %s son: \n" , argv[argc-1]);
                    if (archivos(argv[argc-1], buf.st_ino) == NOT_OK)
                    {
                        fprintf (stderr, "main(): llamada a un archivos (argv[argc-1])= %s, buf.st_ino= %d): NOT_OK\n", argv[argc-1], (int)buf.st_ino);
                    }
                }        
            }
            else
                fprintf(stderr, "%s es un directorio y este programa no esta buscando sus links\n", argv[i]);
        }
        else
              fprintf(stderr, "El archivo %s no existe\n", argv[i]);
    }
    return(0);
}


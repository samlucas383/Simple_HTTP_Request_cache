#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>

const int host_name_size = 50; 
const int html_buffer_size = 100000; 


char* gethost(char host_name[host_name_size])
{
	struct hostent *server; 
	struct in_addr **addr_list; 
	struct in_addr addr; 
	char *ip_addr = malloc(host_name_size); 

	server = gethostbyname(host_name); 	
	ip_addr = inet_ntoa(*(struct in_addr*)server->h_addr);
	return ip_addr; 
			
}

void writetofile(char html_buffer[html_buffer_size], char *whole_date, char host_name[host_name_size]){
        FILE *file;
	FILE *instr_list;
	char *file_name = malloc(20);
	char *inst_name = malloc(40); 
	sprintf(file_name, "%s.txt", whole_date); 	
        file = fopen(file_name, "w+");
        fprintf(file, html_buffer);
	
	instr_list = fopen("list.txt", "a"); 
	sprintf(inst_name, "%s-%s\n", host_name, whole_date); 
	fprintf(instr_list, inst_name);

	fclose(instr_list); 
	fclose(file); 

}

char splitstring(char html_buffer[html_buffer_size], char host_name[host_name_size])
{
	char *word_ptr;
	char *date_ptr = malloc(20); 
	char *month = malloc(10); 
	char new_buffer[html_buffer_size];
	strcpy(new_buffer, html_buffer); 	
        char *token = strtok(new_buffer, "\n");
	char *token2 = strtok(NULL, "\n");
	word_ptr = strstr(token2, "Date"); 
	if(word_ptr != NULL)
	{
		strcpy(date_ptr, token2); 	
	}
	char *token3 = strtok(NULL, "\n");
	word_ptr = strstr(token3, "Date");
        if(word_ptr != NULL)
        {
		strcpy(date_ptr, token3);
        }

	char *token4 = strtok(NULL, "\n");
	word_ptr = strstr(token4, "Date");
        if(word_ptr != NULL)
        {
		strcpy(date_ptr, token4);
        }

	char *token5 = strtok(NULL, "\n");
 	word_ptr = strstr(token5, "Date");
        if(word_ptr != NULL)
        {
		strcpy(date_ptr, token5);
        }

	month = strtok(date_ptr, " "); 
	char *day_word = strtok(NULL, " "); 
	char *day_num = strtok(NULL, " "); 
	month = strtok(NULL, " ");
	char *year = strtok(NULL, " ");
	char *hour = strtok(NULL, ":");
	char *minute = strtok(NULL, ":");
	char *second = strtok(NULL, " ");
	
       	char *month_num = strstr(month, "Feb"); 
	if(month_num != NULL)
	{
		strcpy(month_num, "02"); 
	}
	char *whole_date = malloc(30); 
	sprintf(whole_date,"%s%s%s%s%s%s", year , month_num, day_num , hour , minute , second); 
	writetofile(html_buffer, whole_date, host_name);
}


char* check_list(char host_name[host_name_size])
{
	FILE *file = fopen("list.txt", "r+"); 
	int count = 0; 
	char c;
       	char line[40];
	char line2[40]; 
	char line_array[6][50]; 
	char *word_ptr; 
	char *ret_line = malloc(50); ;  
	char ch = "ch";
	
	
	while(fgets(line, 40,file))
	{
		strcpy(line_array[count], line);	
		count++; 
	} 
	

	fclose(file); 
	FILE *file2 = fopen("list.txt", "w+"); 

	if(count >= 6){	
		for(int i=1; i<count; i++)
		{
			fprintf(file2, line_array[i]); 
		       	
		}
	}
	
	else{
		for(int i =0; i < count; i++)
   	       {
   			fprintf(file2, line_array[i]);
                }
	}
       	
		
       	fclose(file2); 	
	FILE *file3 = fopen("list.txt", "r+"); 
	while(fgets(line, 40, file3))
	{
		word_ptr = strstr(line, host_name);
                if( word_ptr != NULL)
                {
                        strcpy(ret_line, line);
			return(ret_line);

                }
        }
	strcpy(ret_line, "empty"); 
	return(ret_line);
}	



int main()
{

	struct sockaddr_in client;
	struct sockaddr_in server; 
	int socket_desc; 
	int port, n; // port number to be used 
	char host_name[host_name_size]; // holds name of host  
	char html_buffer[html_buffer_size]; // html buffer
	char request[host_name_size];
	int run = 1;
       	char *ret_line = "empty"; 	
	
	 
	
	while(run == 1){
		socket_desc = socket(AF_INET, SOCK_STREAM, 0); // creates descriptor returns value
		printf("Enter host name: "); 
		scanf("%s", &host_name);

		if(strcmp(host_name, "quit") != 1){
			printf("Goodbye\n"); 
			return 0;  
		}

		printf("Enter post number: "); 
		scanf("%d", &port);
				
		ret_line = check_list(host_name);
	       	//printf("%s\n", ret_line); 	
		if(strcmp(ret_line,"empty") != 0){
			printf("Website already cached as: %s\n", ret_line); 
		} 
		else{
			if(socket_desc < 0)
			{
				printf("Error opening socet\n"); 
			}
			else{
				printf("Successfully opened socket\n"); 
			} 
	
			bzero((char *) &server, sizeof(server)); 
			server.sin_family = AF_INET; 
			server.sin_port = htons(port);
			inet_pton(AF_INET,gethost(host_name),&(server.sin_addr)); 

			if(connect(socket_desc, (struct sock *) &server, sizeof(server)) < 0){
				printf("Error in conecting\n"); 
			}
			else{ 
				printf("successfully connected\n"); 
			}
	
			sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\n\r\n\0", &host_name);

			write(socket_desc, request, strlen(request)); // write to server 

			bzero(html_buffer, html_buffer_size); // clear buffer just in case 

			sleep(1); // sleep to wait on all packets to be sent 

			while(n = read(socket_desc, html_buffer, sizeof(html_buffer)) > 0){
				splitstring(html_buffer, host_name);	
	       			bzero(html_buffer, html_buffer_size);  	
			}  

			close(socket_desc); 
		} 
	}
	return 0;
}

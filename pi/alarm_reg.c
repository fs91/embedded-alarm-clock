/* example_reg.c - main */

/********************************************************************************/
/*										*/
/* register a service with the inflection server				*/
/*										*/
/* use:   ./example_reg user pass service					*/
/*										*/
/* where user is a valid career account, pass is a random string that is	*/
/*	NOT the user's real password, service is service name			*/
/*										*/
/* The program sends a registration message to the inflection server, and then	*/
/* waits for an access app to connect and send a prompt.  When the prompt	*/
/* arrives, the program sends a reply that says					*/
/*										*/
/*		 "The reg app received the prompt: xxx"				*/
/*										*/
/* where xxx is a copy of the prompt that arrived.  After sending a reply, the	*/
/* reg app closes the TCP connection						*/
/*										*/
/********************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include "inflection.h"

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff
#endif	/* INADDR_NONE */

/* Define buffer size */

#define		BUFF_SIZ	2048	/* size of a buffer for a connection	*/
#define         FILENAME        "schedule.txt" /* location of alarm schedule */
#define         CRONFILE        "contab.alarm" /* location of alarm crontabs */

/* Function Prototypes */
int saveWeek(char * buffer);
int getWeek(char * buffer, int buffer_length);
char * formatCron(char * buffer, int start, int length);


/*
 * Take a buff in the msg format for 1 schduel and converts to the correct conjob format
 * str should be unallocated and alloacted in this method
 *
 * Returns 0 on Sucess
 * Returns -1 on Failure
 */
char * formatCron(char * buffer, int start, int length) {

  char * temp;
  temp = (char *) malloc(sizeof(char) * 100);

  //Check to see if enough memory is available
  //Important to do on the PI
  if (temp == NULL) {
    return NULL;
  }

  memset(temp, 0, 100);

  int i;
  int c = 0;
  int current = 0;
  char name[20], day[20], time[20], duration[20];  
  char * section = name;

  //Traverse the buffer, each section is split up by pipes
  //When a pipe is encountered copy switch to next section
  for (i = start; i < length + start; i++) {
    if (buffer[i] == '|' || (i - start + 1) == length) {

      if (buffer[i] == '|') {
	section[c] = 0;
      } else {
	section[c] = buffer[i];
	section[c+1] = 0;
      }

      c = 0;
      
      if (current == 0)
	section = day;
      else if (current == 1)
	section = time;
      else if (current == 2)
	section = duration;

      current++;
    } else {
      //if the section has more than 20 characters do not copy, this stop buffer overflow attacks
      if (c < 20) {
	section[c] = buffer[i];
	c++;
      }
    }
  }
  
  char min[3];
  char hour[3];

  memcpy(hour, time, 2);
  memcpy(min, time + 3, 2);

  min[2] = hour[2] = 0;

  int dow = 0;

  if (day[0] == 'M')
    dow = 1;
  else if (day[0] == 'T')
    dow = 2;
  else if (day[0] == 'W')
    dow = 3;
  else if (day[0] == 'R')
    dow = 4;
  else if (day[0] == 'F')
    dow = 5;

  sprintf(temp, "%s %s * * %d ./alarm %s", min, hour, dow, duration);
  printf("%s\n", temp);

  return temp;
}


/*
 * saveWeek - Take a buffer as parameter which contains the entire weekly schedule
 * Parses the buffer, and stores the weekly schedule in a file
 *
 * Returns 0 on Success
 * Returns -1 on Failure
 */
int saveWeek(char * buffer) {
  printf("%s\n", buffer);

  //Make sure the first two characters are W+
  if (strncmp(buffer, "W+", 2) != 0) {
    return -1;
  }

  FILE * out_sch, * out_cron;
  char * mode = "w";

  //Open schedule file for writing
  out_sch = fopen(FILENAME, mode);
  if (out_sch == NULL) {
    return -1;
  }

  //Open cron file for writing
  out_cron = fopen(CRONFILE, mode);
  if (out_cron == NULL) {
    return -1;
  }

  int i;
  int length = strlen(buffer);

  int start = 2;
  int end = 2;

  //Traverse the message everytime a + is encountered means there is a new entry
  //Save the entry to the file
  for (i = 2; i < length; i++) {
    if (buffer[i] == '+') {
      end = i;
      char * cron_str = formatCron(buffer, start, end - start);
      
      //Formats Cron_str for output to cronfile, also checks the formats so if it success can output to schedule file
      if ( cron_str != NULL) {
	fprintf(out_cron, "%s\n", cron_str);
	fprintf(out_sch, "%.*s\n", end - start, buffer + start);
      }
      if (cron_str != NULL)
	free(cron_str);

      start = i + 1;
    }
  }

  fclose(out_sch);
  return 0;
}


/*
 * getWeek -- Returns the current schedule saved on the PI and stores it in the buffer
 * The buffer_length must be large enough to store the entire schedule or the buffer is not modified
 *
 * Returns 0 on Success
 * Returns -1 on Failure
 */
int getWeek(char * buffer, int buffer_length) {
  FILE * in;
  char * mode = "r";

  printf("test");
  //Open File for reading
  in = fopen(FILENAME, mode);
  if (in == NULL) {
    return -1;
  }

  printf("test1\n");
  char * line;
  line = (char *)malloc(sizeof(char) * 100);
  size_t len;
  ssize_t read;
  
  int current = 2;

  //Prepend the W+ symbol which lets the client know this msg contains the day of the week
  buffer[0] = 'W';
  buffer[1] = '+';

  /*
   * Read schedule line by line, and copy into the buffer with the correct format for sending
   * If the schedule is too big for the buffer, close the file and return an error
   */
  while ((read = getline(&line, &len, in)) != -1) {
    if (current + read > buffer_length) {
      if (line != NULL) {
	free(line);
      }
      fclose(in);

      return -1;	  
    }
    memcpy(&buffer[current], line, (read - 1));
    current = current + (read - 1);    
    
    //Append the + symbol 
    buffer[current] = '+';
    current = current + 1;

  }

  buffer[current] = '$';
  //  if (line != NULL) {
  //free(line);
  //}

  fclose(in);
  return 0;
}


/********************************************************************************/
/*										*/
/* main -- main program for registration app to test the inflection server	*/
/*										*/
/********************************************************************************/


main (int argc, char *argv[]) {

	int		i;			/* loop index			*/
	int		n;			/* number of chars read		*/
	char		*user, *pass, *svc;	/* given by command line args	*/
	char		host[]="xinu00.cs.purdue.edu";	/* location of server	*/
	int		len;			/* string length temporaries	*/

	struct	cmd	*pcmd;			/* ptr to a registration command*/

	struct	hostent	*phe;			/* pointer to host info. entry	*/
	struct	protoent *ppe;			/* ptr. to protocol info. entry	*/
	struct	sockaddr_in socin;		/* an IPv4 endpoint address	*/
	int	addrlen;			/* len of a sockaddr_in struct	*/
	
	int	sock;				/* descriptor for socket	*/

	char	buffer[BUFF_SIZ];		/* input buffer for prompt	*/

	char	reply[] = "This is an echo from the reg side -> ";
						/* reply prefix			*/
	char	replybuf[BUFF_SIZ+sizeof(reply)];	/* reply buffer		*/

	/* check args */

	int q = 0;
	
	if (q == 1) {
	  memset(replybuf, 0, BUFF_SIZ+sizeof(reply));
	  if (getWeek(replybuf, BUFF_SIZ + sizeof(reply)) < 0)
	    printf("Failure");
	  else
	    printf("%s - ", replybuf);
	  exit(1);
	}

	if (q == 2) {
	  int rets;
	  rets = saveWeek("W+class1|M|14:30|50+class1|W|14:30|50+class1|F|14:30|50+class2|T|10:00|75+class2|R|10:00|75+");

	  if (rets == 0) {
	    printf("Win\n");
	  } else {
	    printf("Fail\n");
	  }
	  
	  exit(1);
	}

	if (argc != 4) {
		fprintf(stderr, "use is:   ./example_reg user passwd service\n");
		exit(1);
	}

	user = argv[1];
	pass = argv[2];
	svc  = argv[3];

	if (strlen(user) > UID_SIZ) {
		fprintf(stderr, "user name %s is too long\n", user);
		exit(1);
	}

	if (strlen(pass) > PASS_SIZ) {
		fprintf(stderr, "password %s is too long\n", pass);
		exit(1);
	}

	if (strlen(svc) > SVC_SIZ) {
		fprintf(stderr, "Service name %s is too long\n", svc);
		exit(1);
	}

	while (1) {
	/* Open socket used to connect to inflection server */

	memset(&socin, 0, sizeof(socin));
	socin.sin_family = AF_INET;

	/* Map host name to IP address or map dotted decimal */

	if ( phe = gethostbyname(host) ) {
		memcpy(&socin.sin_addr, phe->h_addr, phe->h_length);
	} else if ( (socin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE ) {
		fprintf(stderr, "can't get host entry for %s\n", host);
		exit(1);
	}

	socin.sin_port = htons( (unsigned short)TCPPORT );
	ppe = getprotobyname("tcp");

	/* Create the socket */

	sock = socket(PF_INET, SOCK_STREAM, ppe->p_proto);
	if (sock < 0) {
		fprintf(stderr, "cannot create socket\n");
		exit(1);
	}

	/* Connect the socket */

	if (connect(sock, (struct sockaddr *)&socin, sizeof(socin)) < 0) {
		fprintf(stderr, "can't connect to port %d\n", TCPPORT);
		exit(1);
	}

	/* Form a registration command and send */

	pcmd = (struct cmd *) buffer;
	pcmd->cmdtype = CMD_REGISTER;

	/* Add user ID */

	len = strlen(user);
	memset(pcmd->cid, ' ', UID_SIZ);
	memcpy(pcmd->cid, user, len);

	pcmd->cslash1 = '/';

	/* Add password */

	len = strlen(pass);
	memset(pcmd->cpass, ' ', PASS_SIZ);
	memcpy(pcmd->cpass, pass, len);

	pcmd->cslash2 = '/';

	/* Add service */

	len = strlen(svc);
	memset(pcmd->csvc, ' ', SVC_SIZ);
	memcpy(pcmd->csvc, svc, len);

	pcmd->dollar  = '$';

	/* Send registration message */

	send(sock, buffer, sizeof(struct cmd), 0);

	/* Wait for access app to respond by sending data */
	int bytes_read = 0;
	int read_all = 1;

	while (read_all) {
	  n = read(sock, &buffer[bytes_read], BUFF_SIZ);
	  if (n < 0) {
	    fprintf(stderr, "error reading from the socket\n");
	    exit(1);
	  } else if (n == 0) {
	    fprintf(stderr, "\nTCP connection was closed before a prompt arrived\n");
	    exit(0);
	  }
	  
	  printf("Recieved: %.*s\n", n, buffer + bytes_read);
	  int i;
	  for (i = 0; i < n; i++) {
	    if (buffer[bytes_read + i] == '\0') {
	      read_all = 0;
	    }	     
	  }
	  bytes_read += n;
	}

	/* prompt arrived from access app */
	n = bytes_read;

	buffer[n] = '\0';
	fprintf(stderr, "\nReceived a prompt: %s\n", buffer);
	memset(replybuf, 0, BUFF_SIZ+sizeof(reply));

	int num = 0;
	if (strcmp(buffer, "MY_WEEK") == 0) {

	  if (getWeek(replybuf, BUFF_SIZ + sizeof(reply)) < 0)
	    printf("Failure\n");
	  else
	    printf("Sucess\n");

	}
	else {
	  int ret;
	  ret = saveWeek(buffer);
	  replybuf[0] = ret;
	}
	len = strlen(replybuf);
	send(sock, replybuf, len, 0);
	replybuf[len+n] = '\0';

	fprintf(stderr, "Sent a reply: %s\n", replybuf);

	fprintf(stderr, "\nClosing the TCP connection.\n");
	close(sock);

	memset(buffer, 0, BUFF_SIZ);
	memset(replybuf, 0, BUFF_SIZ);
	}
}

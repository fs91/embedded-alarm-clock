
/* Common definintions for client and server sides for inflection server	*/

/* Define sizes */

#define		UID_SIZ		10	/* Bytes in a user ID			*/
#define		PASS_SIZ	8	/* Bytes in a password			*/
#define		SVC_SIZ		10	/* Bytes in a service name. Change next	*/
					/*   line if size changes		*/
#define		SVC_BLANK	"          " /* SVC_SIZ blank characters	*/

/* TCP port to use */

#define		TCPPORT		49909	/* TCP port the service will use	*/

/* structure of a command */

struct	cmd	{			/* format of a command			*/
	char	cmdtype;		/* command type (see CMD_* above)	*/
	char	cid[UID_SIZ];		/* user ID (blank-padded on right)	*/
	char	cslash1;		/* Must be a slash character 		*/
	char	cpass[PASS_SIZ];	/* password (blank padded on right)	*/
	char	cslash2;		/* Must be a slash character		*/
	char	csvc[SVC_SIZ];		/* service name				*/
	char	dollar;			/* Must be a dollar sign		*/
};

#define		CMD_SIZ		sizeof(struct cmd) /* bytes in a command	*/

/* Definition of command types */

#define		CMD_ACCESS	'A'	/* Command type for "Access"		*/
#define		CMD_CLEAR	'C'	/* Command type for "Clear"		*/
#define		CMD_REGISTER	'R'	/* Command type for "Register"		*/
#define		CMD_LIST	'L'	/* Command type for "List"		*/

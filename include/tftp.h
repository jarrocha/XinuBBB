/* tftp.h - definitions for trivial file transfer protocol */

/* TFTP Packet Op codes */
#define TFTP_RRQ   1   /* Read Request    */
#define TFTP_WRQ   2   /* Write Request   */
#define TFTP_DATA  3   /* Data Packet     */
#define TFTP_ACK   4   /* Acknowledgement */
#define TFTP_ERROR 5   /* Error           */

/* TFTP Error Codes */
#define TFTP_ERROR_NOT_DEFINED         0  /* Not defined, see error message (if any). */
#define TFTP_ERROR_FILE_NOT_FOUND      1  /* File not found.                          */
#define TFTP_ERROR_ACCESS_VIOLATION    2  /* Access violation.                        */
#define TFTP_ERROR_DISK_FULL           3  /* Disk full or allocation exceeded.        */
#define TFTP_ERROR_ILLEGAL_OP          4  /* Illegal TFTP operation.                  */
#define TFTP_ERROR_UNKNOWN_TRANSFER_ID 5  /* Unknown transfer ID.                     */
#define TFTP_ERROR_FILE_EXISTS         6  /* File already exists.                     */
#define TFTP_ERROR_NO_SUCH_USER        7  /* No such user.                            */

#define TFTP_PORT       69      /* UDP Port for TFTP            */
#define	TFTP_MAXNAM	    64      /* Max length of a file name    */
#define	TFTP_MAXDATA    512     /* Max size of a data packet    */
#define	TFTP_MAXRETRIES	3       /* Number of retranmissions     */
#define	TFTP_WAIT       5000    /* Time to wait for reply (ms)  */

/* Xinu Specific Flags */
#define TFTP_NON_VERBOSE 0  /* Do not use verbose output */
#define TFTP_VERBOSE     1  /* Use verbose output        */

/* Format of a TFTP message (items following opcode depend on msg type)	*/

#pragma pack(1)
struct	tftp_msg {
	uint16		tf_opcode;	/* One of the opcodes above	*/
	union {

	 /* Items in a RRQ or WRQ message */

	 struct	{
	  char	tf_filemode[TFTP_MAXNAM+10]; /* file name and mode	*/
	 };

	 /* Items in a Data packet */

	 struct {
	  uint16	tf_dblk;	/* Block number of this data	*/
	  char		tf_data[TFTP_MAXDATA]; /* Actual data		*/
	 };

	 /* Items in an ACK packet */

	 struct {
	  uint16	tf_ablk;	/* Block number being acked	*/
	 };

	/* Items in an Error packet */

	 struct {
	  uint16	tf_ercode;	/* Integer error code		*/
	  char		tf_ermsg[TFTP_MAXDATA]; /* Error message	*/
	 };
	};
};
#pragma pack()

status tftpget(uint32 serverip, const char* filename, char* rcv_buf, uint32 rcv_buf_size, byte verbose);
status tftpget_mb(uint32 serverip, const char* filename, char** rcv_bufs, uint32* rcv_buf_sizes, uint32 rcv_buf_count, byte verbose);
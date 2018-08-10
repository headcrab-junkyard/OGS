

// defs common to client and server

#if (defined(_M_IX86) || defined(__i386__)) && !defined(id386)
#define id386	1
#else
#define id386	0
#endif


#define	MAX_SCOREBOARD		16		// max numbers of players

#define	SOUND_CHANNELS		8

#define	MAX_MSGLEN		1450		// max length of a reliable message
#define	MAX_DATAGRAM	1450		// max length of unreliable message

//
// stats are integers communicated to the client by the server
//
//define	STAT_FRAGS			1
//define	STAT_WEAPONFRAME	5

//
// print flags
//
#define	PRINT_LOW			0		// pickup messages
#define	PRINT_MEDIUM		1		// death messages
#define	PRINT_HIGH			2		// critical messages
#define	PRINT_CHAT			3		// chat messages


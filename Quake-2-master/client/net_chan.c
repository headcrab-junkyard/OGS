/*

packet header
-------------
31	sequence
1	does this message contain a reliable payload
31	acknowledge sequence
1	acknowledge receipt of even/odd message
16	qport

The remote connection never knows if it missed a reliable message, the
local side detects that it has been dropped by seeing a sequence acknowledge
higher thatn the last reliable sequence, but without the correct evon/odd
bit for the reliable set.

If the sender notices that a reliable message has been dropped, it will be
retransmitted.  It will not be retransmitted again until a message after
the retransmit has been acknowledged and the reliable still failed to get there.

if the sequence number is -1, the packet should be handled without a netcon

The reliable message can be added to at any time by doing
MSG_Write* (&netchan->message, <data>).

If the message buffer is overflowed, either by a single message, or by
multiple frames worth piling up while the last reliable transmit goes
unacknowledged, the netchan signals a fatal error.

Reliable messages are always placed first in a packet, then the unreliable
message is included if there is sufficient room.

To the receiver, there is no distinction between the reliable and unreliable
parts of the message, they are just processed out as a single larger message.

Illogical packet sequence numbers cause the packet to be dropped, but do
not kill the connection.  This, combined with the tight window of valid
reliable acknowledgement numbers provides protection against malicious
address spoofing.


The qport field is a workaround for bad address translating routers that
sometimes remap the client's source port on a packet during gameplay.

If the base part of the net address matches and the qport matches, then the
channel matches even if the IP port differs.  The IP port should be updated
to the new value before sending out any replies.


If there is no information that needs to be transfered on a given frame,
such as during the connection stage while waiting for the client to load,
then a packet only needs to be delivered if there is something in the
unacknowledged reliable
*/

netadr_t	net_from;
sizebuf_t	net_message;
byte		net_message_buffer[MAX_MSGLEN];

void Netchan_Init ()
{
	port = Sys_Milliseconds() & 0xffff;

	qport = Cvar_Get ("qport", va("%i", port), CVAR_NOSET);
}

qboolean Netchan_NeedReliable (netchan_t *chan)
{
	qboolean	send_reliable;

// if the remote side dropped the last reliable message, resend it
	send_reliable = false;

	if (chan->incoming_acknowledged > chan->last_reliable_sequence
	&& chan->incoming_reliable_acknowledged != chan->reliable_sequence)
		send_reliable = true;

// if the reliable transmit buffer is empty, copy the current message out
	if (!chan->reliable_length && chan->message.cursize)
	{
		send_reliable = true;
	}

	return send_reliable;
}

void Netchan_Transmit (netchan_t *chan, int length, byte *data)
{
	byte		send_buf[MAX_MSGLEN];

	send_reliable = Netchan_NeedReliable (chan);

	if (!chan->reliable_length && chan->message.cursize)
	{
		memcpy (chan->reliable_buf, chan->message_buf, chan->message.cursize);
		chan->reliable_length = chan->message.cursize;
		chan->message.cursize = 0;
		chan->reliable_sequence ^= 1;
	}


// write the packet header
	SZ_Init (&send, send_buf, sizeof(send_buf));

	w1 = ( chan->outgoing_sequence & ~(1<<31) ) | (send_reliable<<31);
	w2 = ( chan->incoming_sequence & ~(1<<31) ) | (chan->incoming_reliable_sequence<<31);

	chan->outgoing_sequence++;
	chan->last_sent = curtime;

	MSG_WriteLong (&send, w1);
	MSG_WriteLong (&send, w2);

	// send the qport if we are a client
	if (chan->sock == NS_CLIENT)
		MSG_WriteShort (&send, qport->value);

// copy the reliable message to the packet first
	if (send_reliable)
	{
		SZ_Write (&send, chan->reliable_buf, chan->reliable_length);
		chan->last_reliable_sequence = chan->outgoing_sequence;
	}
	
// add the unreliable part if space is available
	if (send.maxsize - send.cursize >= length)
		SZ_Write (&send, data, length);
	else
		Com_Printf ("Netchan_Transmit: dumped unreliable\n");

// send the datagram
	NET_SendPacket (chan->sock, send.cursize, send.data, chan->remote_address);

	if (showpackets->value)
	{
		if (send_reliable)
			Com_Printf ("send %4i : s=%i reliable=%i ack=%i rack=%i\n"
				, send.cursize
				, chan->outgoing_sequence - 1
				, chan->reliable_sequence
				, chan->incoming_sequence
				, chan->incoming_reliable_sequence);
		else
			Com_Printf ("send %4i : s=%i ack=%i rack=%i\n"
				, send.cursize
				, chan->outgoing_sequence - 1
				, chan->incoming_sequence
				, chan->incoming_reliable_sequence);
	}
}

qboolean Netchan_Process (netchan_t *chan, sizebuf_t *msg)
{
	int			qport;

	if (showpackets->value)
	{
		if (reliable_message)
			Com_Printf ("recv %4i : s=%i reliable=%i ack=%i rack=%i\n"
				, msg->cursize
				, sequence
				, chan->incoming_reliable_sequence ^ 1
				, sequence_ack
				, reliable_ack);
		else
			Com_Printf ("recv %4i : s=%i ack=%i rack=%i\n"
				, msg->cursize
				, sequence
				, sequence_ack
				, reliable_ack);
	}

//
// discard stale or duplicated packets
//
	if (sequence <= chan->incoming_sequence)
	{
		if (showdrop->value)
			Com_Printf ("%s:Out of order packet %i at %i\n"
				, NET_AdrToString (chan->remote_address)
				,  sequence
				, chan->incoming_sequence);
		return false;
	}

//
// dropped packets don't keep the message from being used
//
	chan->dropped = sequence - (chan->incoming_sequence+1);
	if (chan->dropped > 0)
	{
		if (showdrop->value)
			Com_Printf ("%s:Dropped %i packets at %i\n"
			, NET_AdrToString (chan->remote_address)
			, chan->dropped
			, sequence);
	}

//
// if the current outgoing reliable message has been acknowledged
// clear the buffer to make way for the next
//
	if (reliable_ack == chan->reliable_sequence)
		chan->reliable_length = 0;	// it has been received
	
//
// if this message contains a reliable message, bump incoming_reliable_sequence 
//
	chan->incoming_sequence = sequence;
	chan->incoming_acknowledged = sequence_ack;
	chan->incoming_reliable_acknowledged = reliable_ack;
	if (reliable_message)
	{
		chan->incoming_reliable_sequence ^= 1;
	}

//
// the message can now be read from the current message pointer
//
	chan->last_received = curtime;

	return true;
}


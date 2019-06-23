
	Note: GS prediction:
	
	in loop on client:
	* get latest ack client state
	* get first cmd that goes after that state (that cmd is sent to server and
	applied locally)
	* apply this cmd to latest local state (run playermove as it's done on server)
	* if this is the last cmd that present in queue then return from loop
	* latest local state = predicted state with latest unack cmd
	* latest unack cmd++ (= next unack cmd in queue)
	* it's not a ring buffer in use
	* prediction ability is limited in time (it's ok since we only need an
	instant reaction to our action on client side)
	* prediction is disabled in sp
	* prediction should be disabled on moving platforms?
	* all moving things from server should be interpolated (but their anim should be predicted/extrapolated?)
	* this difference in time between send a cmd and reveice ack from server - client shouldn't set a new pos from server, it should ignore if it's between some interval of appliable difference between sv/cl pos (or smooth this diff in time)

	server:
	* get cmd from client
	* get latest client state and apply this cmd on it (playermove) which
	should get the same results as on client
	(cmd with forwardmove=1.0 and time=100ms should get 32 units of move at 320 forwardspd cvar)
	* client send a cmd and apply it locally instantly
	* server applies that cmd after ping*0.5=latency
	* so the command ack is coming back to client after 2*latency=ping
	* and now this is the latest ack cmd (in couple with it's state) that should be used for prediction
	* prediction error are smoothed in time (something like)
	
	double sound prevention:
	* playermove code has a mech that is used to prevent step sound to play twice
	("runcmds" or smthng like that)
	
	weapon prediction:
	* protocol has a svc_weaponanim entry that should set client a new weapon animation;
	this should be ignored (I think) when cl_lw is enabled; instead there is a local weapon simulation
	should be applied (attack cmd -> server -> apply anim and muzzle locally if we can) -> ack from server
	* just apply weapon anim if local weapon code allows us to do that
	
	
	Prediction is enabled by default in GS now and there is no way to disable it
	Pushlatency cvar has issues before and was removed from public access (and mb from the code too)
	(Almost) all data for this mech is present inside hlsdk headers
	This mech is similar to demo playback
	GS periodically syncs client and server clocks

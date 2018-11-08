/// @file

#ifdef GLX
static qboolean mouse_avail;
static qboolean mouse_active;
static int mx, my;
static int old_mouse_x, old_mouse_y;

static cvar_t in_mouse = { "in_mouse", "1", false };
static cvar_t in_dgamouse = { "in_dgamouse", "1", false };
static cvar_t m_filter = { "m_filter", "0" };

qboolean dgamouse = false;
#endif

void Force_CenterView_f()
{
	cl.viewangles[PITCH] = 0;
}

#ifdef MESA
void mousehandler(int buttonstate, int dx, int dy)
{
	mouse_buttonstate = buttonstate;
	mx += dx;
	my += dy;
}
#endif

void IN_Init()
{
#ifdef X11
	Cvar_RegisterVariable (&_windowed_mouse);
	Cvar_RegisterVariable (&m_filter);
	Cvar_RegisterVariable (&mouse_button_commands[0]);
	Cvar_RegisterVariable (&mouse_button_commands[1]);
	Cvar_RegisterVariable (&mouse_button_commands[2]);
	Cmd_AddCommand ("force_centerview", Force_CenterView_f);
#elif MESA
	int mtype;
	char *mousedev;
	int mouserate;

	if (UseMouse)
	{
		Cvar_RegisterVariable (&mouse_button_commands[0]);
		Cvar_RegisterVariable (&mouse_button_commands[1]);
		Cvar_RegisterVariable (&mouse_button_commands[2]);
		Cmd_AddCommand ("force_centerview", Force_CenterView_f);

		mouse_buttons = 3;

		mtype = vga_getmousetype();

		mousedev = "/dev/mouse";
		if (getenv("MOUSEDEV")) mousedev = getenv("MOUSEDEV");
		if (COM_CheckParm("-mdev"))
			mousedev = com_argv[COM_CheckParm("-mdev")+1];

		mouserate = 1200;
		if (getenv("MOUSERATE")) mouserate = atoi(getenv("MOUSERATE"));
		if (COM_CheckParm("-mrate"))
			mouserate = atoi(com_argv[COM_CheckParm("-mrate")+1]);

		if (mouse_init(mousedev, mtype, mouserate))
		{
			Con_Printf("No mouse found\n");
			UseMouse = 0;
		}
		else
			mouse_seteventhandler(mousehandler);
	}
#endif
}

void IN_Shutdown()
{
#ifdef MESA
	if (UseMouse)
		mouse_close();
#endif
}

/*
===========
IN_Commands
===========
*/
void IN_Commands()
{
#ifdef GLX
	if(!dpy || !win)
		return;

	if(vidmode_active || key_dest == key_game)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse();
#elif X11
	int i;

	for (i=0 ; i<mouse_buttons ; i++) {
		if ( (mouse_buttonstate & (1<<i)) && !(mouse_oldbuttonstate & (1<<i)) )
			Key_Event (K_MOUSE1 + i, true);

		if ( !(mouse_buttonstate & (1<<i)) && (mouse_oldbuttonstate & (1<<i)) )
			Key_Event (K_MOUSE1 + i, false);
	}

	mouse_oldbuttonstate = mouse_buttonstate;
#elif MESA
	if (UseMouse)
	{
		// poll mouse values
		while (mouse_update())
			;

		// perform button actions
		if ((mouse_buttonstate & MOUSE_LEFTBUTTON) &&
			!(mouse_oldbuttonstate & MOUSE_LEFTBUTTON))
			Key_Event (K_MOUSE1, true);
		else if (!(mouse_buttonstate & MOUSE_LEFTBUTTON) &&
			(mouse_oldbuttonstate & MOUSE_LEFTBUTTON))
			Key_Event (K_MOUSE1, false);

		if ((mouse_buttonstate & MOUSE_RIGHTBUTTON) &&
			!(mouse_oldbuttonstate & MOUSE_RIGHTBUTTON))
			Key_Event (K_MOUSE2, true);
		else if (!(mouse_buttonstate & MOUSE_RIGHTBUTTON) &&
			(mouse_oldbuttonstate & MOUSE_RIGHTBUTTON))
			Key_Event (K_MOUSE2, false);

		if ((mouse_buttonstate & MOUSE_MIDDLEBUTTON) &&
			!(mouse_oldbuttonstate & MOUSE_MIDDLEBUTTON))
			Key_Event (K_MOUSE3, true);
		else if (!(mouse_buttonstate & MOUSE_MIDDLEBUTTON) &&
			(mouse_oldbuttonstate & MOUSE_MIDDLEBUTTON))
			Key_Event (K_MOUSE3, false);

		mouse_oldbuttonstate = mouse_buttonstate;
	}
#endif
}

/*
===========
IN_Move
===========
*/
void IN_MouseMove(usercmd_t *cmd)
{
#ifdef GLX
	if(!mouse_avail)
		return;

	if(m_filter.value)
	{
		mx = (mx + old_mouse_x) * 0.5;
		my = (my + old_mouse_y) * 0.5;
	}
	old_mouse_x = mx;
	old_mouse_y = my;

	mx *= sensitivity.value;
	my *= sensitivity.value;

	// add mouse X/Y movement to cmd
	if((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
		cmd->sidemove += m_side.value * mx;
	else
		cl.viewangles[YAW] -= m_yaw.value * mx;

	if(in_mlook.state & 1)
		V_StopPitchDrift();

	if((in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * my;
		if(cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if(cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * my;
		else
			cmd->forwardmove -= m_forward.value * my;
	}
	mx = my = 0;
#elif X11
	while (Mouse_Update())
		;

	if (m_filter.value) {
		mouse_x = (mouse_x + old_mouse_x) * 0.5;
		mouse_y = (mouse_y + old_mouse_y) * 0.5;
	}

	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;
   
	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;
   
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;
	if (in_mlook.state & 1)
		V_StopPitchDrift ();
   
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1)) {
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	} else {
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}
	mouse_x = mouse_y = 0.0;
#elif MESA
	if (!UseMouse)
		return;

	// poll mouse values
	while (mouse_update())
		;

	if (m_filter.value)
	{
		mouse_x = (mx + old_mouse_x) * 0.5;
		mouse_y = (my + old_mouse_y) * 0.5;
	}
	else
	{
		mouse_x = mx;
		mouse_y = my;
	}
	old_mouse_x = mx;
	old_mouse_y = my;
	mx = my = 0; // clear for next update

	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

	// add mouse X/Y movement to cmd
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;
	
	if (in_mlook.state & 1)
		V_StopPitchDrift ();
		
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}
#endif
}

void IN_Move(usercmd_t *cmd)
{
	IN_MouseMove(cmd);
}

#ifdef GLX
static void IN_DeactivateMouse()
{
	if(!mouse_avail || !dpy || !win)
		return;

	if(mouse_active)
	{
		uninstall_grabs();
		mouse_active = false;
	}
}

static void IN_ActivateMouse()
{
	if(!mouse_avail || !dpy || !win)
		return;

	if(!mouse_active)
	{
		mx = my = 0; // don't spazz
		install_grabs();
		mouse_active = true;
	}
}
#endif // GLX
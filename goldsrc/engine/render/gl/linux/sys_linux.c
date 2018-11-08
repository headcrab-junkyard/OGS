/// @file

void Sys_SendKeyEvents()
{
#ifdef GLX
	HandleEvents();
#elif X11
	if (dpy)
	{
		while (Keyboard_Update())
			;

		while (keyq_head != keyq_tail)
		{
			Key_Event(keyq[keyq_tail].key, keyq[keyq_tail].down);
			keyq_tail = (keyq_tail + 1) & 63;
		}
	}
#elif MESA
	if (UseKeyboard)
		while (keyboard_update());
#endif
}
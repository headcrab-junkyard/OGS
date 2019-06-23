# Introduction to the Console

Throughout this document, examples of commands are given, all of which
are typed in at the console. To bring up the console, press the tilde ('~')
key or press ESC to bring up the menu, select Options, and select Console...
from the options menu. To exit the console, press ESC.

The console provides a way to change console variables and also accepts
commands that change game settings such as movement keys, video mode, as
well as providing an interface for key binding and command aliasing (more
on that later).

The console also has a command history with which you can browse through
previous commands. Use the up and down arrows to navigate through the
command history and press <enter> to re-issue a command.

Partially typing a command and then pressing the TAB key will complete the
currently typed text with the first matching console variable or command.
(Yes, this is a good way to look for console commands.)

To review previous actions by page, use the PGUP and PGDN keys.

# Key Binding and Aliases

Pressing the tilde key ("~") will bring down the console (pressing the
tilde key or ESC while in the console will close the console). From the
console you can adjust your player controls, this is done by "binding"
keys to commands.  The format for binding keys is as follows:

bind <key> <command>

Where <key> is a valid key control and <command> is a valid quake command.

Example:
To bind the j key to the 'jump' command, you would type:
bind j +jump			
and press enter.

Non-printable keys such as 'page up' and buttons from the mouse/joystick are
bound in the same manner as printable characters. A list of bindable keys can
be found at the end of this file.

Example:
To bind the page up key to the 'jump' command, you would type:
bind pageup +jump
and press enter.

To bind the right mouse button to the attack command, you would type:
bind mouse2 +attack
and press enter.

The alias command is used to create a reference to a command or list of
commands.  When aliasing multiple commands, or commands that contain
multiple words (such as "fraglimit 50"), you must enclose all the commands
in quotation marks and separate each command with a semi-colon.

Example of an alias that changes some Deathmatch server parameters:

alias net_game "hostname my_server ; fraglimit 15 ; timelimit 15"
bind INS net_game

Once the server is spawned (you must be the one running the -listen server),
you just push the Insert key to set the hostname, frag limit and time limit
of the server. So now the first person to 15 frags, or with the one with the
most frags in 15 minutes, wins.

Another example would be to change to the Rocket Launcher, fire one rocket,
and change back to the Double Barrel Shotgun, when you press the "," key:

alias rl_dbsg "impulse 7 ; +attack ; wait ; -attack ; impulse 3"
bind , rl_dbsg

Aliasing is very powerful, allowing you great flexibility, so you should
experiment by aliasing different commands in various ways.

A list of common commands can be found in the next section.
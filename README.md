<p align="center">
	<a href="https://github.com/BlackPhrase/OGS"><img width="256" heigth="256" src="./docs/OGSLogo512x512.png?raw=true" alt="OGS Logo"/></a>
</p>

<p align="center">
	<a href="https://ci.appveyor.com/project/BlackPhrase/OGS">
		<img src="https://ci.appveyor.com/api/projects/status/github/BlackPhrase/OGS?svg=true"/>
	</a>
	<a href="https://travis-ci.org/BlackPhrase/OGS">
		<img src="https://travis-ci.org/BlackPhrase/OGS.svg"/>
	</a>
	<a href="https://codeclimate.com/github/BlackPhrase/OGS/issues">
		<img src="https://img.shields.io/codeclimate/issues/github/BlackPhrase/OGS.svg"/>
	</a>
</p>

# OGS Engine

>Gold Source is a version of Quake and Quake is open source so maybe that is an option for you.
>
>Kind regards,
>
>Mike

GoldSource Engine Clone Prototype

Clean open source reimplementation of the GoldSource game engine  
Based on original Quake (Id Tech 2) engine sources

For more information about the project you can visit its official [Wiki](https://gitlab.com/BlackPhrase/OGS/wikis/home)

## Mini-Q&A

**Q: So no using of reverse-engineered code at all?**  
**A:** Nope, only backtrace logs and a lot of researching of original GS engine history

**Q: How much the GoldSource engine is different from original Quake engine?**  
**A:** Let's say it's pretty much a "dieselpunk" version of Quake engine

**Q: What are the "backtrace logs"?**  
**A:** Call stack of the latest called program functions/class methods before crash of the program. 
This information allows you to see which function caused the crash and its location in source code. 
(Backtrace logs don't show you the actual source code)

**Q: Will the engine be able to run my precious (%MODNAME%)?**  
**A:** Technically, it's possible to interconnect the reimplemented engine with original mods written 
using HLSDK, its license doesn't explicitly forbids to do that, but that's at your own risk. And only for non-commercial use. 
The game dlls compiled from the HLSDK code will still be licensed under terms of Half-Life SDK License, I can only permit to link 
the GPL3 OGS code with them as a special exception

**Q: Can I use the HLSDK to create a mod for this engine?**  
**A:** Nope, HLSDK EULA expects that any modification created using it will be used only by the original 
GoldSrc engine. You need to reimplement the game code yourself or use the reimplemented OGS SDK template game code based on Quake's QC progs code 
(if it's present at the time you reading this, I'm not planning to reimplement the game code for this engine for now, can't say for sure, but 
I doubt that anyone else decides to rewrite the game code for this engine, so it probably end up being used as a tech demo of the GoldSrc 
showcasing its features using some recreated HL Alpha maps)

## License

[![license](https://img.shields.io/github/license/BlackPhrase/OGS.svg)](https://github.com/BlackPhrase/OGS/blob/master/LICENSE)

GNU GPL v3+
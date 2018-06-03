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

For more information about the project you can visit its official [Wiki](https://github.com/BlackPhrase/OGS/wiki)

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
**A:** Nope, HLSDK EULA expects that any modification created using it will be used only by the original 
GoldSrc engine. Technically, it still might be possible to interconnect the reimplemented engine with original mods written 
using HLSDK, but that's at your own risk. I'm not planning to reimplement the game code for this engine. I'm also thinking about 
preventing the engine from supporting game/client dlls loading and just contain stubs for them inside the engine, 
can't say for sure now, but I doubt that anyone else decides to rewrite the game code for this engine, so it probably end up being used 
as a tech demo of the GoldSrc showcasing its features using some recreated HL Alpha maps

## License

[![license](https://img.shields.io/github/license/BlackPhrase/OGS.svg)](https://github.com/BlackPhrase/OGS/blob/master/LICENSE)

GNU GPL v3+
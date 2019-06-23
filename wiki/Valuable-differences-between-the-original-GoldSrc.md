Here is an updateable list of features that implemented/works different from the original GoldSrc:

* Proper source code files structurization!;
* No VGUI1 code inside the engine module;
* Both render and sound module code was split from the engine module into their own modules (which makes them freely modifiable);
* Removed the C wrapper for filesystem interface calls (original GS still uses it; OGS has it in rewritten to CFileSystem class form but probably it would removed too);
* Whole engine module codebase is rewritten to OOP (GS still using the procedural style);
* Whole code base is using the modern C++ standards;
* No D3D support planned (you can implement it yourself if you want);
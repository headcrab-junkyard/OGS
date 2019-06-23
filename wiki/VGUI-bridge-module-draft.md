In case we can't use VGUI2 to prevent linkage of VGUI lib with engine module we need to write a set of virtual interfaces that wraps the original vgui classes

I mean something like this:

> struct I(VGUI)Panel  
> {  
> // bla-bla-bla  
> };

> class C(VGUI)Panel : I(VGUI)Panel  
> {  
> public:  
> // bla-bla-bla  
> private:  
> vgui::Panel mPanel;  
> };

I can't say for sure, but maybe even VGUI2 works in that way
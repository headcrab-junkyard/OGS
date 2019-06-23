https://en.wikipedia.org/wiki/ReactOS#Internal_audit
>Some countries, including the United States, require that a reimplementation based on disassembled code must be written by someone other than the person having disassembled and examined the original code, whereas other countries allow both tasks to be performed by the same individual.

http://store.steampowered.com/subscriber_agreement/#2
>Except as otherwise permitted under this Agreement (including any Subscription Terms or Rules of Use), or under applicable law notwithstanding these restrictions, you may not, in whole or in part, copy, photocopy, reproduce, publish, distribute, translate, reverse engineer, derive source code from, modify, disassemble, decompile, create derivative works based on, or remove any proprietary notices or labels from the Content and Services or any software accessed via Steam without the prior consent, in writing, of Valve.

Current codebase consists of:

* NetQuake as a base; (GPL)
* QuakeWorld above it; (GPL)
* Q2 for fixes, improvements, render and such; (GPL)
* tier1 from Source SDK 2013; (should be rewritten from scratch)
* GameUI; (came from Alien Swarm SDK so it should be ok)
* vstdlib rewritten from scratch; (still wip; using the compiled library export symbols info)
* Probably some Xash3D sources later for some client-side (GPL; mostly replicated; contains some reversed parts; Valve still didn't close it)
* tier0; (contains some reversed bits but mostly blank atm; should be (re)written from scratch)
* filesystem; (reimplemented from scratch)
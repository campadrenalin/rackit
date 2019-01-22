# RackIt

This is a virtual synth rack toolkit designed to:

1. Run really fast for actual synthesis
2. Not be noticeably slow for configuration changes
3. Be suitable for music production and experimentation
4. Be suitable for _realtime aleotoric music_ in video games, primarily in Love2D engine
5. Act as a learning opportunity for the C programming language!

Plenty of audio software already exists, but I found I was regularly frustrated that I couldn't just play around and experiment on a raw level. Simple math stuff was often difficult to do. The closest I got was SynthMaker, which does have plenty of low-level widgets. And I do take some conceptual inspiration from using this! But it's still restrictive to which toys are already in the toybox, and the polyphony support always felt a little clumsy-magical, like something I didn't quite understand, and might rather do explicitly.

Now, mostly for fun, I'm writing my own virtual modular rack, that I can extend arbitrarily. I plan to write a lot of video game music that has hard-scripted elements and pseudorandom composition. For example, one fun idea is to have a "complexity director" that increases and decreases the "busyness" of the song - each track would expose a series of boolean toggles (which would have arbitrary meaning within the track), and the complexity director would look at the entire concatenated fleet of toggles, turning them on and off as measures go by. I'm actually thinking of making this part of the node data in the Pattern FSM. *The intent is to make recognizable, iconic music, but which always stays fresh.* Despite the video game context, I'm actually much less interested in incorporating player events as an input source, I find it's a little slow-to-react and boring in most games. Only broad strokes "which area am I in" questions seem to really provide useful data for the kind of stuff I do. These are all, of course, highly picky opinions that need not apply to anyone else using RackIt.

If you *do* find this interesting or useful, I'm pleasantly surprised. This is a personal project, so I'll state up front that I'm not as interested to fix problems or implement features, unless the request overlaps with something I'd want to do anyways. Keep expectations realistic. I like internet strangers, but my policy is that I don't owe them anything, and I think that can only be a fair policy if it's announced early and emphasized when necessary, given current FOSS culture.

## Nitty Gritty notes

 * Written in C (I tried to learn C++ and it made me hate life, to the point that I'm surprised how much I like C afterwards)
 * Lua bindings
 * SDL support (good for standalone), I'll be trying to make these optional
 * Love2D support coming eventually, which will need to exclude SDL and provide some good integration recipes
 * No license chosen yet

## Fun pointless trivia

 * This repo was originally a playground for programming language experiments using software synth as a concrete example domain. Eventually I just got really sucked into the software synth stuff. I do still push against the limits of C macros, so I may reintroduce some dynamic code generation, but I've been trying to take well-supported C macros as far as they'll go. If you look at early repo history you can see my buried garbage!
 * Most American car horns honk in the key of F.
 * I found the previous trivia by literally Googling "Fun pointless trivia" and picking a promising-looking result. I got astoundingly lucky that [the first item of the page](http://www.djtech.net/humor/useless_facts.htm) happened to be music/frequency related. That's not intentional, it's a _sign._
 * PBS is literally and openly a government conspiracy to make your kids less stupid. They pioneered the hybrid gov't+crowdfunding finance model so that they wouldn't have to take money from George Soros, for the practical and wise reason that Soros wasn't that rich yet, only starting his first hedge fund in the same year (1969). If you didn't read this very carefully and are now angry with me, for either political party affiliation, then you are the kind of person I'm poking fun at.

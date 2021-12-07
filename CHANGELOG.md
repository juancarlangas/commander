Commander 1.0.1 Alpha
=====================
- Correction of the missing left fader when the adjustin comes from having the same left and right
value.

- The Orchestration editor now return to the first variation when it comes from a new patch,
instead of returning to the same

Korg 4.7.3
==========

- A new Korg's paradigma prototype: The program now targets to a far-laptop scenario, where it is so difficult to read normal-sized console text, introducing an extensive use of the LCD characters. Now it features a SEARCH window, a PRE-CHOOSING (selected) song, and an ACTIVE (sended) song, all of them in a large-sized old-lcd style.

- Changed some keyboard shorcuts, improving security in the typewritting to avoid changing some system parameters (like VARIATION).

Korg 4.7.2
==========

- Complete redefining of the paradigma in the capture mechanism code: now all the engine was defined, structured, cleaned, and rebuilded in the main() function, disappearing the capture() function (even the 'capturing.c' file) and now uses a 'get_command()' function to improve legibility in the processors switch mechanism located on the main function.
- Delete of the full-displaying info of the track on LCDwindow, remaining just the song name.
- Bringing back of a newly redesigned COMPUTER window (previous development of this version located this two labels in the LCD window, just above the song name).

Korg 4.7.1
==========

- Introducing the VARIATION concept: Now each song contains TWO optional patches, indeeded to different orchestra configurations (e.g BRASS / NO BRASS, GUITAR / NO GUITAR, etc.)
- New Windows:
    *digitsWindow: Showing the bnk/num of the SELECTED patch (in real time)
    *computerWindow: Showing the variation1 / variation 2 state and the combinator / sequencer mode state.
- Returning of the digit-style song printing in the LCD display (*lcdWindow), displaying not the selected patch but the ACTIVE patch and containting more information such Genre, Section, etc.
    

Korg 4.7
========

- Complete restructuring of the software writing, achieveing a clearer structure and approaching to the C++ and Qt implementations of the korg/commander software.

Korg 4.6
========

- New data files manager based on commander 0.1: now the measures to counting rows are file-based instead of header-declarative.

Korg 4.5
========

- Redesigned interface based on commander 0.1
- Now the text-databased do not contain a space " " after the commas. This approachs to an eventually compatibilly with XML spreadsheets formats (like Microsoft Excel or LibreOffice)

Korg 4.4
========

- New internal Ingeeniery in storaging data; now there are two databases: one for combinations and the other for sequences, rather than one swaped.

Korg-4.3
========

- Multiple source playlist construction: now you can add sequences and/or combinations to a single playlist. Change mode is now not playlist-destructive.

Korg-4.2
========
- Implementations of both modes MIDI system: now Korg sends Bank/Program Changes, Song Select commands, and/or pmidi/mplayer system() commands as requiered.
- Dissapear of the comb / seq buttons and reimplement inside the lcd window.


Korg-4.1
========

- Second and succesful (thanks to God) introduction of the SEQUENCER MODE!!!, capable to play MIDI (pmidi) and AUDIO files (aplay) through C-system() console-related bridge-function.
- Rearangement of the windows. 
- Playlist: Ctrl-X, Ctrl-C, Ctrl-V and Ctrl-A escape secuences-
- Playlist: Multiple selection implement.

Korg-4.0-preprimaria
====================

- No mames!!!

Korg-3.3
========

- Introduction of the PLAYLIST!!!
- Redefinied windows look.

Korg-3.2.4
==========

- Rearrangment of the windows: Display Window & Search Windows interchanges its locations
- Color redesign of Display Window, semejating to new Nord Piano 3 Display.

Korg-3.2.3
==========

- Introduction of the displayWindow Window, containing all the reference of the patch.

Korg-3.2.2
--------

- Essentialy the same of 3.2.1
- Obsesive bug fix in string.c on korg-3.2.1 This bug occurs attemping to fix the extra character in "string" array.
- Introduction of the optional debugWindow Window for displaying debug-variables.
- Correct implementing of the no_accent() function on string.

Korg-3.2.1
--------

- New windows: search window.
- Eliminate of the "state" window.

Korg-3.2
--------

- First implementation of WINDOWS ncurses feature, with 2 windows (generalWindow for database and stateWindow for all the rest).
- Database actualization with MonkeyBongo songs, and some single-instrument patches.
- Return to transparent background with ncurses use_default_colors() function.

Korg-3.1
---------

- MIDIIII!!!!!!! :-P The first functional MIDI implementation, with the alsalib raw_midi API.


Korg-3.0
---------

- The first ncurses implementation.



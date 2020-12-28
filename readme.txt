                           EASY SPLITTER ver 2.0

                            By L. D. Blake 2005


Thank you for downloading Easy Splitter version 2.0  This is a full rewrite
of the original code.  Several new functions have been added and it's
behaviour has been changed as well.  If you are updating from version 1.X
you will need to study the demo program and read this entire file.  You
will have to modify your WM_SIZE handlers, to use this version.


EasySplitter is a custom control, in library format, that can be used with
Pelles C to implement a very simple splitter window.  With EasySplitter you
create a splitter bar as a windows control that you can size, move and
stylize like any other windows control.  All mouse capture and window drag
operations are handled internally by the control itself.

The user moves a splitter window by holding either the left or right mouse
button and dragging it to it's new location or they can use the keyboard
arrow keys.  At the end of each move EasySplitter emits WM_SIZE messages so
you can manage splitters in your normal window resizing code, just like the
user was resizing the window from it's border.  You do this by arranging
your other control windows around the splitters.



SETTING UP EASY SPLITTER

EasySplitter is supplied as a static library, usable by Pelles C.  When you
write programs using EasySplitter the library code is linked directly into
your program.  You don't need to distribute a DLL to make it work.

You must place the library (EASYSPLIT.LIB) and include file (EASYSPLIT.H)
in folders that Pelles C knows to search when compiling and linking your
projects.  Generally it is best not to place these files directly in the
PellesC folders.  You should create your own folder for 3rd party libraries
and add it to the Folders lists in POIDE's Tools/Options/Folders dialog.

With this arrangement, a "pragma" in the include file will make sure the
linker can find the library when you compile and link your programs.

Please Note: EasySplitter was developed for Pelles C.  However there is
nothing specific to Pelles C in the way it works.  It should not be too
much trouble to adapt it to other implementations of the language.



REGISTERING THE "EASYSPLIT" WINDOW CLASS

Before you can use EasySplitter in your programs you need to register its
window class.  The EasySplitter include file (EASYSPLIT.H) includes a
function that does this for you...

    ATOM RegisterEasySplit(HINSTANCE inst);

You must call this function, supplying the instance handle of your program,
before creating splitter windows.  inst cannot be NULL.



CREATING SPLITTER WINDOWS

EasySplitter windows are created like any other window control.  You must
use the WS_CHILD style or the window creation will fail.

Example:

    CreateWindowEx(WS_EX_DLGMODALFRAME, "EASYSPLIT", NULL
                  WS_CHILD | WS_VISIBLE | ES_VERT | ES_LINE | ES_TRACK,
                  100,0,11,200,hMainWindow,NULL,ghInstance,NULL);


There are several window styles that are specific to EasySplitter:

ES_HORZ   Creates a horizontal splitter that can only be moved vertically
ES_VERT   Creates a vertical splitter that can only be moved horizontally
ES_LINE   Draws a line in the middle of the splitter bar
ES_TRACK  Emits WM_SIZE messages with every mouse move.
ES_DOCK   Makes parent window borders magnetic to the splitter



EASYSPLITTER MESSAGES

There are several messages you can send to an EasySplitter window.  The
messages are:

ESM_UNDO          -- Return Splitter to position of last mouse capture.
                     WPARAM = 0,
                     LPARAM = 0,
                     Return = New splitter position.

ESM_STOP          -- abort mouse tracking
                     WPARAM = 0,
                     LPARAM = 0,
                     Return = 0

ESM_SETCURSOR     -- change the mouse cursor displayed over a splitter
                     WPARAM = 0,
                     LPARAM = Handle of new cursor
                     Return = 0

ESM_SETCOLORS     -- change foreground and background colors
                     WPARAM = Foreground color
                     LPARAM = Background color
                     Return = 0

ESM_SETTRACKING   -- enable/disable mouse tracking of splitters
                     WPARAM = 0
                     LPARAM = TRUE = splitter follows mouse
                     Return = 0

ESM_SETKBMOVE     -- enable/disable arrow keys for moving splitters
                     WPARAM = 0
                     LPARAM = TRUE = arrow keys move splitters (default)
                     Return = 0

ESM_SETLINE       -- enable/disable the center line drawn in splitters
                     WPARAM = 0
                     LPARAM = TRUE = draw center line in splitters
                     Return = 0

ESM_SETBORDER     -- set borders beyond which splitter will not move
                     WPARAM = width of border.
                     LPARAM = Border to set.  One of ESB_LEFT, ESB_TOP,
                              ESB_RIGHT, ESB_BOTTOM, ESB_ALL
                     Return = 0

ESM_SETDOCKING    -- enable/disable and set width of magnetic borders
                     WPARAM = width of magnetic border (default = 10)
                     LPARAM = TRUE = borders are magnetic
                     Return = 0

ESM_SETPOS        -- Move the splitter inside it's parent
                     WPARAM = 0
                     LPARAM = new position of splitter bar
                     Return = adjusted position of splitter

ESM_GETPOS        -- Get current splitter position (See WM_SIZE info)
                     WPARAM = 0
                     LPARAM = 0
                     Return = adjusted position of splitter


EasySplitter windows emit the following message when the splitter bar is
moved:

WM_SIZE         Sent by EasySplitter windows to their parent windows. If
                the ES_TRACK style is used, EasySplitter windows will emit
                a stream of WM_SIZE messages as the user moves the mouse.
                If you do not use ES_TRACK only one WM_SIZE is sent when
                the user releases the mouse.  When the control has keyboard
                focus, a WM_SIZE message is sent after each keystroke.

                Inside your WM_SIZE handlers you need to call ESM_GETPOS to
                retrieve the position of each splitter window, redraw the
                splitters and then rearrange your other child windows
                around the splitters.


WARNING:        To programatically move a spiltter from inside a WM_SIZE
                handler DO NOT use ESM_SETPOS or ES_UNDO. This will cause a
                message race and may cause system lockups. Use MoveWindow
                or SetWindowPos api calls instead.  ESM_SETPOS and ESM_UNDO
                are safe when used ouside of WM_SIZE handlers.


NOTE:           Because of the way WM_SIZE messages are dispatched in
                Windows.  The new ES_DOCK style and ESM_SETDOCKING message
                will only work on systems where the "Show window contents
                while dragging" feature of the Windows Desktop is enabled.



DEALING WITH EASY SPLITTER'S MESSAGES

Here is an example of how you deal with Easy Splitter windows in your
WM_SIZE code, taken from the demo program:

    LRESULT ResizeWindows(void)
      { RECT wr, esv, esh;
        INT pos;
        // Get parent size information
        GetClientRect(MainWind,&wr);

        // relocate the vertical splitter
        pos = SendMessage(VSplitter,ESM_GETPOS,0,0);
        MoveWindow(VSplitter,pos-4,0,8,wr.bottom,0);
        GetChildRect(VSplitter, &esv);

        // relocate the horizontal splitter
        pos = SendMessage(HSplitter,ESM_GETPOS,0,0);
        MoveWindow(HSplitter,esv.right,pos-4,(wr.right - esv.right),8,0);
        GetChildRect(HSplitter, &esh);

        // fit left window beside vertical splitter
        MoveWindow(Child1,0,0,esv.left,wr.bottom,0);

        // fit top window above and to the right of the splitters
        MoveWindow(Child2,esv.right,0,
                  (wr.right - esv.right),esh.top,0);

        // fit the bottom window below and to the right of the splitters
        MoveWindow(Child3,esv.right,esh.bottom,
                  (wr.right - esv.right),(wr.bottom - esh.bottom),0);

        // force a repaint of the entire window
        InvalidateRect(MainWind,&wr,0);
        return 1; }



BORDERS AND DOCKING

These features are new in Version 2.0.

Each splitter is aware of it's parent window's dimensions when processing
the ESM_GETPOS message.  During this time the splitter positions are
adjusted to confine the splitter inside it's parent window and inside any
internal borders you've set.

A Border is simply a distance, measured in pixels, from the edge of a
window.  For example a verticla splitter set with a right border of 20 will
never go further right than the right side of it's parent window less 20
pixels.  You can set individual borders for each Splitter in a given parent
window.  You set borders, using the ESM_SETBORDER message.  For a vertical
splitter you can set left and right borders using ESB_LEFT and ESB_RIGHT.
For a horizontal splitter you can set top and bottom borders using ESB_TOP
and ESB_BOTTOM.

Docking is a new feature that mostly works.  You can use the ES_DOCK style
or send the ESM_SETDOCKING message to enable splitters to latch on to the
border edge of their parent windows.  When a splitter is docked it will
stay on it's border relative to the nearest edge (left and right for
vertical splitters, top and bottom for horizontal ones) giving the
appearance of tracking outward with the parent window's edge as the user
resizes it. It should be noted this feature is not 100% reliable due to the
way WM_SIZE messages are handled by Windows.  If the user gets ahead of the
splitter by more than the width of the magnetic edge (eg. during very fast
mouse moves) the splitter will become unstuck.



ADDITIONAL FUNCTIONS

The EasySplitter include file (EASYSPLIT.H) includes one additional
function that I've found very useful:

    BOOL WINAPI GetChildRect(HWND hWin, PRECT pRect);

You give it the handle of a child window and a pointer to a RECT structure
and it fills in the RECT with the child window's coordinates inside the
client area of it's parent window.  This is very handy when using
MoveWindow or SetWindowPos with controls as there is no need to convert
back and forth between screen and client coordinates.



THE DEMO PROGRAM

ESDemo is the EasySplitter demonstration program.

As is usually the case the best example is working code.  The demo program
creates a horizontal and a vertical splitter and uses WM_SIZE to redraw the
screen when either the parent window is resized or a splitter is moved.  It
uses most of the EasySplit functions and messages, so you can see it in
action.



LICENSE AND STUFF

EasySplitter is released with no promises of anything.  If you have trouble
with it, feel free to contact me, but don't expect to hold me liable for
anything.

This project is supplied free of charge, with source code, and can be
included in any project you wish.

If you make changes and wish to distribute an altered version you must
document the original authorship and the changes you've made.  You must
also make it clear that your version is altered.

You can send comments and suggestions to me at:  fware@start.ca

Enjoy!



EASY SPLITTER HISTORY


Version 2.0

This is a full rewrite introducing several new features.

  1) The problem with ESM_STOP has been eliminated

  2) Settable borders that limit the motion of the splitter bars inside
     their parent windows (See the demo program for examples)

  3) Several new messages toggling various features (see above)

  4) New message WM_SETPOS allows setting the Splitter's position

  5) Simplified WM_SIZE handlers using new ESM_GETPOS message.

  6) Docking... Window borders can be made Magnetic.  Splitters can now
     dock to magnetic borders and follow the edges of their parent windows
     during window resizing.

  7) Inward Right and Bottom border moves no longer plow over splitters.
     Splitters will track inside their parent windows when the window is
     resized inward.

  8) A behaviour change had to be made to accomodate the new features.

     a. Inside WM_SIZE handlers you *must* call the ESM_GETPOS message for
        each splitter.  The return value is the position of the centerline
        of the splitter, in child coordinates relative to the splitter's
        parent window.
     b. The Easy Splitter code no longer moves the splitter window for you.
        As it turned out, in practical use, the WM_SIZE handler most often
        had to re-size the splitter windows to accomodate other window
        elements, resulting in a lot of redundent window drawing that is
        eliminated in this version.

  9) The demo program has been updated to version 2.0 as well.


Version 1.2

1) Using ESM_STOP no longer releases mouse capture.

2) If mouse not captured ESM_STOP does nothing.


Version 1.1

1) New default cursors.  Vertical splitters use the IDC_SIZEWE and
   horizontal ones use ICD_SIZENS cursors. (Suggested by Timo Lahde>

2) Fixed a minor bug in drawing of disabled windows.


Version 1.0

1) Splitter bars can now accept keyboard focus and can be moved using the
   arrow keys.  Left, Right, Home and End move vertical splitters.  Up,
   Down, PgUp and PgDn move horizontal ones.  To use this feature just add
   the WS_TABSTOP style when creating splitter windows.

2) The drawing routines have been improved to provide a more efficient
   response to WM_PAINT and to draw a focus rectangle when the splitter bar
   gets keyboard focus.

3) Splitter windows can now be enabled and disabled, like other control
   windows.  When disabled they do not respond to keyboard or mouse actions
   and if ES_LINE is used, the center line is removed, providing visual
   cues for the user.


Version 0.5

  First version, not widely distributed.





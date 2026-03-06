# Highlander
Highlander is a hobby game engine I have been working on for about 6 months. Currently it is purely for fun!

It currently works only on debian based linux distros with the ASSIMP library installed. You can install it with the following commands:

sudo apt update
sudo apt install libassimp5

If that doesn't work, idk do something it's in alpha it's probably going to break

## How To Use
This only currently applies to Alpha 1.2 because the UI is slightly different (not really)

first you need to run the executable and make sure it doesn't crash (if it does it's probably a missing library).
If that works then you are now in the viewport and your mouse will be locked. What you need to do from there is simple. Click the = key on your keyboard to lock/unlock your mouse.
When it is locked you can look around with the mouse and move with WASD.
Now you are in the engine hooray! If the window panels in the engine are really small you can snap/resize them to your liking.
To load the demo file you need to go into the Highlander file and unzip the tar.gz. then, click File and then Open Project and type texture_tester exactly. It might crash if you type it wrong
or if it is still compressed when you try to load it. On second thought, keeping it compressed and being able to load it would be cool! I'll add that soon (hopefully)!

Now that you have a file loaded you can modify it how you want! To change global lighting settings for the engine click the lighting dropdown menu and
some global lighting setting will appear such as ambient light!

To add/modify objects, click the Objects button at the top in the settings window! There should be new buttons and 2 lists. A light list and an object list. You can select objects
and lights from here and modify your scene. To add a new object (currently supports only obj's), type in the full path to an obj file you would like to add at the top of the window.
Then, Just below it click Select Item type, Object, and then Add OBJ. It will take a second to load but then your object should show up! To add a light, Click Select Item type, then
click Light, and Add Point Light! Congratulations you can use the really early versions of Highlander now! Good job I guess

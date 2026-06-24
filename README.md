# Highlander
Highlander is a hobby game engine I have been working on for a while.

It's currently in Alpha so some features will be confusing/broken. Hope you have fun trying it out though!

## How To Use
This tutorial only applies to Highlander Alpha 1.4 and later due to the UI being different.

Here are the steps to get started!

1: Open the executable (obviously)
2: Click the = symbol on your keyboard to lock/unlock your mouse
3: Click a project if you have one (if not click new project)
4: To add an object, go to Add or Import in the top bar and navigate to the Import OBJ or OBJ button.
5: Find your OBJ in the file browser. You won't see it at first because there's no materials loaded!
6: Find a material (a set of a diffuse, normal (GL), and ARM texture
7: Import the images for each material
8: go to the Object list on the left and click on your object
9: Go to the Materials Dropdown menu and click the material you just made.
10: You won't see the object because there's no light!
11: Go to the dropdown just above the object properties that says Select Item type. This is where you can manually add an OBJ (which will be deprecated soon) and where you can add lights!
12: After clicking the dropdown select Lights and then Add Point Light. Don't worry it'll be simpler in future updates.
13: Modify the light in the light menu so that you can see your object clearly
14: Congrats! You made your first object in Highlander! If you can't see it, move with WASD or look around to find it.

## Saving Your Project
This applies to anything Alpha 1.2 and up!

1: Click File in the Top Left
2: Hover over the save button and a menu will show up where you can name your project and save it!

## Loading Your Project
You can load it in the splash screen or anytime in the editor! Follow the steps to load it in the editor:

1: Click File in the Top Left
2: Hover over the Load Button and all your projects will show up
3: Click any of them to load the new project (make sure to save what you have first)

## How to Playtest
This applies to anything Alpha 1.3 and up!

1: Navigate to where you want to start the game
2: Click Test in the top and then click Launch In Editor
3: Click Stop Playing in the same menu if you want to stop playing

## Installing on Debian Linux

If you are on debian, it won't work without the assimp library. You can install it with the following commands:

sudo apt update && sudo apt install libassimp5

If that doesn't work, idk do something it's in alpha it's probably going to break

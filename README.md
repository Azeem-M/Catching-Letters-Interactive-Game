# Catching-Letters-Interactive-Game

Game Description:
Players start with a blank black screen with a red line near the bottom of the
screen. Letters start appearing near the top of the screen, randomly from A
through Z and move downwards. The goal of the game is for the user to last as
long as possible without letting any of the letters hit the red line.
This game can be used as typing practice, as it can increase the users typing
coordination and speed. It forces them to type letters on a keyboard as fast as
possible.
How to Play:
The game is played with the use of the PS/2 keyboard device. The user is to
type the appropriate keys on this device in order to ‘catch’ the letters while
they are on their way to the red line. If a user types any key and the letter for
that corresponding key is not on the screen at that instance, then nothing will
occur and the letters will keep moving down. When the user ‘catches’ a letter, a
new one will be displayed near the top half of the screen. The game over
screen displays once any letter hits the red line.
The switches on the De1-SoC board have been also used for the games
functionality. Putting SW1 high will result in the game to pause. Putting SW2
high will result in the game being reset. The game will continue to reset at
random as long as SW2 is high. The remaining switches have no use.
An effective game strategy would be to focus on keys in the lower half of the
screen, as typing the letters in the top half will keep creating more near this
region. Another useful strategy is to look for letters that appear more than
once and try to eliminate those in a combination.
Attribution/Task Division:
The task division for this project development is as follows:
Member 1: Azeem Mohammad
●Implemented the array with all the needed information for the games
letters
○An array of structs, the structs have randomized values for letters,
positions, etc.
●Game loop
○Set up logic for what to do when the game is being played,
conditions that cause the game to be over, what to do when the
game is over, how to restart the game
●Input devices
○Dealing with getting input from the switches (SW1, SW2) and the
keyboard data from the PS/2 keyboard
●Testing
○Played the game several times to debug any errors and check for
any incorrect drawings
Member 2: Suleiman Najim
●Plotting letters graphics
○Implemented a function that works similar to plot_pixel but takes
in an additional argument which is a letter value. Based on this
letter, it draws the correct letter onto the screen using the
corresponding pixel patterns
●Game Over screen
○Implemented the game over screen to have a big letter message
using a function called plot_pixel_big
○Screen is displayed once the red line is hit by any letter
●Testing
○Played the game several times to debug any errors and check for
any incorrect drawings

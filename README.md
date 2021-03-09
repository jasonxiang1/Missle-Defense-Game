# Missle Defense Game

# Configuration
If running on VS Code, make sure to configure the CMake build version to GCC 5.5.0. Versions higher than this will cause compile errors.

# How to Run


## General Workflow

1. Open startup.cpp. Set the desired difficulty level.
2. Open Controller.cpp. Set the teleportationCheat variable to either true or false.
3. Configure the binInterval and safetyRad variables to vary performance
    * (default) binInterval = 2.5
    * (default) safetyRad = 4
4. Build the CMake and Run the programs through terminal
5. Press 'C' for the AI controller. Press 'space' for manual conrol of the player.

# Grading Section

## Question 01: AI System Overview

The AI controller to the program is derived from the functions created in theController.cppfile.Within theController.cppfile, thecontrolfunction serves as the main brains of the operation.  The overallflow of the AI system is as shown below.  The AI first predicts the landing locations of allprojectiles in the air, determines the danger zones on the map and if the player is standing on a danger zone,if so then the AI will determine a safe direction to move and move until the it reaches the safe part of themap.

![system overview](img/img01.png)

Once a player is generated, a sectioned out map of the game is created based on a pre-specified bin intervalvalue. In this program, the default bin interval parameter is set to 2.5. The first step for the AI is to determinewhere all the projectiles are going to land on the map.

In thetrackProjectiles function, the projectile information is pulled from the game class inputg. The func-tion uses common kinematics equations to determine the time of impact and the landing location of theprojectiles. The following kinematics equations are used:

<div class="math">
\begin{equation*}
    \begin{gathered}
    v_{fy} = \sqrt{v_{iy}^2 + 2*g*v_y} \\
    \Deltav t = \frac{-v_{fy}+|v_{iy}|}{g} \\
    \text{if $v_{iy} >$ 0: } \Deltav t = \Deltav t \frac{2*v_{iy}}{|g|}\\
    \Deltav x = v_{ix}* \Deltav t \\
    \text{Predicted Time = } \Delta t \\
    \text{Predicted Location = } \Deltav x + x_{i}
    \end{gathered}
\end{equation*}
</div>
ThetrackProjectilefunction outputs the predicted time and location values for each projectile.  Calculatedtime-of-impact and location-of-impact values are stored in the pre-defined classprediction.  Thepredictionvariable is a global variable that can be seen and modified by other functions in theController.cppfile.Afterwards, danger zones are determined on the map based on these predicted values.

In  thedetermineSafeSpotsfunction,  the  system  runs  through  both  the  elements  in  the  globalpredictionvariable and in theexplosionclass that is stored in the game classg.  A safety radius is prespecified whichis the buffer distance around the centroid of the location.   All bin locations within that zone is set to 1on thebinVectvariable which is a one-hot vector to denote whether or not each increment of location onthe map is safe (i.e.  0) or dangerous (i.e.  1).  In addition to determining danger zones based on the datain thepredictionvariable, the function also determines danger zones based on the locations of the currentexplosions.  Elements in theBinVectvariable, which denote to each increment of the map, is set to 1 at thecentroid of the danger as well as within a certain distance away from it. ThedangerSetterfunction was used to set the element values to hot (i.e. 1)

After determining the danger zones in map, next the AI will determine if the player’s location is on a dangerzone and act accordingly.  Because thebinVectis a global variable, thecontrolfunction determine if thecurrent bin that player is on is a danger bin. If the current location is counted as a danger bin, then the player will go to thepickSafeSpotfunction to determine which direction to go to avoid the imminent danger.

In thepickSafeSpotfunction, the AI system looks to the left and right sides of the player.  If the left sidehas more safe zones (i.e.  has more 0’s) than the right side, then the function will output an integer signalwhich denotes to which direction to move.  Likewise, the function will output a right-moving signal if thenumber of safe zones on the right side of the player is greater than the number of safe zones on the rightside of the player. On the edge case scenario where both the left and right side of the player are equal, thenthe function will randomly select a direction for which the player will traverse. 

## Question 02: Development Challenges

The main challenges apparent in this task were:

* Handling environmental edge cases One particular edge case is when two predicted explosions areoccurring next to each other.  After the AI determines a direction to move, it must continue movingin that direction until it reaches a bin location that is predicted to be safe. If more than one predictedexplosion is occurring right after the other, then the next safe location is after that particular explosion.This is an issue due to the relatively slow speed of the player because the player is usually only fastenough to move away from one predicted explosion in time, not 2 or more. One method to handle thisedge case would be to develop a limited range on the left and right side of the player when determininga safe direction to travel. But implementing this feature brings about further edge case failures.
* Sectioning the map optimally When determining the resolution of the bin locations on the map, thereis a balance between performance and efficiency. One must select a resolution that is sparse enough tobe efficient yet fine enough to accurately characterize the environment. A lower resolution of the mapwould bring out an inaccurate depiction of where predicted explosions would occur.  In the extremecase, the AI will think that the significantly more spaces on the map are occupied by explosions whenit  is  not  the  case.   At  the  same  time,  a  higher  resolution  map  would  prove  to  be  computationallyexpensive.  The balance between the two is one that must be resolved empirically through trial-and-error data where edge cases are heavily considered.
* Avoiding ”drifting” AI behavior For example, one issue for AI in this game is opting to drift in onedirection.  Whether it be the left or right, the player would fall further and further to one extreme ofthe map. This may prove difficult to handle because maneuverability is limited when one direction isthe only choice when the player is in the corner.  Generally, developing an AI entails implementingsafeguards and effective generalization to ensure the player does not fall to local minima maneuvers.

## Question 03: Performances on Harder Difficulties

In the hard difficulty mode, the AI can last for an average for 5-15 seconds. The main challengewith the increased difficulty is the higher likelihood of overlapping and consecutive explosion occurring.The AI focusing on the explosion closest to the player, namely the explosion directly overhead, and adjustsits position appropriately to avoid it. In many cases in the harder difficulty, the player may be able to moveaway from the explosion directly overhead, but the player ends up walk right into an explosion that justoccurred right after.

## Question 04: Reflections and Thoughts on Assignment

For giving students hands-on experience developing C++, this assignment is able to meet its goal.Working on this assignment solidified several C++ concepts such as vectors, pointers, references, classes,and variable  types (static,  global,  local,  etc...).   This  assignment requires  the student  to apply  a healthydiversity of concepts to get a working AI model for the player.

One  suggestion  for  improvement  would  be  to  provide  a  failed  AI  model  as  an  example  for  students  toimprove or replace. This way, students do not have to experiment and test different parts of the program tounderstand the flow and coding structure of the program.  Seeing a poor/failing AI model working wouldopen students’ eyes to the fundamental code that needs to be implemented to get something working onthe program. In addition, the time saved from this implementation would be greatly appreciated as studentsmay not have the time luxury in the middle of the semester to tinker around with the code to get a basic AIworking.


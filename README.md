# Lifesim

Lifesim is a life simulator which aims to disperse a collection of "pawns" to the world and they interact based on random features each possesses; such as lifespan, mating radius and mating factor, etc. These factors affect the likelihood of mating in each season (frame) and can produce offspring between the two mated pawns. Pawns can die due to old age, or starvation or killed if too many pawns are in its immediate radius. A pawn can randomly migrate in a season based on the migration probability in the config, in fact all of the above effects and more can be modified in the config.ini file.

## Installation
1. clone the repo
2. clone and build [Raylib](https://github.com/raysan5/raylib), then install to /usr/local/lib
3. clone and build [Unity](https://github.com/ThrowTheSwitch/Unity), then install to /usr/local/lib
4. run make in project directory to build

## Usage
>```$bin/lifesim 1000```
>
>>this runs lifesim with the parameters contained in config.ini with 1000 starting pawns.
>>
>>press space to pause simulation, press esc to quit

![output](https://github.com/user-attachments/assets/e2020b76-61fd-4b21-9d17-d304b016ee4d)

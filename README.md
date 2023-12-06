# EnhancedSkyMap
Enhanced Sky Dome for Unreal Engine with real world Sun and Moon positioning.

## Aims
This makes use of Ephemeris Library (https://github.com/MarScaper/ephemeris) to calculate realistic Sun and Moon positioning to generate a Sky Dome in Unreal Engine.
Current Version of the project is for Unreal 5.3.2 or greater.

Orbit calculations and date/time conversions are done in C++, the rest is done in Blueprints.

Included are realistic sky textures taken from NASA (https://svs.gsfc.nasa.gov/4851/) and further processed for rendering.

Sky dome includes a pre-built constelations map texture and a Milky Way + Andromeda low spectrum overlay.

The project comes with a demo map to check positionings and other possible settings.

This was inspired by some tutorials on YouTube (listed bellow), picked from the Sun Position Calculator plugin (comes with the engine), but redone almost from scratch and full refactored code as an excercize to study Unreal Engine architecture and its funcionalities.
<br>https://www.youtube.com/watch?v=Zg2NaZcyAjk
<br>https://www.youtube.com/watch?v=88-wwT0uH5k
<br>https://www.youtube.com/watch?v=Et9ZS5xbeJs


# License
Free for open and commercial use (see licenses attached).

That's it.
Enjoy :)

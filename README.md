# KhuGle
* KhuGle: Open source platform for game programming and data analysis (Kyung Hee University Data Analysis & Vision Intelligence)
* C++, (Game layout, Collision, Physics, Sound/Image Processing)

## Source file
* Main.cpp
  + Main routine for game and data analysis
* KhuGleWin.cpp / KhuGleWin.h
  + GUI class using Windows API
* KhuGleComponent.cpp / KhuGleComponent.h
  + Base class for scene, layer, sprite
  + KhuGleScene.cpp / KhuGleScene.h
  + KhuGleLayer.cpp / KhuGleLayer.h
  + KhuGleSprite.cpp / KhuGleSprite.h
* KhuGleBase.cpp / KhuGleBase.h
  + Point, line, rectangle, vector (2D/3D), memory allocation
* SoundPlayWin.cpp
  + Playing wave sounds

## Projects
* 01 KhuGle(Windows Api)
  + Window, F11 (full screeen), keyboard and mouse events
* 02 KhuGle(Game Layout)
  + Scene, layer, sprite (rectangle, ellipse), static move
* 03 KhuGle(Colligision and Physics)
  + Circle-to-circle collision, circle-to-line collision, gravity
* 04 KhuGle(3D Renderging)
  + Projection and view matrices, 3D object rendering
* 05 KhuGle(Sound Processing)
  + FFT, spectrogram, FIR/IIR filters

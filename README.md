# KhuGle
* KhuGle: Open source platform for game programming and data analysis (Kyung Hee University, Data Analysis & Vision Intelligence)
* C++, (Game layout, Collision, Physics, Sound/Image Processing, Data analysis)

## Lecture note
*  pdf file [link](Via%20Game%20Programming(20210704).pdf)

## Source files
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
 
## Visual Studio Setting
* Project setting (Configuration Properties)
  + Target Platform Version: your platform version
  + Platform Toolset: Your platform
  + Character Set: Multi-Byte Character Set


## Projects
* 01 KhuGle(Windows Api)
  + Window, F11 (full screeen), keyboard and mouse events
* 02 KhuGle(Game Layout)
  + Scene, layer, sprite (rectangle, ellipse), static move
* 03 KhuGle(Collision and Physics)
  + Circle-to-circle collision, circle-to-line collision, gravity
* 04 KhuGle(3D Renderging)
  + Projection and view matrices, 3D object rendering
* 05 KhuGle(Sound Processing)
  + FFT, spectrogram, FIR/IIR filters
* 06 KhuGle(Image Processing)
  + Convolution, DCT, image compression
* 07 KhuGle(Correlation and Clustering)
  + Pearson correlation, similarity, k-means
* 08 KhuGle(Regression)
  + Least squares, parameter estimation
* 09 KhuGle(Performance Evaluation)
  + Accuracy, presision, recall, f-score, ROC
* 10 KhuGle(Perceptron), [1.KhuDaNet-Perceptron](https://github.com/NizeLee/KhuDaNet/tree/main/1.KhuDaNet-Perceptron)
* 11 KhuGle(MLP-DNN), [2.KhuDaNet-MLP-DNN](https://github.com/NizeLee/KhuDaNet/tree/main/2.KhuDaNet-MLP-DNN)
* 12 KhuGle(CNN), [3.KhuDaNet-CNN](https://github.com/NizeLee/KhuDaNet/tree/main/3.KhuDaNet-CNN)

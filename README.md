(please wait for the gifs to load)
![Sample Gif](https://github.com/508312/public-files/blob/master/demo/gif_hand.gif)
![Sample Gif](https://github.com/508312/public-files/blob/master/demo/gif_auto.gif)

#**STATE OF THE PROJECT**
I work on this project when I have free time so it is not as polished as it can be. 
It is semi-actively being worked on, and there are a few more things I would like to add before distributing.

###**Building**
Feel free to build the project, you will need the following libraries: OpenCV 4.6.0, SDL2 2.28.1, OneTBB. 
The classes folder is src. When building on mac or linux, you will have to comment out the windows DPI settings(first two includes and first line in main).
I am currently working on this project alone, so I am yet to make a proper way to build the project. 
If you run into any issues contact me.

###**Some technical stuff**
Want to build gif creator, simple gui before distributing. Before that will have to make a pretty driver class, because currently ImageBuilder owns all CompositeImages, and I initialize all the other major classes from outside, a little ugly but it works fiine. Will make a good driver class which will do initialization and own objects with use of smart pntrs for defined lifetime.

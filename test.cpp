#include <opencv2/core/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include <cmath>

#include <windows.h>
#include <winuser.h>

#include "Timer.h"
#include "Color.h"
#include "CompositeImage.h"
#include "ImageBuilder.h"
#include <vector>
#include "Guimage.h"
#include <ImageBuilderCimplementation.h>
#include <unordered_map>

#include "TextureLoader.h"

void event_handler(int event, int x, int y, int flags, void* userdata){
   if (event == cv::EVENT_MOUSEWHEEL) {
        std::cout << "calling on " << float(flags/65536) * 0.001 + 1 << std::endl;
        std::cout << "calling event on x " << x << " y " << y << std::endl;
        ((Guimage*)userdata)->move_cam_pos_based_on_mouse(x, y, float(flags/65536) * 0.001 + 1);
        ((Guimage*)userdata)->increment_zoom(float(flags/65536) * 0.0001 + 1);
        ((Guimage*)userdata)->generate_image();
        ((Guimage*)userdata)->show();
   }
}

int main( int argc, char* args[] ) {
    SetProcessDPIAware();

    cv::namedWindow( "Display window", cv::WINDOW_NORMAL );
    cv::resizeWindow( "Display window", cv::Size(1600, 1600));
    //cv::setMouseCallback("Display window", locator, NULL);//Mouse callback function on define window//

    TextureLoader test_loader({1600, 800, 400, 200, 100, 50});

    std::cout << "size of guimage " << sizeof(Guimage) << std::endl;
    std::cout << "size of composite image " << sizeof(CompositeImage) << std::endl;
    std::cout << "size of image builder " << sizeof(ImageBuilder) << std::endl;

    ImageBuilder builder(320, 1600, 1600, 1, 10, 1, &test_loader);
    builder.load_images("imgsmall");
    builder.build_images();

    std::vector<CompositeImage> images = *(builder.get_images());

    Guimage test(&test_loader, &images[0]);

    float zoom = 1;
    test.change_zoom(zoom);
    test.change_cam_pos(800, 800);
    test.generate_image();
    test.show();


    cv::setMouseCallback("Display window", event_handler, &test);
    cv::Mat ii;

    for (int i = 0; i < 30; i++) {
        test.increment_zoom(1.2);
        test.generate_image();
        test.show();
        cv::waitKey(1);
        //ii = test.get_image();
        //cv::imwrite("anim/" + std::to_string(i) + ".jpg", ii);
    }
    std::cout << "ZOOMED " << std::endl;
    /*
    for (int i = 0; i < 10; i++) {
        test.increment_zoom(1.12137);
        test.generate_image();
        test.show();
        cv::waitKey(1);
        //ii = test.get_image();
        //cv::imwrite("anim/" + std::to_string(i) + ".jpg", ii);
    } */

    cv::waitKey(0);
    cv::destroyWindow("Display window");

    return 0;
}


    /*
    averages

    for(int i = 0 ; i < 300; i++) {
        for(int j = 0; j < 300; j++) {
            cv::mean(image(piece));
            piece += cv::Point(small_x, 0);
            for(int k = 0; k < 3000; k++) {
                (543.213) * (4362.342) + (6453.2) * (6342.8) + (21356.2) * (4384.4);
            }
        }
        piece -= cv::Point(piece.x, -small_y);
    }
    */

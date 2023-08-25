#ifndef STARTING_VARIABLES_H
#define STARTING_VARIABLES_H

/** Variables to be set at the start of the program. **/
struct starting_variables
{
    /** Display window dimensions. **/
    int window_width, window_height;

    /** Image dimensions. **/
    int image_width, image_height;

    /** How much to upscale compiled image. **/
    float compiled_upscale;

    /** Number of parts image will be divided into. **/
    int image_number_of_cols, image_number_of_rows;

    /** Width when the image should be rendered as it's parts. **/
    int detailed_image_width;

    /** Width when the part image should be rendered as separate full image **/
    int local_transition_width;

    /** Parts with total count below this threshold will be replaced by other parts from the image. **/
    int prune_threshold;

    /** If distance to previous image is below this number, previous image will be used. **/
    int closeness_threshold;

    /** Mipmaps of the original image. **/
    std::vector<int> resolutions;

    /** Width when the image from lookup should be loaded in the memory. **/
    int load_width;

    /** "software" or "metal". GPU or CPU rendering. **/
    char* render_type;

    /** Folder from which to load images. **/
    char* folder;
};

#endif // STARTING_VARIABLES_H

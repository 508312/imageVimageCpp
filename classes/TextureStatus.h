#ifndef TEXTURESTATUS_H
#define TEXTURESTATUS_H

/** Various texture statuses for imageLoader **/
enum TextureStatus : uint8_t {
    TEXTURE_NOT_LOADED = 1,
    TEXTURE_STARTED_LOADING = 2,
    TEXTURE_LOADED = 3,
    TEXTURE_STARTED_UNLOADING = 4,
};

#endif

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "ppm.h"

pixel_t img_t::operator()(int i, int j) const {
    return (this->data)[i * this->w + j];
}

pixel_t &img_t::operator()(int i, int j) {
    return (this->data)[i * this->w + j];
}

img_t *read_ppm(const char *fname) {
    FILE *img_file = NULL;
    if (!(img_file = fopen(fname, "rb"))) {
        fprintf(stderr, "error: could not open file %s\n", fname);
    }
    int w = 0;
    int h = 0;
    img_t *img = NULL;
    if (fscanf(img_file, "P6 %d %d 255%*c", &w, &h)) {
        img = img_init(w, h);
    } else {
        fprintf(stderr, "error: invalid ppm file header\n");
        return NULL;
    }

    if (!fread(img->data, 3, w * h, img_file)) {
        fprintf(stderr, "error: invalid ppm file format\n");
        return NULL;
    }

    fclose(img_file);
    return img;
}

int write_ppm(const img_t *img, const char *fname) {
    if (!(img && fname)) {
        fprintf(stderr, "error: illegal argument\n");
        return 0;
    }
    FILE *img_file = NULL;
    if (!(img_file = fopen(fname, "wb"))) {
        fprintf(stderr, "error: could not open file %s\n", fname);
        return 0;
    }
    fprintf(img_file, "P6\n%d %d\n255\n", img->w, img->h);
    fwrite(img->data, img->w * img->h, 3, img_file);

    fclose(img_file);
    return 1;
}

img_t *img_init(int w, int h) {
    if (!(w > 0 && h > 0)) {
        fprintf(stderr, "error: cannot initialize image with "
                        "dimensions %d by %d\n", w, h);
        return NULL;
    }
    img_t *img = (img_t *) malloc(sizeof(*img));
    img->w = w;
    img->h = h;

    img->data = (pixel_t *) malloc(w * h * sizeof(*(img->data)));
    memset(img->data, 0, w * h * sizeof(*(img->data)));

    return img;
}

img_t **img_destroy(img_t **img) {
    free((*img)->data);
    (*img)->data = NULL;
    free(*img);
    *img = NULL;
    return img;
}

#ifndef __PPM_H__
#define __PPM_H__

typedef struct pixel_t {
    unsigned char r, g, b;
} pixel_t;

typedef struct img_t {
    pixel_t *data;
    int w, h;
    pixel_t operator()(int, int) const;
    pixel_t &operator()(int, int);
} img_t;

/*
 * Initialize a new img_t with corresponding height and width.
 * If creation is unsuccessful, NULL is returned.
 */
img_t *img_init(int w, int h);

/*
 * Destroys an img_t. If successful, NULL is returned,
 */
img_t **img_destroy(img_t **img);

/*
 * Read a *.ppm file and return the corresponding img_t * struct.
 * If the reading is unsuccessful, NULL is returned.
 */
img_t *read_ppm(const char *fname);

/*
 * Write an img_t to fname. If the write is unsuccessful, 0
 * is returned.
 */
int write_ppm(const img_t *img, const char *fname);

#endif

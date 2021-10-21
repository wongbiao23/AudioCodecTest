#ifndef LIBSPEEX_H
#define LIBSPEEX_H

#include <stdint.h>
#include "config.h"
#include "speex/speex.h"

#define SPEEX_FRAME     320

/*!
 * @brief encoder or decoder
 *
 */
typedef enum libspeex_type_e {
    SPEEX_TYPE_ENCODER,
    SPEEX_TYPE_DECODER,
} libspeex_type_t;

/*!
 * @brief speex struct
 *
 */
typedef struct libspeex_s {
    void*           state;
    SpeexBits       bits;
    int             quality;
    int             size;
    libspeex_type_t type;
} libspeex_t;

/*!
 * @brief init libspeex_t
 *
 */
int libspeex_init(libspeex_t* ls, libspeex_type_t type, int quality);

/*!
 * @brief encode with speex
 *
 * in's size = 320 bytes
 * out's size must >= libspeex_t.size
 *
 */
int libspeex_encode(libspeex_t* ls, const uint8_t* in, uint8_t* out);

/*!
 * @brief decode with speex
 *
 * in's size = libspeex_t.size
 * out's size must >= 320 bytes
 *
 */
int libspeex_decode(libspeex_t* ls, const uint8_t* in, uint8_t* out);

/*!
 * @brief uninit libspeex_t
 *
 */
int libspeex_free(libspeex_t* ls);

#endif /* end of include guard: LIBSPEEX_H */

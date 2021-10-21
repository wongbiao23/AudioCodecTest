
#include <stdint.h>
#include <stdio.h>
#include "libspeex.h"


const int sx_q2s[] = {
    6, 10, 15, 20, 20, 28, 28, 38, 38, 46, 62
//  0, 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10
}; /**< quality to size */


int libspeex_init(libspeex_t* ls, libspeex_type_t type, int quality) {
    ls->state = NULL;
    ls->bits.owner = 0;
    ls->type = type;
    ls->quality = quality;
    if (quality < 0 || quality > 10) {
        return -1;
    }
    ls->size = sx_q2s[ls->quality];

    if (type == SPEEX_TYPE_ENCODER) {
        /*Create a new encoder state in narrowband mode*/
        ls->state = speex_encoder_init(&speex_nb_mode);

        /*Set the quality to 8 (15 kbps)*/
        int tmp = quality;
        speex_encoder_ctl(ls->state, SPEEX_SET_QUALITY, &tmp);

        /*Initialization of the structure that holds the bits*/
        speex_bits_init(&ls->bits);
    } else if (type == SPEEX_TYPE_DECODER) {
        /*Create a new decoder state in narrowband mode*/
        ls->state = speex_decoder_init(&speex_nb_mode);

        /*Set the perceptual enhancement on*/
        int tmp = quality;
        speex_decoder_ctl(ls->state, SPEEX_SET_ENH, &tmp);

        /*Initialization of the structure that holds the bits*/
        speex_bits_init(&ls->bits);
    } else {
        return -2;
    }

    if (ls->state == NULL) {
        return -3;
    }

    return 0;
}

int libspeex_encode(libspeex_t* ls, const uint8_t* in, uint8_t* out) {
    int i;
    float input[SPEEX_FRAME / 2];
    for (i = 0; i < SPEEX_FRAME / 2; i++)
        input[i] = ((int16_t*)in)[i];
        //input[i] = (float)((int16_t)(in[2 * i]) << 8 + in[2 * i + 1]);

    /*Flush all the bits in the struct so we can encode a new frame*/
    speex_bits_reset(&ls->bits);

    /*Encode the frame*/
    speex_encode(ls->state, input, &ls->bits);

    /*Copy the bits to an array of char that can be written*/
    speex_bits_write(&ls->bits, out, ls->size);

    return 0;
}

int libspeex_decode(libspeex_t* ls, const uint8_t* in, uint8_t* out) {
    float output[SPEEX_FRAME / 2];

    /*Copy the data into the bit-stream struct*/
    speex_bits_read_from(&ls->bits, in, ls->size);

    /* Decode the data */
    /* 0 for no error, -1 for end of stream, -2 corrupt stream */
    int ret = speex_decode(ls->state, &ls->bits, output);
    if (ret != 0) {
        return ret;
    }

    /*Copy from float to short (16 bits) for output*/
    for (int k = 0; k < SPEEX_FRAME / 2; k++) {
        out[2*k] = ((int16_t)(output[k])) & 0xFF;
        out[2*k + 1] = ((int16_t)(output[k]) >> 8) & 0xFF;
    }
    return 0;
}

int libspeex_free(libspeex_t* ls) {
    if (ls->type == SPEEX_TYPE_ENCODER) {
        /*Destroy the encoder state*/
        if (ls->state) {
            speex_encoder_destroy(ls->state);
            ls->state = NULL;
        }

        /*Destroy the bit-packing struct*/
        if (ls->bits.owner) {
            speex_bits_destroy(&ls->bits);
            ls->bits.owner = 0;
        }
    } else if (ls->type == SPEEX_TYPE_DECODER) {
        /*Destroy the decoder state*/
        if (ls->state) {
            speex_decoder_destroy(ls->state);
            ls->state = NULL;
        }

        /*Destroy the bit-stream struct*/
        if (ls->bits.owner) {
            speex_bits_destroy(&ls->bits);
            ls->bits.owner = 0;
        }
    } else {
        return -1;
    }
    return 0;
}

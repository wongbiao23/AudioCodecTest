#ifndef LIBGSM_H
#define LIBGSM_H

#include <stdint.h>
#include <stdbool.h>
#include "gsm.h"
#include "private.h"

/*!
 * @brief gsm instance struct
 *
 * `libgsm_t*` == `gsm` from gsm.h
 */
typedef struct gsm_state libgsm_t;

/*!
 * @brief init gsm instance as encoder or decoder
 *
 * @param type true for encoder and false for decoder
 */
void libgsm_init(libgsm_t* const g, bool type);

/*!
 * @brief macro for gsm_encode
 *
 * @param g libgsm_t* type as encoder or decoder
 * @param in uint8_t* type that saves audio data, 320byte
 * @param out uint8_t* type that used to save compressed data, 33bytes
 * @return return void
 */
#define libgsm_encode(g, in, out) gsm_encode((g), (gsm_signal*)(in), (gsm_byte*)(out))

/*!
 * @brief macro for gsm_decode
 *
 * @param g libgsm_t* type as encoder or decoder
 * @param in uint8_t* type that saves compressed data, 33bytes
 * @param out uint8_t* type that used to save uncompressed audio, 320bytes
 * @return return int which should == 0
 */
#define libgsm_decode(g, in, out) gsm_decode((g), (gsm_byte*)(in), (gsm_signal*)(out))

#endif /* ifndef LIBGSM_H */

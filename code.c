
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "libgsm.h"
#include "libspeex/libspeex.h"

#define GSM_VOICE_SIZE            320
#define GSM_FRAME_SIZE            33

// 多通道gsm编解码参数
#define GSM_SAMPLE_SIZE           2
#define GSM_CHANNEL               4
#define GSM_MULTI_VOICE_SIZE      (GSM_CHANNEL * GSM_VOICE_SIZE)
#define GSM_MULTI_SAMPLE_SIZE     (GSM_CHANNEL * GSM_SAMPLE_SIZE)
#define GSM_MULTI_FRAME_SIZE      (GSM_CHANNEL * GSM_FRAME_SIZE)

static int          sx_quality = 10; // 8

int read_data(const char* str, uint8_t** pbuf, size_t* len) {
    FILE* fp;
    fp = fopen(str, "rb");
    if (!fp)
        return -1;
    fseek(fp, 0, SEEK_END);
    *len = ftell(fp);
    *pbuf = (uint8_t*)malloc(*len);
    fseek(fp, 0, SEEK_SET);
    fread(*pbuf, 1, *len, fp);
    fclose(fp);
    return 0;
}

void save_enc_speex(const char* str, uint8_t* pbuf, size_t len) {
    libspeex_t ls;
    libspeex_init(&ls, SPEEX_TYPE_ENCODER, sx_quality);

    int32_t num = len / SPEEX_FRAME;
    uint8_t* buf = (uint8_t*)malloc(num * ls.size);

    for (int32_t k = 0; k < num; k ++) {
        libspeex_encode(&ls, pbuf + k * SPEEX_FRAME, buf + k * ls.size);
    }
    libspeex_free(&ls);

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * ls.size, fp);
    fclose(fp);
    free(buf);
}

void save_dec_speex(const char* str, uint8_t* pbuf, size_t len) {
    libspeex_t ls;
    libspeex_init(&ls, SPEEX_TYPE_DECODER, sx_quality);

    int32_t num = len / ls.size;
    uint8_t* buf = (uint8_t*)malloc(num * SPEEX_FRAME);

    for (int32_t k = 0; k < num; k ++) {
        libspeex_decode(&ls, pbuf + k * ls.size, buf + k * SPEEX_FRAME);
    }
    libspeex_free(&ls);

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * SPEEX_FRAME, fp);
    fclose(fp);
    free(buf);
}

void save_enc_gsm(const char* str, uint8_t* pbuf, size_t len) {
    libgsm_t lg;
    libgsm_init(&lg, true);

    int32_t num = len / GSM_VOICE_SIZE;
    uint8_t* buf = (uint8_t*)malloc(num * GSM_FRAME_SIZE);
    for (int32_t k = 0; k < num; k ++) {
        libgsm_encode(&lg, (gsm_signal*)(pbuf + k * GSM_VOICE_SIZE), (gsm_byte*)(buf + k * GSM_FRAME_SIZE));
    }

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * GSM_FRAME_SIZE, fp);
    fclose(fp);
    free(buf);
}

void save_dec_gsm(const char* str, uint8_t* pbuf, size_t len) {
    libgsm_t lg;
    libgsm_init(&lg, false);

    int32_t num = len / GSM_FRAME_SIZE;
    uint8_t* buf = (uint8_t*)malloc(num * GSM_VOICE_SIZE);

    for (int32_t k = 0; k < num; k ++) {
        if (0 != libgsm_decode(&lg,
                    (gsm_byte*)(pbuf + k * GSM_FRAME_SIZE),
                    (gsm_signal*)(buf + k * GSM_VOICE_SIZE))) {
            printf("libgsm decode failed\n");
            return;
        }
    }

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * GSM_VOICE_SIZE, fp);
    fclose(fp);
    free(buf);
}

// gsm多通道音频编码
// 从交织的音频数据中将每个通道分割出来单独编码，每个通道帧长320个字节，压缩为33个字节
// 比如四通道 320*4 = 1280，也就是每1280个字节为一个单元块
// 1280个字节先解出第一个通道的320个字节，编码为33个字节
// 然后解出第二个通道的320个字节编码为33个字节，直到所有的通道处理完
// 然后继续处理下一个1280个字节的单位块，直到所有的单元编码完成
void save_enc_gsm_multi_channel(const char* str, uint8_t* pbuf, size_t len) {
    libgsm_t lg;
    libgsm_init(&lg, true);

    // 计算单元块个数
    int32_t num = len / (GSM_MULTI_VOICE_SIZE);
    uint8_t* buf = (uint8_t*)malloc(num * GSM_MULTI_FRAME_SIZE);

    uint8_t data[GSM_VOICE_SIZE];
    // 对于每个单元块
    for (int32_t k = 0; k < num; k ++) {
        int32_t block_offset = k * GSM_MULTI_VOICE_SIZE;
        // 一个单元块中的每个通道
        for (int32_t i = 0; i < GSM_CHANNEL; i++) {
            int32_t channel_offset = block_offset + i * GSM_SAMPLE_SIZE;
            // 一个通道的每个采样点
            for (int32_t j = 0; j < GSM_VOICE_SIZE/GSM_SAMPLE_SIZE; j++) {
                int32_t sample_offset = channel_offset + j * GSM_MULTI_SAMPLE_SIZE;
                // 一个采样点的每个字节
                for (int32_t z = 0; z < GSM_SAMPLE_SIZE; z++) {
                    data[GSM_SAMPLE_SIZE*j + z] = pbuf[sample_offset + z];
                }
            }

            // 单元块中某个通道的编码
            libgsm_encode(&lg, (gsm_signal*)data, (gsm_byte*)(buf + 
                            k * GSM_MULTI_FRAME_SIZE + i*GSM_FRAME_SIZE));
        }
    }

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * GSM_MULTI_FRAME_SIZE, fp);
    fclose(fp);
    free(buf);
}

// gsm多通道音频解码
// 根据 @save_enc_gsm_multi_channel 编码过程解码
// 每个通道的编码帧解码后进行交织还原成原始音频，单通道帧33字节，解码为320字节
// 如果为四通道 33*4 = 132，则每132个字节为一个解码单元块
// 取一个单元块数据，每33个字节为一帧顺序解码，最终解码为 320*4 = 1280个字节
// 将解码后的1280个字节交织还原为多声道音频
void save_dec_gsm_multi_channel(const char* str, uint8_t* pbuf, size_t len) {
    libgsm_t lg;
    libgsm_init(&lg, false);

    int32_t num = len / (GSM_MULTI_FRAME_SIZE);
    uint8_t* buf = (uint8_t*)malloc(num * GSM_MULTI_VOICE_SIZE);

    uint8_t data[GSM_MULTI_VOICE_SIZE];
    // 对每一个单元块
    for (int32_t k = 0; k < num; k ++) {
        // 一个单元块的每个通道进行解码
        for (int32_t i = 0; i < GSM_CHANNEL; i++) {
            if (0 != libgsm_decode(&lg, 
                (gsm_byte*)(pbuf + k * GSM_MULTI_FRAME_SIZE + i * GSM_FRAME_SIZE), 
                (gsm_signal*)(data + i * GSM_VOICE_SIZE))) {
                printf("libgsm decode failed\n");
                return;
            }
        }

        // 一个单元块解码数据交织
        int32_t block_offset = k * GSM_MULTI_VOICE_SIZE;
        // 一个单元的每个通道
        for (int32_t i = 0; i < GSM_CHANNEL; i++) {
            int32_t channel_offset = block_offset + i * GSM_SAMPLE_SIZE;
            // 一个通道的每个采样点
            for (int32_t j = 0; j < GSM_VOICE_SIZE/GSM_SAMPLE_SIZE; j++) {
                int32_t sample_offset = channel_offset + j * GSM_MULTI_SAMPLE_SIZE;
                for (int32_t z = 0; z < GSM_SAMPLE_SIZE; z++) {
                    buf[sample_offset + z] = data[i * GSM_VOICE_SIZE + j * GSM_SAMPLE_SIZE + z];
                }
            }
        }
    }

    FILE* fp = fopen(str, "wb");
    fwrite(buf, 1, num * GSM_MULTI_VOICE_SIZE, fp);
    fclose(fp);
    free(buf);
}

void enc_speex_cpu_test(uint8_t* pbuf, size_t len) {
    libspeex_t ls;
    libspeex_init(&ls, SPEEX_TYPE_ENCODER, sx_quality);

    int32_t num = len / SPEEX_FRAME;
    uint8_t* buf = (uint8_t*)malloc(num * ls.size);
    int32_t loop_count = 0;

    for (int32_t k = 0; k < num; k ++) {
        libspeex_encode(&ls, pbuf + k * SPEEX_FRAME, buf + k * ls.size);

        if (k == (num - 1)) {
            printf("file encode complete, restart from head, loop count %d\n", ++loop_count);
            k = -1;
        }

        usleep(10000);
    }
    libspeex_free(&ls);
}

void enc_gsm_cpu_test(uint8_t* pbuf, size_t len) {
    libgsm_t lg;
    libgsm_init(&lg, true);

    int32_t num = len / GSM_VOICE_SIZE;
    uint8_t* buf = (uint8_t*)malloc(num * GSM_FRAME_SIZE);
    int32_t loop_count = 0;

    for (int32_t k = 0; k < num; k ++) {
        libgsm_encode(&lg, (gsm_signal*)(pbuf + k * GSM_VOICE_SIZE), (gsm_byte*)(buf + k * GSM_FRAME_SIZE));

        if (k == (num - 1)) {
            printf("file encode complete, restart from head, loop count %d\n", ++loop_count);
            k = -1;
        }

        usleep(10000);
    }
}

int main(int argc, const char *argv[]) {
    uint8_t* pbuf = NULL;
    size_t len;

#if 1
    if (argc == 1 || read_data(argv[1], &pbuf, &len)) {
        printf("Invalid file: %d, %s, %s\n", argc, argv[0], argv[1]);
        return 0;
    }
#else
    read_data("data.raw", &pbuf, &len);
#endif

    // save_enc_speex("speex-enc.raw", pbuf, len);
    // save_dec_speex("speex-dec.raw", pbuf, len);
    // save_enc_gsm("gsm-enc.raw", pbuf, len);
    // save_dec_gsm("gsm-dec.raw", pbuf, len);
    // save_enc_gsm_multi_channel("gsm-enc-multi.raw", pbuf, len);
    save_dec_gsm_multi_channel("gsm-dec-multi.raw", pbuf, len);

    free(pbuf);
    return 0;
}

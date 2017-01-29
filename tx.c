#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define FREQ0 1200.0f
#define FREQ1 1800.0f
#define N 256
#define DELTA0 (FREQ0/SAMPLE_RATE)
#define DELTA1 (FREQ1/SAMPLE_RATE)
#define BAUD 30
#define SPB (160 * 600 / BAUD) // samples per bit

#ifndef M_PI
#define M_PI 3.14159265
#endif

float delta = DELTA0;
int done = 0;
int count = 0;
int txi = 0;
const int tx_data[] = {0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1};
const int len = 12;

typedef struct {
    float phase;
} Context;

static int tx_cb(const void *ib, void *ob,
                unsigned long fpb,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags flags,
                void *ctx) {

    Context *data = (Context*) ctx;
    float *out = (float*) ob;

    for (int i = 0; i<fpb; i++) {
        *out++ = 0.5f*(sinf(2*M_PI*data->phase));
        data->phase += delta;

        count++;

        if (count >= SPB) {
            if (txi >= len) {
                txi = 0;
            }
            delta = (tx_data[txi++] ? DELTA1 : DELTA0);
            count = 0;
        }

        if (data->phase >= 1.0f) {
            data->phase -= 1.0f;
        }
    }
    return 0;
}

int main() {
    PaError err;


    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ) );
        printf("fail to initialize\n");
        return 1;
    }

    Context data;
    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream,
                               0,
                               1, // mono output
                               paFloat32,  /* 32 bit floating point output */
                               SAMPLE_RATE,
                               N,
                               tx_cb,
                               &data);

    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ) );
        printf("failed to open stream\n");
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
        ;

    while (!done);

    err = Pa_StopStream(stream);
    if (err != paNoError)
        ;

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        ;

    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

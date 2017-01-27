#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define NUM_SECONDS 10
#define FREQ 440.0f
#define N 256
#define DELTA (FREQ/SAMPLE_RATE)

float delta = DELTA;

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
        *out++ = sinf(2*M_PI*data->phase);
        data->phase += delta;

        if( data->phase >= 1.0f ) data->phase = 0.0f;
    }
    return 0;
}

int main() {
    PaError err;

    const int tx_data[] = {0, 1, 0, 1, 0, 0, 1, 1};

    err = Pa_Initialize();
    if (err != paNoError) {
        printf("fail\n");
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
        printf("failed to open stream\n");
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
        ;

    for (int i = 0; i<sizeof(tx_data)/sizeof(tx_data[0]); i++) {
        delta = (tx_data[i] ? 2 : 1) * DELTA;
        Pa_Sleep(5000);
    }

    err = Pa_StopStream(stream);
    if (err != paNoError)
        ;

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        ;

    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText( err ) );
    }
}
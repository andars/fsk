#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define NUM_SECONDS 10
#define FREQ 440.0f
#define DELTA (FREQ/SAMPLE_RATE)
#define N 1024

float delta = DELTA;
float coeff;

static int rx_cb(const void *ib, void *ob, unsigned long fpb,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags,
                 void *ctx) {
    float *in = (float *) ib;
    float s0, s1 = 0, s2 = 0;

    for (int i = 0; i<fpb; i++) {
        s0 = coeff*s1 - s2 + *in++;
        s2 = s1;
        s1 = s0;
    }

    float mag = s1*s1 + s2*s2 - coeff*s1*s2;
    printf("%f\n", mag);
}

int main() {
    int k = (int)(0.5 + N*FREQ/SAMPLE_RATE); // dft freq bin
    float omega = 2*M_PI/N*k;
    float cs = cosf(omega);
    coeff = 2*cs;

    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("failed to initialize portaudio\n");
    }

    PaStream *rx_stream;
    err = Pa_OpenDefaultStream(&rx_stream,
                               1, // mono input
                               0,
                               paFloat32,  /* 32 bit floating point input*/
                               SAMPLE_RATE,
                               N,        /* frames per buffer */
                               rx_cb, /* this is your callback function */
                               NULL);

    err = Pa_StartStream(rx_stream);
    if(err != paNoError)
        ;

    Pa_Sleep(5000);

    err = Pa_StopStream(rx_stream);
    if(err != paNoError)
        ;

    err = Pa_CloseStream(rx_stream);
    if(err != paNoError)
        ;

    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    }
}

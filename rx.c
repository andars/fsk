#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define NUM_SECONDS 10
#define FREQ 1200.0f
#define DELTA (FREQ/SAMPLE_RATE)
#define N 2048

float delta = DELTA;
float coeff1;
float coeff2;

static int rx_cb(const void *ib, void *ob, unsigned long fpb,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags,
                 void *ctx) {
    float *in = (float *) ib;
    float s0, s1 = 0, s2 = 0;

    for (int i = 0; i<fpb; i++) {
        s0 = coeff1*s1 - s2 + *in++;
        s2 = s1;
        s1 = s0;
    }
    float mag1 = s1*s1 + s2*s2 - coeff1*s1*s2;

    in = (float *) ib;
    s1 = 0; s2 = 0;

    for (int i = 0; i<fpb; i++) {
        s0 = coeff2*s1 - s2 + *in++;
        s2 = s1;
        s1 = s0;
    }
    float mag2 = s1*s1 + s2*s2 - coeff2*s1*s2;

    //printf("%f       %f\n", mag1, mag2);
    printf("%f\n", mag1);

    if (mag1 > 0.5) {
        //printf("1\n");
    } else {
        //printf("0\n");
    }
    return 0;
}

float goertzel_coeff(int freq) {
    int k = (int)(0.5 + N*freq/SAMPLE_RATE); // dft freq bin
    float omega = 2*M_PI/N*k;
    float cs = cosf(omega);
    return 2*cs;
}

int main() {
    coeff1 = goertzel_coeff(FREQ);
    coeff2 = goertzel_coeff(FREQ*1.5f);

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

    int d;
    scanf("%c", &d);

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

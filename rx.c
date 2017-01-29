#include <portaudio.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 96000
#define FREQ0 1200.0f
#define FREQ1 1800.0f
#define N 1024

/* For N = 256, f0 = 1500 and f1 = 2250? */

float coeff0;
float coeff1;

float y;
float y_prev = 0;

static int rx_cb(const void *ib, void *ob, unsigned long fpb,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags,
                 void *ctx) {
    float *in = (float *) ib;
    float s0, s1 = 0, s2 = 0;

    for (int i = 0; i<fpb; i++) {
        s0 = coeff0*s1 - s2 + *in++;
        s2 = s1;
        s1 = s0;
    }
    float mag0 = s1*s1 + s2*s2 - coeff0*s1*s2;
    mag0 /= N;

    in = (float *) ib;
    s1 = 0; s2 = 0;

    for (int i = 0; i<fpb; i++) {
        s0 = coeff1*s1 - s2 + *in++;
        s2 = s1;
        s1 = s0;
    }
    float mag1 = s1*s1 + s2*s2 - coeff1*s1*s2;
    mag1 /= N;

    // lpf mag1
    y = y_prev + 0.35 * (mag0 - y_prev);
    y_prev = y;

    printf("%f,%f,", mag0, mag1);
    if (mag1 > mag0 && mag0 + mag1 > 0.2) {
        printf("1\n");
    } else {
        printf("0\n");
    }
    return 0;
}

float goertzel_coeff(float freq) {
    int k = (int)(0.5 + N*freq/SAMPLE_RATE); // dft freq bin
    fprintf(stderr, "bin size: %f, index: %d\n", SAMPLE_RATE/(float) N, k);
    float omega = 2*M_PI/N*k;
    float cs = cosf(omega);
    return 2*cs;
}

int main() {
    coeff0 = goertzel_coeff(FREQ0);
    coeff1 = goertzel_coeff(FREQ1);

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

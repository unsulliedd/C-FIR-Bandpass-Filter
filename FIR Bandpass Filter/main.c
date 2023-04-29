#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979323846

typedef struct WAVHeader {
    char     riff[4];           // "RIFF"
    int32_t  chunk_size;        // File size
    char     wave[4];           // "WAVE"
    char     fmt[4];            // "fmt "
    int32_t  subchunk1_size;    // Subchunk size
    int16_t  audio_fmt;         // Audio format
    int16_t  num_channels;      // Number of channels
    int32_t  sample_rate;       // Sampling rate
    int32_t  byte_rate;         // Byte rate
    int16_t  block_align;       // Block alignment
    int16_t  bits_per_sample;   // Bits per sample
    char     data[4];           // "data"
    int32_t  subchunk2_size;    // Data size
} WAVHeader;

void fir_bandpass_blackman(double* window, int numtaps, double fs, double min_f, double max_f, double* h)
{
    double wc1 = 2.0 * PI * min_f / fs; // Normalized cutoff frequency 1
    double wc2 = 2.0 * PI * max_f / fs; // Normalized cutoff frequency 2
    double M = numtaps - 1; // Filter order

    // Calculate the Blackman window
    for (int n = 0; n < numtaps; n++) {
        double a0 = 0.42;
        double a1 = 0.5;
        double a2 = 0.08;
        double wn = a0 - (a1 * cos((2.0 * PI * n) / M)) + (a2 * cos((4.0 * PI * n) / M));   //Blackman window fuction
        window[n] = wn;
    }

    // Calculate the FIR filter coefficients
    for (int n = 0; n < numtaps; n++) {
        if (n == (numtaps / 2)) {
            h[n] = (wc2 - wc1) / PI;
        }
        else {
            h[n] = (sin(wc2 * (n - M / 2.0)) - sin(wc1 * (n - M / 2.0))) / (PI * (n - M / 2.0));
        }
        h[n] *= window[n];  // Multiplication of coefficient array with Blackman window
    }
}

void apply_fir_bandpass_filter(short* buffer, short* filtered_buffer, int num_samples, int numtaps, double fs, double min_f, double max_f, double* h)
{
    FILE* filtered_out = fopen("filtered_output.txt", "w");
    int f_units_per_star = 50;

    for (int i = 0; i < num_samples; i++)
    {
        filtered_buffer[i] = 0;  // Set the output buffer to zero before filtering
    }

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < numtaps; j++)
        {
            if (i >= j)
            {
                filtered_buffer[i] += h[j] * buffer[i - j];
            }
        }

        for (int k = 0; k < abs(filtered_buffer[i]) / f_units_per_star; k++)
        {
            fprintf(filtered_out, "*");
        }
        fprintf(filtered_out, "\n");
    }
    printf("\nThe graph of the filtered signal has been successfully generated in the \"filtered_out.txt\" file.\n");
    fclose(filtered_out);
}

int main() {
    FILE* wav_input = fopen("test.wav", "rb");  // input file
    if (wav_input == NULL)
    {
        printf("Failed to open the file!");
        fclose(wav_input);
        return 1;
    }
    else
    {
        printf("Generating signal graph of the file as \"Wav_Out.txt\"\n\n");
    }

    WAVHeader header;
    fread(&header, sizeof(header), 1, wav_input);   // Reading the WAV file header
    fseek(wav_input, sizeof(WAVHeader), SEEK_SET);  // Moving the file pointer to the data section

    int num_samples = (header.chunk_size - sizeof(WAVHeader)) / sizeof(short);
    short* buffer = malloc(num_samples * sizeof(short));
    if (buffer == NULL)
    {
        free(buffer);
    }
    short max_value = SHRT_MIN;
    short min_value = SHRT_MAX;
    int i;
    size_t num_samples_read = 0;

    FILE* output = fopen("Wav_Out.txt", "w");
    if (output == NULL)
    {
        printf("Failed to open the file!");
        return 1;
    }
    else
    {
        int units_per_star = 50;

        while (num_samples_read < num_samples)
        {
            num_samples_read += fread(buffer + num_samples_read, sizeof(short), num_samples - num_samples_read, wav_input);
            if (num_samples_read <= 0)
            {
                break; // "Error in fread or end of file reached, exiting the loop."

            }
            for (i = 0; i < num_samples_read; i++)
            {
                if (buffer[i] > max_value) {
                    max_value = buffer[i];
                }
                if (buffer[i] < min_value) {
                    min_value = buffer[i];
                }
            }

            short origin = (max_value + abs(min_value)) / 2;

            for (i = 0; i < num_samples_read; i++)
            {
                if (buffer[i] >= 0) {
                    for (int j = 0; j < origin / units_per_star; j++) {
                        fprintf(output, " ");
                    }
                    for (int j = 0; j < buffer[i] / units_per_star; j++) {
                        fprintf(output, "*");
                    }
                }
                else
                {
                    for (int j = 0; j < (buffer[i] + origin) / units_per_star; j++) {
                        fprintf(output, " ");
                    }
                    for (int j = 0; j < abs(buffer[i]) / units_per_star; j++) {
                        fprintf(output, "*");
                    }
                }
                fprintf(output, "\n");
            }
        }
        printf("Successfully generated input signal graph to Wav_Out.txt file.\n\n");
        fclose(output);
    }

    double fs = header.sample_rate;
    double min_f, max_f;
    int numtaps;

    printf("Lower cutoff frequency: ");
    if (scanf("%lf", &min_f) != 1) {
        printf("Please enter only numerical values.\n");
        exit(1);
    }

    printf("Upper cutoff frequency.: ");
    if (scanf("%lf", &max_f) != 1) {
        printf("Please enter only numerical values.\n");
        exit(1);
    }

    printf("FIR filter coefficient count: ");
    if (scanf("%d", &numtaps) != 1) {
        printf("Please enter only numerical values.\n");
        exit(1);
    }

    double* window = malloc(numtaps * sizeof(double));                  // Dynamic memory allocation for Blackman window
    double* h = malloc(numtaps * sizeof(double));                       // Dynamic memory allocation for filter coefficients
    fir_bandpass_blackman(window, numtaps, fs, min_f, max_f, h);        
    short* filtered_buffer = malloc(num_samples * sizeof(short));       // Dynamic memory allocation for filtered signal
    apply_fir_bandpass_filter(buffer, filtered_buffer, num_samples, numtaps, fs, min_f, max_f, h);

    free(buffer);
    free(filtered_buffer);
    free(window);
    free(h);
    fclose(wav_input);

    return 0;
}
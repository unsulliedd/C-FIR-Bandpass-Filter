# WAV Signal Filtering with FIR Bandpass

This project demonstrates filtering a WAV audio signal using a Finite Impulse Response (FIR) bandpass filter. The program reads a WAV file, applies a Blackman windowed FIR bandpass filter, and generates a graph of the filtered signal.

## Getting Started

### Prerequisites

- C compiler (e.g., GCC)
- Make sure you have a WAV audio file for testing (e.g., "test.wav").

### Build and Run

  Clone the repository:

   ```bash
   git clone https://github.com/unsulliedd/C-FIR-Bandpass-Filter.git
  ```

- Follow the on-screen instructions to input lower cutoff frequency, upper cutoff frequency, and FIR filter coefficient count.

***Input Parameters:***

    - Lower cutoff frequency
    - Upper cutoff frequency
    - FIR filter coefficient count

- Enter these values when prompted.

***Output:***

    - Signal graph of the input WAV file: `Wav_Out.txt`
    - Signal graph of the filtered output: `filtered_output.txt`

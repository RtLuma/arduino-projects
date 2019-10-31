#include <arduinoFFT.h>
#include <MCP3XXX.h>

#define FFT_SAMPLES    32

MCP3002 adc;
arduinoFFT FFT = arduinoFFT();

double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];

uint8_t fft_payload[FFT_SAMPLES] = {0};

double maxs[2][FFT_SAMPLES >> 1] = {1.0};
double vals[2][FFT_SAMPLES >> 1] = {1.0};

namespace AudioAnalyzer {
    void begin(void) {
        adc.begin();
    }
    
    void computeFFT(void) {
        for (uint8_t C = 0; C < 2; C++) {
            for (int i = 0; i < FFT_SAMPLES; i++)  {
            //      vReal[i] = C ? adc.analogRead(0) : analogRead(0);
            vReal[i] = adc.analogRead(C);
            vImag[i] = 0;
            }
            FFT.Windowing(vReal, FFT_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
            FFT.Compute(vReal, vImag, FFT_SAMPLES, FFT_FORWARD); /* Compute FFT */
            FFT.ComplexToMagnitude(vReal, vImag, FFT_SAMPLES); /* Compute magnitudes */

            double ratios[FFT_SAMPLES >> 1];
            double hardmax = 0.0;

            for (uint16_t i = 0; i < (FFT_SAMPLES >> 1); i++) {
            double magn = vReal[i];

            if (magn > maxs[C][i]) maxs[C][i] = magn;
            maxs[C][i] *= 0.9995;

            magn /= (maxs[C][i]);
        //      magn = pow(magn, 2);
            magn *= magn;

            if (magn > hardmax) hardmax = magn;
            ratios[i] = magn;
            }

            for (uint16_t i = 0; i < (FFT_SAMPLES >> 1); i++) {
            double magn = ratios[i] * hardmax;

            vals[C][i] *= 0.96;

            if (magn > vals[C][i]) vals[C][i] = (vals[C][i] + magn) / 2.0;
            // if (magn > vals[i]) vals[i] = (vals[i] + 3.0 * magn) / 4.0;

            }
        }
        }

    void prepareFFTPayload(void) {
        uint16_t i, pixel = 0;
        for (i = 0; i < (FFT_SAMPLES >> 1); i++, pixel++) {
            uint8_t hue = ((uint16_t(i) << 8) / (FFT_SAMPLES >> 1));
            // leds.setPixelColor(pixel, leds.Color(
            //                     rainbow(hue + 170) * vals[0][i],
            //                     rainbow(hue + 85)  * vals[0][i],
            //                     rainbow(hue)       * vals[0][i]
            //                 ));
            fft_payload[pixel] = 255 * vals[0][i];
        }

        for (i = (FFT_SAMPLES >> 1) - 1; i > 0; i--, pixel++) {
            uint8_t hue = ((uint16_t(i - 1) << 8) / (FFT_SAMPLES >> 1));
            // leds.setPixelColor(pixel, leds.Color(
            //                     rainbow(hue + 170) * vals[1][i],
            //                     rainbow(hue + 85)  * vals[1][i],
            //                     rainbow(hue)       * vals[1][i]
            //                 ));
            fft_payload[pixel] = 255 * vals[1][i];
        }
        // leds.show();
        }
}
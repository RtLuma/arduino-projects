#include <chrono>
#include <iostream>

#define TESTS 9999
#define RUNS 20

#define PIXELS 600
#define BANDS 7
#define SECTION 43

int main() {

    long long avg1=0, avg2=0;

    for (uint8_t run=0; run < RUNS; run++) {

        uint8_t hue = 230;
        uint8_t colors[BANDS][3]= {{127}};
        uint8_t outs = 127;

        auto start = std::chrono::high_resolution_clock::now();
        /////////////////
        /////////////////
        /////////////////
        for (int t=0; t < TESTS; t++) {
            int8_t band;
            uint8_t nextband, dist;

            for (band = 0; band < BANDS; band++) {
                nextband = band + 1;
                dist = SECTION;
                for (uint8_t p = 0; p < SECTION; p++) {
                    dist--;
                    auto pixelA = colors[band];
                    auto pixelB = colors[nextband];
                    outs = ((pixelA[0]*dist) + (pixelB[0]*p)) / SECTION;
                    outs = ((pixelA[1]*dist) + (pixelB[1]*p)) / SECTION;
                    outs = ((pixelA[2]*dist) + (pixelB[2]*p)) / SECTION;
                }
            }

            for (band = BANDS - 1; band > 0; band--) {
                nextband = band - 1;
                dist = SECTION;
                for (uint8_t p = 0; p < SECTION; p++) {
                    dist--;
                    auto pixelA = colors[band];
                    auto pixelB = colors[nextband];
                    outs = ((pixelA[0]*dist) + (pixelB[0]*p)) / SECTION;
                    outs = ((pixelA[1]*dist) + (pixelB[1]*p)) / SECTION;
                    outs = ((pixelA[2]*dist) + (pixelB[2]*p)) / SECTION;
                }
            }
        }
        /////////////////
        /////////////////
        /////////////////

        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        auto microseconds1 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        printf("top: %d\t", microseconds1);

        avg1 += microseconds1;


        start = std::chrono::high_resolution_clock::now();
        /////////////////
        /////////////////
        /////////////////
        for (int t=0; t < TESTS; t++) {
            int8_t band;
            uint8_t nextband, dist;

            for (band = 0; band < BANDS; band++) {
                nextband = band + 1;
                dist = SECTION;
                for (uint8_t p = 0; p < SECTION; p++) {
                    dist--;
                    outs = (colors[band][0] * dist + colors[nextband][0] * p) / SECTION;
                    outs = (colors[band][1] * dist + colors[nextband][1] * p) / SECTION;
                    outs = (colors[band][2] * dist + colors[nextband][2] * p) / SECTION;
                }
            }

            for (band = BANDS - 1; band > 0; band--) {
                nextband = band - 1;
                dist = SECTION;
                for (uint8_t p = 0; p < SECTION; p++) {
                    dist--;
                    outs = (colors[band][0] * dist + colors[nextband][0] * p) / SECTION;
                    outs = (colors[band][1] * dist + colors[nextband][1] * p) / SECTION;
                    outs = (colors[band][2] * dist + colors[nextband][2] * p) / SECTION;
                }
            }
        }
        /////////////////
        /////////////////
        /////////////////
        elapsed = std::chrono::high_resolution_clock::now() - start;

        auto microseconds2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        printf("bottom: %d\t", microseconds2);
        printf("%d\n", microseconds2 - microseconds1);

        avg2 += microseconds2;

    }

    avg1 /= RUNS;
    avg2 /= RUNS;

    bool slow2 = avg2 > avg1;

    printf("%d was faster by %d", slow2 ? 1 : 2, slow2 ? avg2 - avg1 : avg1 - avg2);

}
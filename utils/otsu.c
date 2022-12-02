#include "otsu.h"

int
otsu_treshold(long int len, Uint32* pixels, int override_treshold)
{
    int treshold = 0, var_max = 0, pbg = 0, pfg = 0, sumbg = 0, sumfg = 0;
    float wbg = 0, wfg = 0, meanbg = 0, meanfg = 0, sumvarbg = 0, sumvarfg = 0;
    float varbg = 0, varfg = 0, var = 0, vars[256];
    Uint8 val = 0;
    unsigned histogram[256];

    for (size_t i = 0; i <= 255; i++)
        histogram[i] = 0;

    if (override_treshold != 0) {
        treshold = override_treshold;
    } else {
        for (int i = 0; i < len; i++) {
            int pixel = pixels[i];
            val = pixel >> 16 & 0xff;
            histogram[val]++;
        }
        for (size_t i = 0; i <= 255; i++) {
            for (size_t j = 0; j <= 255; j++) {
                if (j < i) {
                    pbg += histogram[j];
                    if (histogram[j] != 0) sumbg += j * histogram[j];
                } else {
                    pfg += histogram[j];
                    if (histogram[j] != 0) sumfg += j * histogram[j];
                }
            }
            if (pbg == 0 || pfg == 0) {
                vars[i] = 0;
                continue;
            }
            wbg = (float) pbg / (float) len;
            wfg = (float) pfg / (float) len;
            meanbg = (float) sumbg / (float) pbg;
            meanfg = (float) sumfg / (float) pfg;
            for (size_t j = 0; j <= 255; j++) {
                if (j < i) {
                    if (histogram[j] != 0)
                        sumvarbg += histogram[j] * (float) (j - meanbg) *
                          (float) (j - meanbg);
                } else {
                    if (histogram[j] != 0)
                        sumvarfg += histogram[j] * (float) (j - meanfg) *
                          (float) (j - meanfg);
                }
            }
            varbg = (float) sumvarbg / (float) pbg;
            varfg = (float) sumvarfg / (float) pfg;
            var = wbg * (float) varbg + wfg * (float) varfg;
            vars[i] = var;
            pbg = 0, pfg = 0, sumbg = 0, sumfg = 0;
            sumvarbg = 0, sumvarfg = 0;
        }
        var_max = vars[22];
        for (size_t i = 1; i <= 254; i++) {
            if (vars[i] < var_max && vars[i] != 0) {
                treshold = i;
                var_max = vars[i];
            }
        }
    }
    printf("%d\n",treshold);
    return treshold;
}


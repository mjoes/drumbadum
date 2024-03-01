#include "envelopes.h"

const uint16_t log_env[] = {
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535,
    65535, 65534, 65534, 65534,
    65533, 65532, 65531, 65529,
    65527, 65523, 65518, 65511,
    65501, 65487, 65467, 65440,
    65401, 65347, 65272, 65168,
    65022, 64819, 64538, 64149,
    63610, 62865, 61836, 60418,
    58465, 55780, 52093, 47036,
    40108, 30629, 17677, 0, 
    0,
};

const uint16_t exp_env[] = {
    65535, 62568, 59736, 57032,
    54450, 51986, 49632, 47386,
    45241, 43193, 41238, 39371,
    37589, 35887, 34263, 32712,
    31231, 29817, 28467, 27179,
    25948, 24774, 23652, 22582,
    21560, 20584, 19652, 18762,
    17913, 17102, 16328, 15589,
    14883, 14209, 13566, 12952,
    12366, 11806, 11272, 10761,
    10274, 9809, 9365, 8941,
    8536, 8150, 7781, 7429,
    7093, 6772, 6465, 6172,
    5893, 5626, 5372, 5128,
    4896, 4675, 4463, 4261,
    4068, 3884, 3708, 3540,
    3380, 3227, 3081, 2941,
    2808, 2681, 2560, 2444,
    2333, 2228, 2127, 2031,
    1939, 1851, 1767, 1687,
    1611, 1538, 1468, 1402,
    1338, 1278, 1220, 1165,
    1112, 1062, 1014, 968,
    924, 882, 842, 804,
    768, 733, 700, 668,
    638, 609, 581, 555,
    530, 506, 483, 461,
    440, 420, 401, 383,
    366, 349, 333, 318,
    304, 290, 277, 264,
    253, 241, 230, 220,
    210, 200, 191, 183,
    174, 166, 159, 152,
    145, 138, 132, 126,
    120, 115, 110, 105,
    100, 95, 91, 87,
    83, 79, 76, 72,
    69, 66, 63, 60,
    57, 55, 52, 50,
    48, 45, 43, 41,
    40, 38, 36, 34,
    33, 31, 30, 29,
    27, 26, 25, 24,
    23, 22, 21, 20,
    19, 18, 17, 16,
    16, 15, 14, 14,
    13, 12, 12, 11,
    11, 10, 10, 9,
    9, 9, 8, 8,
    7, 7, 7, 7,
    6, 6, 6, 5,
    5, 5, 5, 4,
    4, 4, 4, 4,
    4, 3, 3, 3,
    3, 3, 3, 3,
    2, 2, 2, 2,
    2, 2, 2, 2,
    2, 2, 2, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 0,
    0
};
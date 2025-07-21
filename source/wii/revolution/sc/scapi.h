#pragma once

extern "C" {

enum SCAspectRatio {
    SC_ASPECT_STD,
    SC_ASPECT_WIDE,
};

enum SCEuRgb60Mode {
    SC_EURGB_50_HZ,
    SC_EURGB_60_HZ,
};

enum SCLanguage {
    SC_LANG_JP,
    SC_LANG_EN,
    SC_LANG_DE,
    SC_LANG_FR,
    SC_LANG_SP,
    SC_LANG_IT,
    SC_LANG_NL,
    SC_LANG_ZH_S,
    SC_LANG_ZH_T,
    SC_LANG_KR,
};

enum SCSoundMode {
    SC_SND_MONO,
    SC_SND_STEREO,
    SC_SND_SURROUND,
};

enum SCSensorBarPos {
    SC_SENSOR_BAR_BOTTOM,
    SC_SENSOR_BAR_TOP,
};

} // extern "C"
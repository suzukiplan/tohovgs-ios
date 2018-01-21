/* Copyright 2012, Suzuki Plan.
 *----------------------------------------------------------------------------
 * [呼称] VGS(Vide Game Sound) Version 1.00
 * [概要] 独自ソフトウェア・シンセサイザー VG-Sound
 * [備考] カーネル種別に依存しない処理を実装する(Windows/Android共用)
 *        ただし、C標準関数は全てのカーネルで使えるものとする.
 *----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include "vge.h"
#include "vgeint.h"
#include "vgsdec.h"

int _bstop;
static inline void setNote(unsigned char cn, unsigned char t, unsigned char n);
inline int getNextNote();

/*
 *----------------------------------------------------------------------------
 * サウンド情報のバッファリング
 *----------------------------------------------------------------------------
 */
void vgsbuf(char* buf, size_t size)
{
    static int an;
    int i, j;
    int wav;
    int cs;
    short* bp;

    an = 1 - an;

    memset(buf, 0, size);
    if (_pause || vge_getmute()) {
        return;
    }

    /* 効果音のバッファリング */
    for (i = 0; i < MAXSLOT; i++) {
        if (_eff[i].flag) {
            if (1 < _eff[i].flag) {
                eff_pos(&_eff[i], 0);
                eff_flag(&_eff[i], 1);
            }
            /* コピーサイズの計算 */
            cs = _eff[i].size - _eff[i].pos;
            if (size < (size_t)cs) {
                cs = (int)size;
            }
            /* バッファリング */
            for (j = 0; j < cs; j += 2) {
                bp = (short*)(&buf[j]);
                wav = *bp;
                wav += *((short*)&(_eff[i].dat[_eff[i].pos + j]));
                if (32767 < wav)
                    wav = 32767;
                else if (wav < -32768)
                    wav = -32768;
                (*bp) = (short)wav;
            }
            /* ポジション・チェンジ */
            eff_pos(&_eff[i], _eff[i].pos + cs);
            if (_eff[i].size <= _eff[i].pos) {
                /* 発音終了 */
                eff_flag(&_eff[i], 0);
            }
        } else {
            eff_pos(&_eff[i], 0);
        }
    }

    if (_bstop) return;
    vgsdec_execute(_psg, buf, size);
}

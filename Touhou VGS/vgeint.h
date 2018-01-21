/* Copyright 2012, Suzuki Plan.
   *----------------------------------------------------------------------------
   * [概要] VGE内部のデータ宣言（vge.cpp/vgeapi.c間で共有する情報群）
   *----------------------------------------------------------------------------
   */

#define MAXSLOT 256

#ifdef __cplusplus
extern "C" {
#endif

/*
 *----------------------------------------------------------------------------
 * VRAM情報:
 * VGEのVRAM(Video RAM)は、32ビット中の8ビットカラー(256色).
 * 昨今のシステムは、32ビット中32ビットカラーが普通が普通だが
 * 独自スプライト処理の性能効率を優先※し、VGE上のVRAMは256色限定とする.
 * (※1ピクセル1バイトになるため)
 * なお、BG領域は状態を保持し、スプライト領域はフレーム毎に内容がクリア.
 * そして、スプライトのパレット0は透過を意味する.
 *----------------------------------------------------------------------------
 */
struct _VRAM {
    unsigned int pal[256];     /* パレット領域 */
    unsigned char bg[0x20000]; /* BG領域 */
    unsigned char sp[0x20000]; /* スプライト領域 */
};

/*
 *----------------------------------------------------------------------------
 * スロット情報
 *----------------------------------------------------------------------------
 */

/* グラフィック */
struct _SLOT {
    int xs;
    int ys;
    unsigned char* dat;
};

/* 効果音 */
struct _EFF {
    unsigned int size;   /* サイズ */
    unsigned short flag; /* 再生フラグ */
    unsigned int pos;    /* 再生位置 */
    unsigned char* dat;  /* PCMデータ */
};

/*
 *----------------------------------------------------------------------------
 * 入力情報
 *----------------------------------------------------------------------------
 */
struct _TOUCH {
    int s;  /* 1:タッチ中, 0:リリース中 */
    int t;  /* タッチしているフレーム数 */
    int dx; /* X方向移動ドット数 */
    int dy; /* Y方向移動ドット数 */
    int cx; /* 現在のX座標 */
    int cy; /* 現在のY座標 */
    int px; /* 直前のX座標 */
    int py; /* 直前のY座標 */
};

/*
 *----------------------------------------------------------------------------
 * グローバル変数のextern宣言
 *----------------------------------------------------------------------------
 */
extern struct _VRAM _vram;
extern struct _SLOT _slot[MAXSLOT];
extern unsigned short ADPAL[256];
extern struct _EFF _eff[MAXSLOT];
extern struct _TOUCH _touch;
extern unsigned char _mute;
extern unsigned char _pause;
extern short* TONE1[85];
extern short* TONE2[85];
extern short* TONE3[85];
extern short* TONE4[85];
extern int isIphone5;
extern void* _psg;
extern int _bstop;

/*
 *----------------------------------------------------------------------------
 * 内部関数(VGE/VGEAPI間)
 *----------------------------------------------------------------------------
 */
void eff_flag(struct _EFF* e, unsigned int f);
void eff_pos(struct _EFF* e, unsigned int f);
char* getbin(const char* name, int* size);
void vgsbuf(char* buf, size_t size);
size_t my_fread(void* buf, size_t size, size_t n, FILE* fp);
size_t my_fwrite(void* buf, size_t size, size_t n, FILE* fp);
void vgsint_setdir(const char* dir);
FILE* vge_fopen(const char* fname, const char* mode);

#ifdef __cplusplus
};
#endif

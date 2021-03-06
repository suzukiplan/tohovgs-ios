#ifndef _INCLUDE_VGEAPI_H
#define _INCLUDE_VGEAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#define vge_deg vge_kkd
#define vge_deg2rad vge_kkd2rad
#define vge_rad2deg vge_rad2kkd

/*
 *----------------------------------------------------------------------------
 * [VGE-API] BG表示関数
 * (1) vge_putBG:  基本関数
 * (2) vge_putBG2: パレット0は描画しない
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 * - sx [I] スロット上のX座標
 * - sy [I] スロット上のY座標
 * - xs [I] 幅
 * - ys [I] 高さ
 * - dx [I] VRAMのX座標
 * - dy [I] VRAMのY座標
 *----------------------------------------------------------------------------
 */
void vge_putBG(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vge_putBG2(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] スプライト表示関数
 * (1) vge_putSP:   基本関数
 * (2) vge_putSPH:  半分(1/2)のサイズで表示する
 * (3) vge_putSPM:  マスク(単色)で表示する
 * (4) vge_putSPMH: マスク(単色) + 半分(1/2)のサイズで表示する
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 * - sx [I] スロット上のX座標
 * - sy [I] スロット上のY座標
 * - xs [I] 幅
 * - ys [I] 高さ
 * - dx [I] VRAMのX座標
 * - dy [I] VRAMのY座標
 * - c [I] 単色表示時のパレット番号 (SPM/SPMHのみ)
 *----------------------------------------------------------------------------
 */
void vge_putSP(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vge_putSPH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vge_putSPM(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);
void vge_putSPMH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_clear: BGを指定パレット番号でクリアする
 *----------------------------------------------------------------------------
 * 引数:
 * - c [I] パレット番号
 *----------------------------------------------------------------------------
 */
void vge_clear(unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_scroll: BGをスクロールさせる
 *----------------------------------------------------------------------------
 * 引数:
 * - x [I] X方向(正=右スクロール、負=左スクロール)
 * - y [I] Y方向(正=下スクロール、負=上スクロール)
 *----------------------------------------------------------------------------
 * 解説:
 * - スクロール後の領域はパレット0でクリアされる
 *----------------------------------------------------------------------------
 */
void vge_scroll(int x,int y);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] ピクセル描画
 * (1) vge_pixelBG: BG面に描画
 * (2) vge_pixelSP: SP面に描画
 *----------------------------------------------------------------------------
 * 引数:
 * - x [I] X座標
 * - y [I] Y座標
 * - c [I] パレット番号
 *----------------------------------------------------------------------------
 */
void vge_pixelBG(int x,int y,unsigned char c);
void vge_pixelSP(int x,int y,unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] ラインを描画する
 * (1) vge_lineBG: BG面に描画
 * (2) vge_lineSP: SP面に描画
 *----------------------------------------------------------------------------
 * 引数:
 * - fx [I] X座標(基点)
 * - fy [I] Y座標(基点)
 * - tx [I] X座標(終点)
 * - ty [I] Y座標(終点)
 * - c [I] パレット番号
 *----------------------------------------------------------------------------
 */
void vge_lineBG(int fx,int fy,int tx,int ty,unsigned char c);
void vge_lineSP(int fx,int fy,int tx,int ty,unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_circleBG: 円を描画する
 * (1) vge_circleBG: BG面に描画
 * (2) vge_circleSP: SP面に描画
 *----------------------------------------------------------------------------
 * 引数:
 * - x [I] X座標(基点)
 * - y [I] Y座標(基点)
 * - r [I] 半径
 * - c [I] パレット番号
 *----------------------------------------------------------------------------
 */
void vge_circleBG(int x,int y,int r,unsigned char c);
void vge_circleSP(int x,int y,int r,unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] 矩形を描画する
 * (1) vge_boxBG: BG面に描画（塗りつぶさない）
 * (2) vge_boxSP: SP面に描画（塗りつぶさない）
 * (3) vge_boxfBG: BG面に描画（塗りつぶす）
 * (4) vge_boxfSP: SP面に描画（塗りつぶす）
 *----------------------------------------------------------------------------
 * 引数:
 * - fx [I] X座標(基点)
 * - fy [I] Y座標(基点)
 * - tx [I] X座標(終点)
 * - ty [I] Y座標(終点)
 * - c [I] パレット番号
 *----------------------------------------------------------------------------
 */
void vge_boxBG(int fx,int fy,int tx,int ty,unsigned char c);
void vge_boxSP(int fx,int fy,int tx,int ty,unsigned char c);
void vge_boxfBG(int fx,int fy,int tx,int ty,unsigned char c);
void vge_boxfSP(int fx,int fy,int tx,int ty,unsigned char c);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] タッチパネルの状態を取得
 *----------------------------------------------------------------------------
 * 引数:
 * - s [O] タッチパネルを押した状態か否か（非タッチ=0, タッチ中=0以外）
 * - cx [O] 現在のX座標（非タッチ中は不定）
 * - cy [O] 現在のY座標（非タッチ中は不定）
 * - dx [O] X方向の移動距離（非タッチ中は不定）
 * - dy [O] Y方向の移動距離（非タッチ中は不定）
 *----------------------------------------------------------------------------
 */
void vge_touch(int* s,int* cx,int* cy,int* dx,int* dy);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_eff: 効果音を鳴らす
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_eff(unsigned char n);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_effstop: 効果音を止める
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_effstop(unsigned char n);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_effstopA: 全ての効果音を止める
 *----------------------------------------------------------------------------
 * 引数: なし
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_effstopA();

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_setmute: 音を消す / 鳴らすの設定
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] 非0=ミュート、0=発音
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_setmute(unsigned char n);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_setmuteCh: チャネル毎の音を消す / 鳴らすの設定
 *----------------------------------------------------------------------------
 * 引数:
 * - ch [I] チャネル番号(0〜5)
 * - n [I] 非0=ミュート、0=発音
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_setmuteCh(unsigned char ch,unsigned char n);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_getmute: 音を消す / 鳴らすの設定を取得
 *----------------------------------------------------------------------------
 * 引数: なし
 *----------------------------------------------------------------------------
 * 戻り値: 非0=ミュート、0=発音
 *----------------------------------------------------------------------------
 */
unsigned char vge_getmute();

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_bplay: BGMを演奏する
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_bplay(unsigned char n);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_bstop: BGMの演奏を中断する
 *----------------------------------------------------------------------------
 * 引数: なし
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_bstop();

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_bresume: BGMの演奏を中断したところから再開する
 *----------------------------------------------------------------------------
 * 引数: なし
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_bresume();

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_bfade: BGMを指定周波数の間隔でフェードアウト
 *----------------------------------------------------------------------------
 * 引数:
 * - hz [I] マスターボリュームを下げる周期Hz (※22050で1秒周期)
 *          ※マスターボリューム10の曲で22050を指定すれば10秒で消音
 *----------------------------------------------------------------------------
 * 戻り値: なし
 *----------------------------------------------------------------------------
 */
void vge_bfade(unsigned int hz);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_setPause: ポーズ状態の設定・解除
 *----------------------------------------------------------------------------
 * 引数:
 * - p [I] ポーズ状態 (0=ポーズ解除する, 0以外=ポーズする)
 *----------------------------------------------------------------------------
 */
void vge_setPause(unsigned char p);

/*
 *----------------------------------------------------------------------------
 * [VGE-API] vge_getdata: DSLOTのデータを取得
 *----------------------------------------------------------------------------
 * 引数:
 * - n [I] スロット番号
 * - size [O] データサイズ
 *----------------------------------------------------------------------------
 * 戻り値: 非NULL=データの先頭ポインタ、NULL=指定スロットにデータは無い
 *----------------------------------------------------------------------------
 */
const char* vge_getdata(unsigned char n,unsigned int* size);

/* 高速数学ライブラリ */
int vge_sin(int r);
int vge_cos(int r);
int vge_rad(int x1,int y1,int x2,int y2);
int vge_kkd(int x1,int y1,int x2,int y2);
int vge_kkd2rad(int kkd);
int vge_rad2kkd(int rad);
void vge_rands();
int vge_rand();

#ifdef __cplusplus
};
#endif

#endif

/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id: tUsart.c 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		LPUART 用 簡易SIOドライバ
 */

#include <sil.h>
#include "stm32l5xx_ll_lpuart.h"
#include "tLpuart_tecsgen.h"

/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
usart_getready(CELLCB *p_cellcb)
{
	return (LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1) == 1);
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
usart_putready(CELLCB *p_cellcb)
{
	return (LL_LPUART_IsActiveFlag_TXE(LPUART1) == 1);
}

/*
 *  受信した文字の取出し
 */
Inline char
usart_getchar(CELLCB *p_cellcb)
{
	return(LL_LPUART_ReceiveData8(LPUART1));
}

/*
 *  送信する文字の書込み
 */
Inline void
usart_putchar(CELLCB *p_cellcb, char c)
{
	 LL_LPUART_TransmitData8(LPUART1, c);
}

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	LL_LPUART_InitTypeDef lpuart_initstruct;

	lpuart_initstruct.PrescalerValue      = LL_LPUART_PRESCALER_DIV1;
	lpuart_initstruct.BaudRate            = ATTR_bps;
	lpuart_initstruct.DataWidth           = LL_LPUART_DATAWIDTH_8B;
	lpuart_initstruct.StopBits            = LL_LPUART_STOPBITS_1;
	lpuart_initstruct.Parity              = LL_LPUART_PARITY_NONE;
	lpuart_initstruct.TransferDirection   = LL_LPUART_DIRECTION_TX_RX;
	lpuart_initstruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;

	LL_LPUART_Init(LPUART1, &lpuart_initstruct);

	LL_LPUART_Enable(LPUART1);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(CELLIDX idx)
{
	LL_LPUART_Disable(LPUART1);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
eSIOPort_putChar(CELLIDX idx, char c)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_putready(p_cellcb)){
		usart_putchar(p_cellcb, c);
		return(true);
	}
	return(false);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
eSIOPort_getChar(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_getready(p_cellcb)) {
		return((int_t)(uint8_t) usart_getchar(p_cellcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(CELLIDX idx, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIOSendReady:
		LL_LPUART_EnableIT_TXE_TXFNF(LPUART1);
		break;
	case SIOReceiveReady:
		LL_LPUART_EnableIT_RXNE_RXFNE(LPUART1);
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(CELLIDX idx, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIOSendReady:
		LL_LPUART_DisableIT_TXE_TXFNF(LPUART1);
		break;
	case SIOReceiveReady:
		LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
		break;
	}
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
void
eiISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_getready(p_cellcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readyReceive();
	}
	if (usart_putready(p_cellcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readySend();
	}
}

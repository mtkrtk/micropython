/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_kernel_impl.c 1399 2020-04-12 02:28:01Z ertl-komori $
 */

/*
 *		コア依存モジュール（ARM-M用）
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"

/*
 *  CPUロックフラグ実現のための変数
 */
volatile bool_t		lock_flag;		/* CPUロックフラグの値を保持する変数 */
volatile uint32_t	saved_iipm;		/* 割込み優先度マスクを保存する変数 */

/*
 *  ベクタテーブル(kernel_cfg.c)
 */
extern const FP vector_table[];

/*
 *  割り込みハンドラテーブル(kernel_cfg.c)
 */
extern const void (*exc_tbl[])(void);

/*
 *  システム例外・割込みの（例外番号 4〜15）
 *  割込み優先度設定レジスタへのアクセスのための配列
 */
static const unsigned int nvic_sys_pri_reg[] = {
	0,
	NVIC_SYS_PRI1,
	NVIC_SYS_PRI2,
	NVIC_SYS_PRI3
};

/*
 *  例外と割込みの割込み優先度をセット
 *
 *  excnoはARM-Mで定められている Exception Number を指定．
 */
void
set_exc_int_priority(uint32_t excno, uint32_t iipm){
	uint32_t tmp, reg;

	/*
	 *  割込み優先度設定レジスタの決定
	 */
	if ((EXCNO_MPU <= excno) && (excno <= IRQNO_SYSTICK)) {
		/*
		 * Exception Number 4(Memory Management)から
		 * Exception Number 15(SysTick)までの割込み優先度はシステム優先度
		 * レジスタにより設定．
		 */
		reg = nvic_sys_pri_reg[excno >> 2];
	}
	else if ((TMIN_INTNO < excno) && (excno <= TMAX_INTNO)){
		/*
		 * IRQ割込みなら
		 */
		reg = NVIC_PRI0 + (((excno - (TMIN_INTNO + 1)) >> 2) * 4);
	}
	else {
		return ;
	}
	
	tmp = sil_rew_mem((void *)reg);
	tmp &= ~(0xFF << (8 * (excno & 0x03)));
	tmp |= iipm << (8 * (excno & 0x03));
	sil_wrw_mem((void *)reg, tmp);
}

/*
 *  例外の許可
 *
 *  Memory Management, Bus Fault, Usage Fault は禁止・許可が可能
 */
void
enable_exc(EXCNO excno)
{
	uint32_t tmp;

	switch (excno) {
	  case EXCNO_MPU:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp |= NVIC_SYS_HND_CTRL_MEM;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_BUS:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp |= NVIC_SYS_HND_CTRL_BUS;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_USAGE:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp |= NVIC_SYS_HND_CTRL_USAGE;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_SECURE:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp |= NVIC_SYS_HND_CTRL_SECURE;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	}
}

/*
 *  例外の禁止
 */
void
disable_exc(EXCNO excno)
{
	uint32_t tmp;

	switch (excno) {
	  case EXCNO_MPU:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp &= ~NVIC_SYS_HND_CTRL_MEM;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_BUS:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp &= ~NVIC_SYS_HND_CTRL_BUS;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_USAGE:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp &= ~NVIC_SYS_HND_CTRL_USAGE;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	  case EXCNO_SECURE:
		tmp = sil_rew_mem((void *)NVIC_SYS_HND_CTRL);
		tmp &= ~NVIC_SYS_HND_CTRL_SECURE;
		sil_wrw_mem((void *)NVIC_SYS_HND_CTRL, tmp);
		break;
	}
}


/*
 *  コア依存の初期化
 */
void
core_initialize(void)
{
	/*
	 *  CPUロックフラグ実現のための変数の初期化
	 */
	lock_cpu_dsp();

	/*
	 *  ベクタテーブルを設定
	 */
	sil_wrw_mem((void*)NVIC_VECTTBL, (uint32_t)vector_table);

	/*
	 *  各例外の優先度を設定
	 *  CPUロック状態でも発生するように，BASEPRIレジスタでマスクでき
	 *  ない'0'とする．
	 */
	set_exc_int_priority(EXCNO_MPU, 0);
	set_exc_int_priority(EXCNO_BUS, 0);
	set_exc_int_priority(EXCNO_USAGE, 0);
	set_exc_int_priority(EXCNO_SVCALL, 0);
	set_exc_int_priority(EXCNO_DEBUG, 0);
	set_exc_int_priority(EXCNO_PENDSV, INT_IPM(-1));
#ifdef TOPPERS_ENABLE_TRUSTZONE
	set_exc_int_priority(EXCNO_SECURE, 0);
#endif /* TOPPERS_ENABLE_TRUSTZONE */

	/*
	 *  各例外の有効化
	 */
	enable_exc(EXCNO_MPU);
	enable_exc(EXCNO_BUS);
	enable_exc(EXCNO_USAGE);
#ifdef TOPPERS_ENABLE_TRUSTZONE
	enable_exc(EXCNO_SECURE);
#endif /* TOPPERS_ENABLE_TRUSTZONE */

	/*
	 *  Configuration Control RegisterのSTKALIGNビットを0にする
	 *  スタックは8byteアラインでなく、4byteアライン
	 */
	sil_andw((void *)CCR_BASE, ~CCR_STKALIGN);

#ifdef TOPPERS_SAFEG_M
	/* Non-secureからのDeep sleepを禁止 */
	sil_wrw_mem((uint32_t *)SCB_SCR, SCB_SCR_SLEEPDEEPS);
#endif /* TOPPERS_SAFEG_M */

#ifdef TOPPERS_FPU_ENABLE
	sil_orw((uint32_t *)CPACR, CPACR_FPU_ENABLE);
	sil_wrw_mem((uint32_t *)FPCCR, FPCCR_INIT);
#ifdef TOPPERS_SAFEG_M
	sil_wrw_mem((uint32_t *)NSACR, NSACR_FPU_ENABLE);
#endif /* TOPPERS_SAFEG_M */
#endif /* TOPPERS_FPU_ENABLE */

	/*
	 * スタックの境界値をタスクコンテキストにキャッシュ
	 */
	for (int i = 0; i < tnum_tsk; ++i) {
		tcb_table[i].tskctxb.stk_top = tinib_table[i].tskinictxb.stk_top;
	}

#ifdef TOPPERS_SAFEG_M
	/* Non-secure 割り込みの優先度を半分以下に設定 */
	sil_wrw_mem((uint32_t *)SCB_AIRCR, SCB_AIRCR_DIS_GROUP);
#endif /* TOPPERS_SAFEG_M */
}

/*
 *  コア依存の終了処理
 */
void
core_terminate(void)
{
	extern void    software_term_hook(void);
	void (*volatile fp)(void) = software_term_hook;

	/*
	 *  software_term_hookへのポインタを，一旦volatile指定のあるfpに代
	 *  入してから使うのは，0との比較が最適化で削除されないようにするた
	 *  めである．
	 */
	if (fp != 0) {
		(*fp)();
	}
}

/*
 *  割込み要求ライン属性の設定
 */
void
config_int(INTNO intno, ATR intatr, PRI intpri)
{
	assert(VALID_INTNO_CFGINT(intno));
	assert(TMIN_INTPRI <= intpri && intpri <= TMAX_INTPRI);

	/* 
	 *  一旦割込みを禁止する
	 */    
	(void)disable_int(intno);

	/*
	 *  割込み優先度をセット
	 */
	set_exc_int_priority(intno, INT_IPM(intpri));

	/*
	 *  割込み要求マスク解除(必要な場合)
	 */
	if ((intatr & TA_ENAINT) != 0U) {
		(void)enable_int(intno);
	}    
}

/*
 *  割込みエントリ
 */
void
core_int_entry(void)
{
	/* 割り込み優先度の保存 */
	const uint32_t basepri = get_basepri();
	const uint32_t intnum = get_ipsr();

#ifdef TOPPERS_SUPPORT_OVRHDR
	/* TODO: 一段目の割り込みのみで行うべき */
	set_basepri(IIPM_LOCK);
	ovrtimer_stop();
	set_basepri(IIPM_ENAALL);
#endif /* TOPPERS_SUPPORT_OVRHDR */
#ifdef LOG_INH_ENTER
	log_inh_enter(intnum);
#endif /* LOG_EXC_ENTER */

	/* 割り込みハンドラの呼び出し */
	exc_tbl[intnum]();

#ifdef LOG_INH_LEAVE
	log_inh_leave(intnum);
#endif /* LOG_INH_LEAVE */
#ifdef TOPPERS_SUPPORT_OVRHDR
	/* TODO: ここではなく PendSV からのリターン時に行うべき */
	ovrtimer_start();
#endif /* TOPPERS_SUPPORT_OVRHDR */

	/* 割り込み優先度を復帰し CPU ロック解除状態へ */
	lock_flag = 0;
	set_basepri(basepri);
}

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 *  未登録の例外が発生した場合に呼び出される
 */
void
default_exc_handler(void *p_excinf)
{
	uint32_t basepri = *(((uint32_t*)p_excinf) + P_EXCINF_OFFSET_BASEPRI);
	uint32_t pc      = *(((uint32_t*)p_excinf) + P_EXCINF_OFFSET_PC);
	uint32_t xpsr    = *(((uint32_t*)p_excinf) + P_EXCINF_OFFSET_XPSR);
	uint32_t excno   = get_ipsr() & IPSR_ISR_NUMBER;

	syslog(LOG_EMERG, "\nUnregistered Exception occurs.");
	syslog(LOG_EMERG, "Excno = %08x PC = %08x XPSR = %08x basepri = %08X, p_excinf = %08X",
		   excno, pc, xpsr, basepri, p_excinf);

	target_exit();
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */

#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 *  未登録の割込みが発生した場合に呼び出される
 */
void
default_int_handler(void)
{
	uint32_t intno = get_ipsr() & IPSR_ISR_NUMBER;

	syslog(LOG_EMERG, "\nUnregistered Interrupt occurs.");
	syslog(LOG_EMERG, "Intno = %08x", intno);

	target_exit();
}
#endif /* OMIT_DEFAULT_INT_HANDLER */

typedef void __attribute__((cmse_nonsecure_call)) (*nonsecure_call_t)(void);
void launch_ns(intptr_t exinf)
{
	set_faultmask_ns(1); /* Non-secure の割り込みを禁止 */
	set_control_ns(0); /* 特権かつスタックポインタを MSP に設定 */
	set_msp_ns(*(uint32_t *)exinf); /* スタックポインタの初期値を設定 */
	sil_wrw_mem((uint32_t *)SCB_NS_VTOR, exinf); /* ベクタテーブルオフセットを設定 */
	/*
	 * Non-secure 側で Lazy stack preservation がアクティブであったとする．
	 * このとき Secure 側の割り込みで Non-secure タスクを終了した場合を考える．
	 * FPCCR_NS.LSPACT = 1 が設定され，FPCAR にスタックの先頭アドレスが格納される．
	 * 次回起動時に初めて浮動小数点数命令を実行すると，
	 * 浮動小数点数レジスタがこのスタックに積まれることになる．
	 * しかし，このスタックはすでに無効なものであるから（最良の場合）Fault が発生する．
	 * これを防ぐため，FPCCR_NS.LSPACT をクリアする．
	 */
	sil_wrw_mem((uint32_t *)FPCCR_NS, FPCCR_INIT);
	set_basepri(0);
	nonsecure_call_t entry = (nonsecure_call_t)*(uint32_t *)(exinf + 4);
	entry();
}
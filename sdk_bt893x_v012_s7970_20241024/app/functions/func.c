#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);

#if VBAT_DETECT_EN

void lowpower_warning_do(void)
{
    if (sys_cb.lpwr_warning_times) {        //低电语音提示次数
        if (RLED_LOWBAT_FOLLOW_EN) {
            led_lowbat_follow_warning();
        }

        sys_cb.lowbat_flag = 1;
//            bsp_res_play(TWS_RES_LOW_BATTERY);
        bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
        plugin_lowbat_vol_reduce();         //低电降低音乐音量

        #if 0
        if (RLED_LOWBAT_FOLLOW_EN) {
            while (get_led_sta(1)) {        //等待红灯闪完
                delay_5ms(2);
            }
            led_lowbat_recover();
        }
        #endif

        if (sys_cb.lpwr_warning_times != 0xff) {
            sys_cb.lpwr_warning_times--;
        }
    }
}

void lowpower_switch_to_normal_do(void)
{
    sys_cb.vbat_nor_cnt++;

    if (sys_cb.vbat_nor_cnt > 40) {
        sys_cb.lowbat_flag = 0;
        sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
        plugin_lowbat_vol_recover();    //离开低电, 恢复音乐音量
    }
}

void lowpower_poweroff_do(void)
{
    bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
    sys_cb.pwrdwn_tone_en = 1;
    func_cb.sta = FUNC_PWROFF;     //低电，进入关机或省电模式
}

AT(.text.func.msg)
void lowpower_vbat_process(void)
{
    int lpwr_vbat_sta = is_lowpower_vbat_warning();

    if (lpwr_vbat_sta == 1) {
//        bsp_res_play(TWS_RES_LOW_BATTERY);
        lowpower_poweroff_do();
        return;
    }

    if ((func_cb.mp3_res_play == NULL) || (lpwr_vbat_sta != 2)) {
        if ((sys_cb.lowbat_flag) && (sys_cb.vbat > 3800)) {
            lowpower_switch_to_normal_do();
        }
        return;
    }

    //低电提示音播放
    sys_cb.vbat_nor_cnt = 0;
    if (sys_cb.lpwr_warning_cnt > xcfg_cb.lpwr_warning_period) {
        sys_cb.lpwr_warning_cnt = 0;
        lowpower_warning_do();
    }
}
#endif // VBAT_DETECT_EN

void func_volume_up(void)
{
    if (func_cb.sta == FUNC_BT) {
        if (sys_cb.incall_flag) {
            bsp_bt_call_volume_msg(KU_VOL_UP);
        } else {
#if BT_HID_VOL_CTRL_EN
            if(bsp_bt_hid_vol_change(HID_KEY_VOL_UP)) {
                return;
            }
#endif

            bt_music_vol_up();

#if WARNING_MAX_VOLUME
            if (sys_cb.vol == VOL_MAX) {
                maxvol_tone_play();
            }
#endif
        }
    } else {
        bsp_set_volume(bsp_volume_inc(sys_cb.vol));
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(1);
    }
}

void func_volume_down(void)
{
    if (func_cb.sta == FUNC_BT) {
        if (sys_cb.incall_flag) {
            bsp_bt_call_volume_msg(KU_VOL_DOWN);
        } else {
#if BT_HID_VOL_CTRL_EN
            if(bsp_bt_hid_vol_change(HID_KEY_VOL_DOWN)) {
                return;
            }
#endif

            bt_music_vol_down();

#if WARNING_MIN_VOLUME
            if (sys_cb.vol == 0) {
                minvol_tone_play();
            }
#endif
        }
    } else {
        bsp_set_volume(bsp_volume_dec(sys_cb.vol));
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(0);
    }
}

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
    vusb_reset_clr();

#if VBAT_DETECT_EN
    lowpower_vbat_process();
#endif // VBAT_DETECT_EN

#if QTEST_EN
    if(QTEST_IS_ENABLE()) {
        qtest_process();
    }
#endif

#if BT_BACKSTAGE_EN
    if (func_cb.sta != FUNC_BT) {
        func_bt_warning();
        uint status = bt_get_status();
#if BT_BACKSTAGE_PLAY_DETECT_EN
        if (status >= BT_STA_PLAYING) {
#else
        if (status > BT_STA_PLAYING) {
#endif
            func_cb.sta_break = func_cb.sta;
            func_cb.sta = FUNC_BT;
        }
    }
#endif

#if PWRKEY_2_HW_PWRON
    //PWRKEY模拟硬开关关机处理
    if (sys_cb.pwrdwn_hw_flag) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        sys_cb.pwrdwn_hw_flag = 0;
    }
#endif

#if USER_NTC
    if (sys_cb.ntc_2_pwrdwn_flag) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        sys_cb.ntc_2_pwrdwn_flag = 0;
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_process();
    }
#endif // CHARGE_EN

#if SYS_KARAOK_EN
    karaok_process();
#endif

#if ANC_ALG_EN
    anc_alg_process();
#endif // ANC_ALG_EN

#if ANC_TOOL_EN
    anc_tool_process();
#endif // ANC_TOOL_EN

#if BT_MUSIC_EFFECT_EN
    music_effect_func_process();
#endif // BT_MUSIC_EFFECT_EN

    if(f_bt.bt_is_inited) {
        bt_thread_check_trigger();
        bsp_res_process();
        app_process();
        le_popup_process();
    }

    gsensor_process();

    sys_run_loop();
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {
#if BT_TWS_EN
        case EVT_BT_UPDATE_STA:
            f_bt.disp_update = 1;    //刷新显示
            break;
#endif
        case KL_NEXT_VOL_UP:
        case KH_NEXT_VOL_UP:
        case KL_PREV_VOL_UP:
        case KH_PREV_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
        case KU_VOL_UP_NEXT:
        case KU_VOL_UP_PREV:
        case KU_VOL_UP:
        case KU_VOL_UP_DOWN:
            func_volume_up();
            break;

        case KLU_VOL_UP:
        case KLU_NEXT_VOL_UP:
        case KL_PREV_VOL_DOWN:
        case KH_PREV_VOL_DOWN:
        case KL_NEXT_VOL_DOWN:
        case KH_NEXT_VOL_DOWN:
        case KU_VOL_DOWN_PREV:
        case KU_VOL_DOWN_NEXT:
        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
        case KL_VOL_UP_DOWN:
        case KH_VOL_UP_DOWN:
            func_volume_down();
            break;

        //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_POWER:
        case KLH_MODE_PWR:
        case KLH_PLAY_PWR_USER_DEF:
            if(xcfg_cb.bt_tswi_kpwr_en) {       //按键关机是否主从切换
                if(sys_cb.discon_reason == 0xff) {
                    sys_cb.discon_reason = 0;   //不同步关机
                }
            }
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
            break;
        case KU_MODE:
        case KU_MODE_PWR:
            func_cb.sta = FUNC_NULL;
            break;

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            break;

#if SYS_KARAOK_EN
        case KU_VOICE_RM:
            karaok_voice_rm_switch();
            break;
#if SYS_MAGIC_VOICE_EN || HIFI4_PITCH_SHIFT_EN
        case KL_VOICE_RM:
            magic_voice_switch();
            break;
#endif
#endif

#if ANC_EN
        case KU_ANC:
        case KD_ANC:
        case KL_ANC:
            sys_cb.anc_user_mode++;
            if (sys_cb.anc_user_mode > 2) {
                sys_cb.anc_user_mode = 0;
            }
            bsp_anc_set_mode(sys_cb.anc_user_mode);
            break;
#endif

        case MSG_SYS_500MS:
            break;

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK)) {
                if (dev_udisk_activation_try(0)) {
                    sys_cb.cur_dev = DEV_UDISK;
                    func_cb.sta = FUNC_MUSIC;
                }
            }
            break;
#endif // MUSIC_UDISK_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                func_cb.sta = FUNC_USBDEV;
            }
            break;

        case EVT_UDE_SET_VOL:
            printf("ude set db: %d\n", sys_cb.db_level);
            bsp_change_volume_db(sys_cb.db_level);
            break;
#endif // FUNC_USBDEV_EN

#if LINEIN_DETECT_EN
        case EVT_LINEIN_INSERT:
            if (dev_is_online(DEV_LINEIN)) {
#if LINEIN_2_PWRDOWN_EN
                sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
                func_cb.sta = FUNC_PWROFF;
#else
                func_cb.sta = FUNC_AUX;
#endif // LINEIN_2_PWRDOWN_EN
            }
            break;
#endif // LINEIN_DETECT_EN

        case EVT_A2DP_SET_VOL:
        case EVT_TWS_INIT_VOL:
            if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0) {
                if(!bsp_res_is_vol_busy()) {
                    bsp_change_volume(sys_cb.vol);
                }
            }
            //no break
        case EVT_TWS_SET_VOL:
            if (sys_cb.incall_flag == 0) {
                printf("VOL: %d\n", sys_cb.vol);
                if(msg != EVT_TWS_INIT_VOL) {   //TWS同步音量，不需要显示
                    gui_box_show_vol();
                }
            }
            //no break
        case EVT_DEV_SAVE_VOL:
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
            break;

#if BT_MUSIC_EFFECT_SOFT_VOL_EN
        case EVT_SOFT_VOL_SET:
            bsp_change_volume(sys_cb.vol);
            break;
#endif // BT_MUSIC_EFFECT_SOFT_VOL_EN
#if ABP_EN
        case EVT_ABP_SET:
            if (sys_cb.abp_mode == 0) {
                abp_stop();
            } else {
                abp_start(sys_cb.abp_mode - 1);
            }
            break;
#endif // ABP_EN

        case EVT_BT_SCAN_START:
            if (bt_get_status() < BT_STA_SCANNING) {
                bt_scan_enable();
            }
            break;
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            bsp_eq_parse_cmd();
            break;
#endif
#if ANC_EN
        case EVT_ONLINE_SET_ANC:
            bsp_anc_parse_cmd();
            break;
#endif

#if ENC_DBG_EN
        case EVT_ONLINE_SET_ENC:
            bsp_enc_parse_cmd();
            break;
#endif

#if BT_MUSIC_EFFECT_DBG_EN
        case EVT_ONLINE_SET_EFFECT:
            bsp_effect_parse_cmd();
            break;
#endif

#if ANC_ALG_DBG_EN
        case EVT_ONLINE_SET_ANC_ALG:
            bsp_anc_alg_parse_cmd();
            break;
#endif // ANC_ALG_DBG_EN

#if ANC_TOOL_EN
        case EVT_ONLINE_SET_ANC_TOOL:
            bsp_anc_tool_parse_cmd();
            break;
#endif // ANC_TOOL_EN

#if SYS_KARAOK_EN
        case EVT_ECHO_LEVEL:
//            printf("echo level:%x\n", sys_cb.echo_level);
    #if SYS_ECHO_EN
            bsp_echo_set_level();
    #endif
            break;

        case EVT_MIC_VOL:
//            printf("mic vol:%x\n", sys_cb.mic_vol);
            bsp_karaok_set_mic_volume();
            break;

        case EVT_MUSIC_VOL:
//            printf("music vol:%x\n", sys_cb.music_vol);
            bsp_karaok_set_music_volume();
            break;
#endif
#if LANG_SELECT == LANG_EN_ZH
        case EVT_BT_SET_LANG_ID:
            param_lang_id_write();
            param_sync();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_set_eq_by_num(sys_cb.eq_mode);
            break;
#endif

#if CHARGE_BOX_EN && (UART0_PRINTF_SEL != PRINTF_VUSB)
        //耳机入仓关机
        case EVT_CHARGE_INBOX:
            if(sys_cb.discon_reason == 0xff) {
                sys_cb.discon_reason = 0;   //不同步关机
            }
            sys_cb.pwrdwn_tone_en = 0;
            charge_box_inbox_wakeup_enable();
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

#if QTEST_EN
        case EVT_QTEST_PICKUP_PWROFF:
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

        case EVT_HFP_SET_VOL:
            if(sys_cb.incall_flag & INCALL_FLAG_SCO){
                bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
                sys_cb.cm_times = 0;
                sys_cb.cm_vol_change = 1;
            }
            break;

#if DAC_DNC_EN
        case EVT_DNC_START:
            if (bsp_res_is_playing()) {
                msg_enqueue(EVT_DNC_START);
            } else {
                #if ANC_EN
                if (sys_cb.anc_user_mode == 0)
                #endif
                {
                    dac_dnc_start();
                }
            }
            break;

        case EVT_DNC_STOP:
            if (bsp_res_is_playing()) {
                msg_enqueue(EVT_DNC_STOP);
            } else {
                dac_dnc_stop();
            }
            break;
#endif

#if IODM_TEST_EN
        case EVT_IODM_TEST:
            iodm_reveice_data_deal();
            break;
#endif

#if ASR_EN
        case EVT_ASR_START:
            if (sys_cb.asr_enable) {
                bsp_asr_start();
            }
            break;

        case EVT_ASR_STOP:
            bsp_asr_stop();
            break;
#endif
#if OPUS_ENC_EN
       case EVT_ENCODE_STOP:
            bsp_opus_encode_stop();
            break;
#endif
    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        param_hfp_vol_write();
        param_sys_vol_write();
        cm_sync();
    }

#if SD_SOFT_DETECT_EN
    sd_soft_cmd_detect(120);
#endif
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    if (sys_cb.cm_vol_change) {
        sys_cb.cm_vol_change = 0;
        param_sys_vol_write();
    }
    param_sync();
    gui_box_clear();
    reset_sleep_delay();
    reset_pwroff_delay();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
    bsp_clr_mute_sta();
    sys_cb.voice_evt_brk_en = 1;    //播放提示音时，快速响应事件。
#if SYS_KARAOK_EN
    karaok_voice_rm_disable();
    bsp_karaok_echo_reset_buf(func_cb.sta);
#endif
}

AT(.text.func)
void func_exit(void)
{
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任务
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
#if SYS_MODE_BREAKPOINT_EN
    param_sys_mode_write(func_cb.sta);
#endif // SYS_MODE_BREAKPOINT_EN
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    func_bt_chk_off();
    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_BTHID_EN
        case FUNC_BTHID:
            func_bthid();
            break;
#endif // FUNC_BTHID_EN

#if FUNC_AUX_EN
        case FUNC_AUX:
            func_aux();
            break;
#endif // FUNC_AUX_EN

#if FUNC_USBDEV_EN
        case FUNC_USBDEV:
            func_usbdev();
            break;
#endif

#if FUNC_SPEAKER_EN
        case FUNC_SPEAKER:
            func_speaker();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

#if FUNC_BT_DUT_EN
        case FUNC_BT_DUT:
            func_bt_dut();
            break;
#endif
#if FUNC_BT_FCC_EN
        case FUNC_BT_FCC:
            func_bt_fcc();
            break;
#endif
#if IODM_TEST_EN
        case FUNC_BT_IODM:
            func_bt_iodm();
            break;
#endif

        case FUNC_CHARGE:
            func_charge();
            break;

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        default:
            func_exit();
            break;
        }
    }
}

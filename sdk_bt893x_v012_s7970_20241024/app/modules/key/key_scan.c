#include "include.h"


void knob_init(void);
void knob_process(void);
u16 knob_process2(u16 *key_val);

#if USER_MULTI_PRESS_EN
const u16 tbl_double_key[] = USER_KEY_DOUBLE;
#endif

u8 bsp_tkey_scan(void);

AT(.com_text.key.table)
const key_shake_tbl_t key_shake_table = {
    .scan_cnt = KEY_SCAN_TIMES,
    .up_cnt   = KEY_UP_TIMES,
    .long_cnt = KEY_LONG_TIMES,
    //@lewis
    #if 1
	.lv1_long_cnt = KEY_LV1_LONG_TIMES,
	.lv2_long_cnt = KEY_LV2_LONG_TIMES,
	.hold_cnt = KEY_HOLD_TIMES,
	#else
    .hold_cnt = KEY_LONG_HOLD_TIMES,
    #endif
    //End
};

AT(.text.bsp.key.init)
void key_init(void)
{
    key_var_init();
	//@lewis
	sys_cb.double_delay_cnt = get_double_key_time();
	sys_cb.poweroff_hold_cnt = ((PWROFF_PRESS_TIME - 3) / 3) * 100 + 300;
	//先保存左耳user def按键功能
	sys_cb.user_def_l_ks_sel = xcfg_cb.user_def_ks_sel;
	sys_cb.user_def_l_kl_sel = xcfg_cb.user_def_kl_sel;
	sys_cb.user_def_l_kd_sel = xcfg_cb.user_def_kd_sel;
	sys_cb.user_def_l_kt_sel = xcfg_cb.user_def_kt_sel;
	sys_cb.user_def_l_kfour_sel = xcfg_cb.user_def_kfour_sel;
	sys_cb.user_def_l_kfive_sel = xcfg_cb.user_def_kfive_sel;
	#if BT_TWS_EN
	if(xcfg_cb.bt_tws_en)
	{
		if(xcfg_cb.user_def_lr_en) {
			if(!func_bt_tws_get_channel()) { //如果为TWS右声道
				//本地load入右耳按键功能
				xcfg_cb.user_def_ks_sel = xcfg_cb.user_def_r_ks_sel;
				xcfg_cb.user_def_kl_sel = xcfg_cb.user_def_r_kl_sel;
				xcfg_cb.user_def_kd_sel = xcfg_cb.user_def_r_kd_sel;
				xcfg_cb.user_def_kt_sel = xcfg_cb.user_def_r_kt_sel;
				xcfg_cb.user_def_kfour_sel = xcfg_cb.user_def_r_kfour_sel;
				xcfg_cb.user_def_kfive_sel = xcfg_cb.user_def_r_kfive_sel;
			}
		} else{
		    //右耳按键功能与左耳一样
			xcfg_cb.user_def_r_ks_sel = sys_cb.user_def_l_ks_sel;
			xcfg_cb.user_def_r_kl_sel = sys_cb.user_def_l_kl_sel;
			xcfg_cb.user_def_r_kd_sel = sys_cb.user_def_l_kd_sel;
			xcfg_cb.user_def_r_kt_sel = sys_cb.user_def_l_kt_sel;
			xcfg_cb.user_def_r_kfour_sel = sys_cb.user_def_l_kfour_sel;
			xcfg_cb.user_def_r_kfive_sel = sys_cb.user_def_l_kfive_sel;
		}
	} 
	#endif
	//End

#if USER_IOKEY
    io_key_init();
#endif

#if USER_ADKEY || USER_ADKEY2 || USER_NTC
    adkey_init();
#endif

#if USER_KEY_KNOB_EN
    knob_init();
#endif

    pwrkey_init();

#if VBAT_DETECT_EN
    vbat_init();
#endif

    bsp_saradc_init();

    bsp_tkey_init();
}

#if USER_MULTI_PRESS_EN
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    u8 i;
    if (key_return == NO_KEY) {
        return false;
    }

    //工具配置了哪些按键支持双击功能？
    i = check_key_double_configure(key_return);
    if (i > 0) {
        return (i - 1);
    }

    for (i=0; i<sizeof(tbl_double_key)/2; i++) {
        if (key_return == tbl_double_key[i]) {
            return true;
        }
    }
    return false;
}
#else
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    return false;
}
#endif // USER_KEY_DOUBLE_EN

AT(.text.bsp.key)
u8 get_pwroff_pressed_time(void)
{
    return PWROFF_PRESS_TIME;
}

AT(.text.bsp.key)
u8 get_double_key_time(void)
{
    if(DOUBLE_KEY_TIME > 7) {
        return 60;
    } else {
        return (u8)((u8)(DOUBLE_KEY_TIME + 2) * 20 + 1);
    }
}

AT(.com_text.bsp.key)
bool get_poweron_flag(void)
{
    return sys_cb.poweron_flag;
}

AT(.com_text.bsp.key)
void set_poweron_flag(bool flag)
{
    sys_cb.poweron_flag = flag;
}

//@lewis
typedef enum {
    KEY_STATE_NOTHING,        // 按键无状态
    KEY_STATE_PRESS,          // 按键按下
    KEY_STATE_UP,             // 按键抬起
    KEY_STATE_DELY,           // 按键抬起后多击延迟
    KEY_STATE_LONG,           // 按键长按
    KEY_STATE_LV1_LONG,       // LV1按键长按
    KEY_STATE_LV2_LONG,       // LV2按键长按
} KEY_STATE_E;

typedef struct {
	u16 pwrkey_up;
    u16 key_up;
    u16 key_cnt;
    u16 pre_keyval;
    u16 delay_cnt;
	u16 repeat_cnt;
    u8 press_cnt;
    KEY_STATE_E KEY_STATE;       
} key_cb_t;

key_cb_t key_cb AT(.com_text.bsp.key_cb);

AT(.com_text.bsp.key)
u16 key_deal(u16 key_val)
{
    u16 key_return = NO_KEY;

	if(sys_cb.poweron_flag) //pwrkey按键松开自动清此标志
	{
		if ((key_val & K_PWR_MASK) == K_PWR) {
			key_cb.key_cnt = 0;
			key_cb.press_cnt = 0;
			key_cb.delay_cnt = 0;
			key_cb.repeat_cnt = 0;
			key_cb.pwrkey_up = 0; // pwrkey按键抬起消抖
			key_cb.KEY_STATE = KEY_STATE_NOTHING; //pwrkey开机时，等待pwrkey松开再作按键检测
			return key_return;
		} else{
			if(key_cb.pwrkey_up < 10) {
				key_cb.pwrkey_up += 1;
			} else{
				sys_cb.poweron_flag = 0;
				key_cb.pwrkey_up = 0;
			}
		}
	}
	
    if (key_val != NO_KEY)
        key_cb.pre_keyval = key_val;
	
    switch(key_cb.KEY_STATE) 
	{
        case KEY_STATE_NOTHING:
        case KEY_STATE_UP:
            if (key_val == NO_KEY) {
                key_cb.key_cnt = 0;    // 按下计时
                key_cb.key_up = 0;     // 抬起计时
                key_cb.press_cnt = 0;  // 按下次数
            } else {
                // 按下消抖
                key_cb.key_cnt++;
                if (key_cb.key_cnt >= key_shake_table.scan_cnt) {
					key_cb.key_up = 0;
                    // 输出按下事件
                    key_return = key_cb.pre_keyval | KEY_SHORT;
                    key_cb.KEY_STATE = KEY_STATE_PRESS;
                }
            }
        break;
			
        case KEY_STATE_PRESS:
            if (key_val == NO_KEY) {
                if (key_cb.key_up < key_shake_table.up_cnt) {
                    // 抬起消抖
                    key_cb.key_up++;
                } else {
                    // printf("KEY_STATE_PRESS\n");
                    key_cb.key_cnt = 0;
                    key_cb.key_up = 0;
					if(USER_MULTI_PRESS_EN && xcfg_cb.user_key_multi_press_en) {
						key_cb.press_cnt += 1;
						key_cb.KEY_STATE = KEY_STATE_DELY;
					} else{
						key_cb.press_cnt = 0;
						key_return = key_cb.pre_keyval | KEY_SHORT_UP;
						key_cb.KEY_STATE = KEY_STATE_NOTHING;
					}
                }
            } else {
                key_cb.key_cnt++;
                key_cb.key_up = 0;
                if (key_cb.key_cnt >= key_shake_table.long_cnt) {
                    // 达到最低长按次数
                    key_cb.KEY_STATE = KEY_STATE_LONG;
                    // 无多击+长按，长按对多击次数重置
                    //key_cb.press_cnt = 0;
                }
            }
        break;
			
        case KEY_STATE_LONG:
		case KEY_STATE_LV1_LONG:
		case KEY_STATE_LV2_LONG:
            if (key_val == NO_KEY) {
                if (key_cb.key_up < key_shake_table.up_cnt) {
                    // 抬起消抖
                    key_cb.key_up++;
                } else {
                    // 输出长按抬起事件
                    // printf("KEY_STATE_LONG_UP\n");
                    if(key_cb.KEY_STATE == KEY_STATE_LV1_LONG) {
						if(key_cb.press_cnt == 0) {
							key_return = key_cb.pre_keyval | KEY_LONG_UP;
						}
                    }
					
                    key_cb.press_cnt = 0; //长按抬起事件对多击次数重置
                    key_cb.key_cnt = 0;
					key_cb.repeat_cnt = 0;
                    key_cb.KEY_STATE = KEY_STATE_UP;
                }
            } else {
                key_cb.key_cnt++;
                key_cb.key_up = 0;

				//软关机长按时间检测
				if(key_cb.key_cnt == sys_cb.poweroff_hold_cnt) {
					// printf("KEY_LHOLD\n");
					key_return = key_cb.pre_keyval | KEY_LHOLD;
					return key_return;
				}

				if(key_cb.KEY_STATE == KEY_STATE_LONG) {
					if(key_cb.key_cnt >= key_shake_table.lv1_long_cnt) {
						// printf("KEY_STATE_LV1_LONG\n");
						if(key_cb.press_cnt == 0) {
							key_return = key_cb.pre_keyval | KEY_LONG; //LV1长按
						}
						key_cb.KEY_STATE = KEY_STATE_LV1_LONG;
						return key_return; //key_cb.repeat_cnt不需要+1
					}
				} else if(key_cb.KEY_STATE == KEY_STATE_LV1_LONG) {
					if(key_cb.key_cnt >= key_shake_table.lv2_long_cnt) {
						// printf("KEY_STATE_LV2_LONG\n");
						if(key_cb.press_cnt == 2) {
							key_return = key_cb.pre_keyval | KEY_DOUBLE_LV2_LONG; //双击+LV2长按
						} else if(key_cb.press_cnt == 0){
							key_return = key_cb.pre_keyval | KEY_LV2_LONG; //LV2长按
						}
						key_cb.KEY_STATE = KEY_STATE_LV2_LONG;
					}
				}
				
				if(key_cb.KEY_STATE >= KEY_STATE_LV1_LONG) { //长按次数大于KEY_LV1_LONG_TIMES后才可以触发连按事件
					key_cb.repeat_cnt += 1;
					if(key_cb.repeat_cnt >= key_shake_table.hold_cnt)
					{
						if(key_return == NO_KEY) { //如果有长按事件，先输出长按事件，后输出长按保持事件
							key_cb.repeat_cnt = 0;
							// 输出长按保持时间
							if(key_cb.press_cnt == 0) {
								key_return = key_cb.pre_keyval | KEY_HOLD;
							}
						}
					}
				}
            }
        break;
			
        // 延迟等待多击输出时间
        case KEY_STATE_DELY:
            if (key_val == NO_KEY) {
                key_cb.delay_cnt++;
                key_cb.key_cnt = 0;
                if (key_cb.delay_cnt >= sys_cb.double_delay_cnt) {
                    // printf("key_cb.press_cnt=%d\n", key_cb.press_cnt);
                    switch(key_cb.press_cnt) {
                        case 1:
                            key_return = key_cb.pre_keyval | KEY_SHORT_UP;
                            break;
                        case 2:
                            key_return = key_cb.pre_keyval | KEY_DOUBLE;
                            break;
                        case 3:
                            key_return = key_cb.pre_keyval | KEY_THREE;
                            break;
                        case 4:
                            key_return = key_cb.pre_keyval | KEY_FOUR;
                            break;
                        case 5:
                            key_return = key_cb.pre_keyval | KEY_FIVE;
                            break;
                        default:
                            break;
                    }
                    key_cb.press_cnt = 0;
                    key_cb.delay_cnt = 0;
                    key_cb.KEY_STATE = KEY_STATE_NOTHING;
                }
            } else {
                key_cb.key_cnt++;
                if (key_cb.key_cnt >= key_shake_table.scan_cnt) {
					key_cb.delay_cnt = 0;
                    key_cb.KEY_STATE = KEY_STATE_PRESS;
                }
            }
        break;

		default:
			break;
    }
	
    return key_return;
}
//End

AT(.com_text.bsp.key)
u16 bsp_key_process(u16 key_val)
{
#if USER_KEY_KNOB2_EN
 	static u8 timer1ms_cnt=0;
    timer1ms_cnt++;
    u16 key_ret = knob_process2(&key_val);
    if (key_ret != 0xffff && key_ret != NO_KEY) {
        return key_ret;
    }
    if(timer1ms_cnt < 5) {
        return NO_KEY;              //貌似有问题
    }
    timer1ms_cnt=0;
#endif

//@lewis
#if 1
    u16 key_return = key_deal(key_val);
#else
    u16 key_return = key_process(key_val);

    //双击处理
#if USER_MULTI_PRESS_EN
    //配置工具是否使能了按键2/3/4/5击功能？
    if (xcfg_cb.user_key_multi_press_en) {
        key_return = key_multi_press_process(key_return);
    }
#endif
#endif
//End

    return key_return;
}

AT(.com_rodata.bsp.key)
const char key_enqueue_str[] = "enqueue: %04x\n";

AT(.com_text.bsp.key)
static bool bsp_key_pwron_filter(u16 key_val)
{
    if ((sys_cb.poweron_flag) && ((key_val & K_PWR_MASK) == K_PWR)) {
        return true;           //剔除开机过程PWRKEY产处的按键消息
    }
    return false;
}

AT(.com_text.bsp.key)
u8 bsp_key_scan_do(void)
{
    u8 key_val = NO_KEY;

    if (!bsp_saradc_process()) {
        return NO_KEY;
    }

#if USER_KEY_KNOB_EN
    knob_process();
#endif

#if USER_TKEY
    key_val = bsp_tkey_scan();
#endif

#if USER_PWRKEY
    if ((key_val == NO_KEY) && (!PWRKEY_2_HW_PWRON)) {
        key_val = pwrkey_get_val();
    }
#endif

#if USER_IOKEY
    if (key_val == NO_KEY) {
        key_val = get_iokey();
    }
#endif

#if USER_ADKEY || USER_ADKEY2 ||USER_ADKEY_MUX_SDCLK
    if (key_val == NO_KEY) {
        key_val = adkey_get_val();
    }
#endif
    return key_val;
}


AT(.com_text.bsp.key)
u8 bsp_key_scan(void)
{
    u8 key_val;
    u16 key = NO_KEY;

    key_val = bsp_key_scan_do();

    key = bsp_key_process(key_val);
    if ((key != NO_KEY) && (!bsp_key_pwron_filter(key))) {
        //防止enqueue多次HOLD消息
        if ((key & KEY_TYPE_MASK) == KEY_LONG) {
            sys_cb.kh_vol_msg = (key & 0xff) | KEY_HOLD;
        } else if ((key & KEY_TYPE_MASK) == KEY_LONG_UP) {
            msg_queue_detach(sys_cb.kh_vol_msg, 0);
            sys_cb.kh_vol_msg = NO_KEY;
        } else if (sys_cb.kh_vol_msg == key) {
            msg_queue_detach(key, 0);
        }
        printf(key_enqueue_str, key);
        msg_enqueue(key);
    }
    return key_val;
}

uint8_t bsp_key_pwr_scan(void)
{
    uint8_t key_val = NO_KEY;

#if USER_TKEY
    key_val = bsp_tkey_scan();
#endif

#if USER_PWRKEY
    if (key_val == NO_KEY) {
        key_val = pwrkey_get_val();
    }
#endif // USER_PWRKEY
    return key_val;
}


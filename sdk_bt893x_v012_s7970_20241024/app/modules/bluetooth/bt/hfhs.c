#include "include.h"
#include "api.h"

char redial_buf[2][32];

void bt_redial_init(void)
{
    memset(redial_buf, 0, 64);
}

void bt_reset_redial_number(uint8_t index)
{
    memset(redial_buf[index], 0, 32);
}

void bt_update_redial_number(uint8_t index, char *buf, u32 len)
{
    if (len < 32) {
        memset(redial_buf[index], 0, 32);
        memcpy(redial_buf[index], buf, len);
    }
}

const char *bt_get_last_call_number(uint8_t index)
{
    if (redial_buf[index][0]) {
        return (const char *)redial_buf[index];
    } else {
        return NULL;
    }
}

const char *hfp_get_last_call_number(uint8_t index) {
    return bt_get_last_call_number(index);
}

#if AB_MATE_CALL_EN
void hfp_hf_emit_curr_calls(u8 idx, u8 type, const char * number, u32 len)
{
    bt_update_redial_number(idx, (char *)number, len);
}

AT(.text.func.btring)
bool hfp_is_list_curr_calls(void) {
    return true;
}
#endif

/************************使用示例************************/
////按键发起回拨号码
//bt_call_redial_number();
//delay_5ms(10); //延迟一下，等它发送完毕
//
////此函数返回需要回拨的号码，例如“10086”
//char * hfp_get_outgoing_number(void) {
//    return "10086";
//}

////发送自定义ATCMD
//bt_hfp_send_at_cmd();
//delay_5ms(10); //延迟一下，等它发送完毕

#if BT_MAP_EN
void bt_get_time(char *ptr);
//此函数返回需要发送的ATCMD
char * hfp_get_at_cmd(void) {
//    return "AT+VTS=1"; //例如，通话过程发起号码键"1"
//    return "ATD10086;"; //也可以，发起回拨号码"10086"
   return "AT+CCLK?\r";//获取IOS手机时间（安卓暂不支持），获取回调函数hfp_get_time
}

//获取IOS手机时间（安卓暂不支持）,需要先发送"AT+CCLK?\r"AT命令
void hfp_notice_network_time(u8 *buf, u16 len)
{
    char cache[14];
    memcpy(&cache[2], buf, 12);
    bt_get_time((char *)cache);
}
#endif

uint hfp_get_mic_gain(void)
{
    return 9;
}

uint hfp_get_spk_gain(void)
{
    return (sys_cb.hfp_vol>15)? 15 : sys_cb.hfp_vol;
}

uint hfp_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 10)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }
    uint bat_level = (sys_cb.vbat - bat_off) / ((4200 - bat_off) / 10);
    if (bat_level) {
        bat_level--;
    }
//    printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
    return bat_level;
#else
    return 9;
#endif
}

uint hfp_get_bat_level_ex(void)
{
    uint level_bat;
    level_bat = hfp_get_bat_level();
    return (level_bat+1) * 10;
}


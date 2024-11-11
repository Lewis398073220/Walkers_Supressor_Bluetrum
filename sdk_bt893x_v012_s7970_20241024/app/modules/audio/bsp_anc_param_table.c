#include "include.h"
#include "bsp_anc_param_table.h"

#if ANC_EN
const anc_param anc_fffb_eq_tbl[8] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_NOS_EQ,      &RES_LEN_ANC_L0_NOS_EQ},
    {&RES_BUF_ANC_L0_MSC_EQ,      &RES_LEN_ANC_L0_MSC_EQ},
    {&RES_BUF_ANC_R0_NOS_EQ,      &RES_LEN_ANC_R0_NOS_EQ},
    {&RES_BUF_ANC_R0_MSC_EQ,      &RES_LEN_ANC_R0_MSC_EQ},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_NOS_EQ,   &RES_LEN_ANC_TP_L0_NOS_EQ},
    {&RES_BUF_ANC_TP_L0_MSC_EQ,   &RES_LEN_ANC_TP_L0_MSC_EQ},
    {&RES_BUF_ANC_TP_R0_NOS_EQ,   &RES_LEN_ANC_TP_R0_NOS_EQ},
    {&RES_BUF_ANC_TP_R0_MSC_EQ,   &RES_LEN_ANC_TP_R0_MSC_EQ},
};

const anc_param anc_fffb_drc_tbl[4] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_DRC,         &RES_LEN_ANC_L0_DRC},
    {&RES_BUF_ANC_R0_DRC,         &RES_LEN_ANC_R0_DRC},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_DRC,      &RES_LEN_ANC_TP_L0_DRC},
    {&RES_BUF_ANC_TP_R0_DRC,      &RES_LEN_ANC_TP_R0_DRC},
};

const anc_param anc_fffb_drc_eq_tbl[4] = {
    //降噪模式参数
    {&RES_BUF_ANC_L0_DRC_EQ,        &RES_LEN_ANC_L0_DRC_EQ},
    {&RES_BUF_ANC_R0_DRC_EQ,        &RES_LEN_ANC_R0_DRC_EQ},
    //通透模式参数
    {&RES_BUF_ANC_TP_L0_DRC_EQ,     &RES_LEN_ANC_TP_L0_DRC_EQ},
    {&RES_BUF_ANC_TP_R0_DRC_EQ,     &RES_LEN_ANC_TP_R0_DRC_EQ},
};

const anc_param anc_hybrid_eq_tbl[2][8] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_NOS_EQ,        &RES_LEN_ANC_L0_NOS_EQ},
        {&RES_BUF_ANC_L0_MSC_EQ,        &RES_LEN_ANC_L0_MSC_EQ},
        {&RES_BUF_ANC_L1_NOS_EQ,        &RES_LEN_ANC_L1_NOS_EQ},
        {&RES_BUF_ANC_L1_MSC_EQ,        &RES_LEN_ANC_L1_MSC_EQ},
        {&RES_BUF_ANC_R0_NOS_EQ,        &RES_LEN_ANC_R0_NOS_EQ},
        {&RES_BUF_ANC_R0_MSC_EQ,        &RES_LEN_ANC_R0_MSC_EQ},
        {&RES_BUF_ANC_R1_NOS_EQ,        &RES_LEN_ANC_R1_NOS_EQ},
        {&RES_BUF_ANC_R1_MSC_EQ,        &RES_LEN_ANC_R1_MSC_EQ},
    },
    {
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_NOS_EQ,     &RES_LEN_ANC_TP_L0_NOS_EQ},
        {&RES_BUF_ANC_TP_L0_MSC_EQ,     &RES_LEN_ANC_TP_L0_MSC_EQ},
        {&RES_BUF_ANC_TP_L1_NOS_EQ,     &RES_LEN_ANC_TP_L1_NOS_EQ},
        {&RES_BUF_ANC_TP_L1_MSC_EQ,     &RES_LEN_ANC_TP_L1_MSC_EQ},
        {&RES_BUF_ANC_TP_R0_NOS_EQ,     &RES_LEN_ANC_TP_R0_NOS_EQ},
        {&RES_BUF_ANC_TP_R0_MSC_EQ,     &RES_LEN_ANC_TP_R0_MSC_EQ},
        {&RES_BUF_ANC_TP_R1_NOS_EQ,     &RES_LEN_ANC_TP_R1_NOS_EQ},
        {&RES_BUF_ANC_TP_R1_MSC_EQ,     &RES_LEN_ANC_TP_R1_MSC_EQ},
    },
};

const anc_param anc_hybrid_drc_tbl[2][4] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_DRC,           &RES_LEN_ANC_L0_DRC},
        {&RES_BUF_ANC_L1_DRC,           &RES_LEN_ANC_L1_DRC},
        {&RES_BUF_ANC_R0_DRC,           &RES_LEN_ANC_R0_DRC},
        {&RES_BUF_ANC_R1_DRC,           &RES_LEN_ANC_R1_DRC},
    },
    {
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_DRC,        &RES_LEN_ANC_TP_L0_DRC},
        {&RES_BUF_ANC_TP_L1_DRC,        &RES_LEN_ANC_TP_L1_DRC},
        {&RES_BUF_ANC_TP_R0_DRC,        &RES_LEN_ANC_TP_R0_DRC},
        {&RES_BUF_ANC_TP_R1_DRC,        &RES_LEN_ANC_TP_R1_DRC},
    },
};

const anc_param anc_hybrid_drc_eq_tbl[2][4] = {
    {
        //降噪模式参数
        {&RES_BUF_ANC_L0_DRC_EQ,        &RES_LEN_ANC_L0_DRC_EQ},
        {&RES_BUF_ANC_L1_DRC_EQ,        &RES_LEN_ANC_L1_DRC_EQ},
        {&RES_BUF_ANC_R0_DRC_EQ,        &RES_LEN_ANC_R0_DRC_EQ},
        {&RES_BUF_ANC_R1_DRC_EQ,        &RES_LEN_ANC_R1_DRC_EQ},
    },
    {
        //通透模式参数
        {&RES_BUF_ANC_TP_L0_DRC_EQ,     &RES_LEN_ANC_TP_L0_DRC_EQ},
        {&RES_BUF_ANC_TP_L1_DRC_EQ,     &RES_LEN_ANC_TP_L1_DRC_EQ},
        {&RES_BUF_ANC_TP_R0_DRC_EQ,     &RES_LEN_ANC_TP_R0_DRC_EQ},
        {&RES_BUF_ANC_TP_R1_DRC_EQ,     &RES_LEN_ANC_TP_R1_DRC_EQ},
    },
};
#endif

#if ANC_EQ_RES2_EN
const anc_exparam anc_fffb_eq_tbl2[8] = {             
    //降噪模式参数
    {RES2_BUF_ANC_L0_NOS_EQ,      RES2_LEN_ANC_L0_NOS_EQ},
    {RES2_BUF_ANC_L0_MSC_EQ,      RES2_LEN_ANC_L0_MSC_EQ},
    {RES2_BUF_ANC_R0_NOS_EQ,      RES2_LEN_ANC_R0_NOS_EQ},
    {RES2_BUF_ANC_R0_MSC_EQ,      RES2_LEN_ANC_R0_MSC_EQ},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_NOS_EQ,   RES2_LEN_ANC_TP_L0_NOS_EQ},
    {RES2_BUF_ANC_TP_L0_MSC_EQ,   RES2_LEN_ANC_TP_L0_MSC_EQ},
    {RES2_BUF_ANC_TP_R0_NOS_EQ,   RES2_LEN_ANC_TP_R0_NOS_EQ},
    {RES2_BUF_ANC_TP_R0_MSC_EQ,   RES2_LEN_ANC_TP_R0_MSC_EQ},
};

const anc_exparam anc_fffb_drc_tbl2[4] = {
    //降噪模式参数
    {RES2_BUF_ANC_L0_DRC,         RES2_LEN_ANC_L0_DRC},
    {RES2_BUF_ANC_R0_DRC,         RES2_LEN_ANC_R0_DRC},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_DRC,      RES2_LEN_ANC_TP_L0_DRC},
    {RES2_BUF_ANC_TP_R0_DRC,      RES2_LEN_ANC_TP_R0_DRC},
};

const anc_exparam anc_fffb_drc_eq_tbl2[4] = {
    //降噪模式参数
    {RES2_BUF_ANC_L0_DRC_EQ,        RES2_LEN_ANC_L0_DRC_EQ},
    {RES2_BUF_ANC_R0_DRC_EQ,        RES2_LEN_ANC_R0_DRC_EQ},
    //通透模式参数
    {RES2_BUF_ANC_TP_L0_DRC_EQ,     RES2_LEN_ANC_TP_L0_DRC_EQ},
    {RES2_BUF_ANC_TP_R0_DRC_EQ,     RES2_LEN_ANC_TP_R0_DRC_EQ},
};

const anc_exparam anc_hybrid_eq_tbl2[2][8] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_NOS_EQ,        RES2_LEN_ANC_L0_NOS_EQ},
        {RES2_BUF_ANC_L0_MSC_EQ,        RES2_LEN_ANC_L0_MSC_EQ},
        {RES2_BUF_ANC_L1_NOS_EQ,        RES2_LEN_ANC_L1_NOS_EQ},
        {RES2_BUF_ANC_L1_MSC_EQ,        RES2_LEN_ANC_L1_MSC_EQ},
        {RES2_BUF_ANC_R0_NOS_EQ,        RES2_LEN_ANC_R0_NOS_EQ},
        {RES2_BUF_ANC_R0_MSC_EQ,        RES2_LEN_ANC_R0_MSC_EQ},
        {RES2_BUF_ANC_R1_NOS_EQ,        RES2_LEN_ANC_R1_NOS_EQ},
        {RES2_BUF_ANC_R1_MSC_EQ,        RES2_LEN_ANC_R1_MSC_EQ},
    },
    {
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_NOS_EQ,     RES2_LEN_ANC_TP_L0_NOS_EQ},
        {RES2_BUF_ANC_TP_L0_MSC_EQ,     RES2_LEN_ANC_TP_L0_MSC_EQ},
        {RES2_BUF_ANC_TP_L1_NOS_EQ,     RES2_LEN_ANC_TP_L1_NOS_EQ},
        {RES2_BUF_ANC_TP_L1_MSC_EQ,     RES2_LEN_ANC_TP_L1_MSC_EQ},
        {RES2_BUF_ANC_TP_R0_NOS_EQ,     RES2_LEN_ANC_TP_R0_NOS_EQ},
        {RES2_BUF_ANC_TP_R0_MSC_EQ,     RES2_LEN_ANC_TP_R0_MSC_EQ},
        {RES2_BUF_ANC_TP_R1_NOS_EQ,     RES2_LEN_ANC_TP_R1_NOS_EQ},
        {RES2_BUF_ANC_TP_R1_MSC_EQ,     RES2_LEN_ANC_TP_R1_MSC_EQ},
    },
};

const anc_exparam anc_hybrid_drc_tbl2[2][4] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_DRC,           RES2_LEN_ANC_L0_DRC},
        {RES2_BUF_ANC_L1_DRC,           RES2_LEN_ANC_L1_DRC},
        {RES2_BUF_ANC_R0_DRC,           RES2_LEN_ANC_R0_DRC},
        {RES2_BUF_ANC_R1_DRC,           RES2_LEN_ANC_R1_DRC},
    },
    {
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_DRC,        RES2_LEN_ANC_TP_L0_DRC},
        {RES2_BUF_ANC_TP_L1_DRC,        RES2_LEN_ANC_TP_L1_DRC},
        {RES2_BUF_ANC_TP_R0_DRC,        RES2_LEN_ANC_TP_R0_DRC},
        {RES2_BUF_ANC_TP_R1_DRC,        RES2_LEN_ANC_TP_R1_DRC},
    },
};

const anc_exparam anc_hybrid_drc_eq_tbl2[2][4] = {
    {
        //降噪模式参数
        {RES2_BUF_ANC_L0_DRC_EQ,        RES2_LEN_ANC_L0_DRC_EQ},
        {RES2_BUF_ANC_L1_DRC_EQ,        RES2_LEN_ANC_L1_DRC_EQ},
        {RES2_BUF_ANC_R0_DRC_EQ,        RES2_LEN_ANC_R0_DRC_EQ},
        {RES2_BUF_ANC_R1_DRC_EQ,        RES2_LEN_ANC_R1_DRC_EQ},
    },
    {
        //通透模式参数
        {RES2_BUF_ANC_TP_L0_DRC_EQ,     RES2_LEN_ANC_TP_L0_DRC_EQ},
        {RES2_BUF_ANC_TP_L1_DRC_EQ,     RES2_LEN_ANC_TP_L1_DRC_EQ},
        {RES2_BUF_ANC_TP_R0_DRC_EQ,     RES2_LEN_ANC_TP_R0_DRC_EQ},
        {RES2_BUF_ANC_TP_R1_DRC_EQ,     RES2_LEN_ANC_TP_R1_DRC_EQ},
    },
};
#endif

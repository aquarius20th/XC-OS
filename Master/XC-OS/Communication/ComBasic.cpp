#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "ComPrivate.h"

TaskHandle_t TaskHandle_Commmunicate;

/*失联超时500ms*/
#define ConnectLost_TimeOut 500

/*实例化NRF对象*/
NRF_Basic nrf(
    NRF_MOSI_Pin, NRF_MISO_Pin, NRF_SCK_Pin, 
    NRF_CE_Pin, NRF_CSN_Pin
);
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND

NRF_TRM  nrfTRM(&nrf);
NRF_FHSS nrfFHSS(&nrf);

/*NRF发送数据缓冲区*/
uint8_t NRF_TxBuff[32];

/*NRF接收数据缓冲区*/
uint8_t NRF_RxBuff[32];

/*NRF地址分配表*/
uint8_t NRF_AddressConfig[NRF_AddressNum * 5] =
{
    45, 23, 78, 19, 61, //ADDR:0
    75, 75, 75, 75, 75, //ADDR:1
    12, 34, 56, 78, 90, //ADDR:2
    12, 90, 17, 44, 55, //ADDR:3
    10, 93, 70, 72, 31, //ADDR:4
    0//ADDR: Handshake
};

/*NRF信号强度指示*/
int16_t NRF_SignalStrength = 0;

/*NRF基本配置表*/
NRF_Config_TypeDef NRF_Cfg = {0, 0, 40};

/*发送数据使能*/
bool State_RF = OFF;

/*回传使能*/
bool State_PassBack = ON;

/*通用回传模式使能*/
bool Enable_CommonPassBack = true;

/*锁定回传数据包*/
bool Lock_CommonPassback = false;

/*握手使能*/
bool State_Handshake = ON;

/*跳频使能*/
bool State_FHSS = OFF;

/**
  * @brief  NRF初始化
  * @param  无
  * @retval true成功 false失败
  */
bool Init_NRF()
{
    /*默认初始化*/
    nrf.Init();

    /*发送模式*/
    nrf.TX_Mode();
    
    /*设置地址*/
    nrf.SetAddress((uint8_t*)NRF_AddressConfig + NRF_Cfg.Address * 5);

    /*设置频率*/
    nrf.SetFreqency(NRF_Cfg.Freq);

    /*设置速率*/
    nrf.SetSpeed(NRF_Cfg.Speed);

    /*数据包长度*/
    nrf.SetPayloadWidth(sizeof(NRF_TxBuff), sizeof(NRF_RxBuff));
    
    /*开射频*/
    nrf.SetRF_Enable(true);
    
    /*更新寄存器状态*/
    nrf.UpdateRegs();

    /*返回连接情况*/
    return nrf.IsDetect();
}

/********************** 协议层 **********************/
/*控制器*/
Protocol_Common_t CTRL;

/*差速转向车辆对象*/
Protocol_CarDS_t CarDS;

/*舵机转向车辆对象*/
Protocol_CarSS_t CarSS;

/*通用对象*/
Protocol_Common_t COMMON_X;

/*多通道浮点型数据回传*/
Protocol_Passback_xChannel_t xChannel_Passback;

/*通用型文本数据回传*/
Protocol_Passback_CommonDisplay_t Common_Passback;

/*通信状态指示*/
ConnectState_t ConnectState;

/**
  * @brief  数据包加载
  * @param  无
  * @retval 无
  */
void LoadDataPack()
{
    float Limit_L = CTRL.KnobLimit.L / (float)CtrlOutput_MaxValue;//左限幅映射0.0~1.0
    float Limit_R = CTRL.KnobLimit.R / (float)CtrlOutput_MaxValue;//右限幅映射0.0~1.0

    CTRL.Info.DataPackage_Num++;//数据包心跳自加

    /*为当前所控制的对象加载数据包*/

    if(CTRL.Info.CtrlObject == X_COMMON)
    {
        COMMON_X = CTRL;
    }
    else if(CTRL.Info.CtrlObject == CAR_DifferentalSteering)
    {
        CarDS.Info = CTRL.Info;
        CarDS.Key = CTRL.Key;
        CarDS.KnobLimit = CTRL.KnobLimit;
        CarDS.KnobCab = CTRL.KnobCab;

        switch(CTRL.Info.CtrlMode)
        {
        case CtrlMode_Tank:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L;
            CarDS.MotorSpeed.R = CTRL.Right.Y * Limit_R;
            break;

        case CtrlMode_Race:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Right.X * Limit_R;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Right.X * Limit_R;
            break;

        case CtrlMode_Expert:
            CarDS.MotorSpeed.L = CTRL.Left.Y * Limit_L + CTRL.Left.X * Limit_L * 0.5f;
            CarDS.MotorSpeed.R = CTRL.Left.Y * Limit_L - CTRL.Left.X * Limit_L * 0.5f;
            break;
        }
        __LimitValue(CarDS.MotorSpeed.L, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarDS.MotorSpeed.R, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
    else if(CTRL.Info.CtrlObject == CAR_ServoSteering)
    {
        CarSS.Info = CTRL.Info;
        CarSS.Key = CTRL.Key;
        CarSS.KnobLimit = CTRL.KnobLimit;
        CarSS.KnobCab = CTRL.KnobCab;

        CarSS.MotorSpeed = CTRL.Left.Y;
        CarSS.SteerAngle = CTRL.Right.X;

        __LimitValue(CarSS.MotorSpeed, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
        __LimitValue(CarSS.SteerAngle, -CtrlOutput_MaxValue, CtrlOutput_MaxValue);
    }
}

/*自定义发送数据包回调*/
static CustomDataPack_Callback_t LoadUserCustomDataPack = 0;

/*自定义回传数据包回调*/
static CustomDataPack_Callback_t LoadUserCustomPassBack_Group[10] = {0};

/**
  * @brief  设置自定义发送数据包回调
  * @param  无
  * @retval 无
  */
void SetUserCustomDataPack_Callback(CustomDataPack_Callback_t callback)
{
    LoadUserCustomDataPack = callback;
}

/**
  * @brief  设置自定义回传数据包回调
  * @param  无
  * @retval 无
  */
bool AddUserCustomPassBack_Callback(CustomDataPack_Callback_t callback)
{
    static uint8_t regCnt = 0;

    /*寻找是否有重复函数*/
    for(uint8_t i = 0; i < __Sizeof(LoadUserCustomPassBack_Group); i++)
    {
        if(callback == LoadUserCustomPassBack_Group[i])
            return true;
    }

    /*在新的空位填入函数地址*/
    if(regCnt < __Sizeof(LoadUserCustomPassBack_Group))
    {
        LoadUserCustomPassBack_Group[regCnt] = callback;
        regCnt++;
        return true;
    }
    else
        return false;
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
static void CommmunicateProcess()
{
    LoadDataPack();//打包数据包

    /*数据包载入NRF发送缓冲区*/
    switch(CTRL.Info.CtrlObject)
    {
    case X_COMMON:
        __TypeExplain(Protocol_Common_t, NRF_TxBuff) = COMMON_X;
        break;

    case CAR_DifferentalSteering:
        __TypeExplain(Protocol_CarDS_t, NRF_TxBuff) = CarDS;
        break;

    case CAR_ServoSteering:
        __TypeExplain(Protocol_CarSS_t, NRF_TxBuff) = CarSS;
        break;
    default:
        if(LoadUserCustomDataPack)
            LoadUserCustomDataPack(NRF_TxBuff);
        break;
    }

    if(ConnectState.Pattern == Pattern_NRF)
    {
        if(State_FHSS)
        {
            nrfFHSS.TxProcess(NRF_TxBuff, NRF_RxBuff);
        }
        /*是否开启回传*/
        else if(State_PassBack)
        {
            /*NRF收发数据*/
            nrfTRM.TranRecv(NRF_TxBuff, NRF_RxBuff);

            /*数组第二位为0说明是用户自定义回传类型*/
            if(NRF_RxBuff[1] == 0)
            {
                Enable_CommonPassBack = false;

                /*调用用户自定义回传数据解析数据*/
                for(uint8_t i = 0; i < __Sizeof(LoadUserCustomPassBack_Group); i++)
                {
                    if(LoadUserCustomPassBack_Group[i])
                        LoadUserCustomPassBack_Group[i](NRF_RxBuff);
                    else
                        break;
                }
            }
            else
            {
                Enable_CommonPassBack = true;

                /*是否锁定回传数据*/
                if(!Lock_CommonPassback)
                    Common_Passback = __TypeExplain(Protocol_Passback_CommonDisplay_t, NRF_RxBuff);
            }
        }
        else
        {
            if(nrf.GetRF_State() != nrf.State_TX)
            {
                nrf.TX_Mode();
            }
            nrf.TranCheck();
            nrf.Tran(NRF_TxBuff);//NRF发送数据
        }
    }
}

void NRF_Delay_Callback(uint32_t ms)
{
    vTaskDelay(ms);
}

void CommmunicateRF_Enable(bool en)
{
    State_RF = en;
    if(en)
    {
        xTaskNotifyGive(TaskHandle_Commmunicate);
    }
}

/**
  * @brief  数据发送任务
  * @param  无
  * @retval 无
  */
void Task_Commmunicate(void *pvParameters)
{
    State_RF = Init_NRF();
    
    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        while(State_RF)
        {
            CommmunicateProcess();
            vTaskDelay(10);
        }
    }
}

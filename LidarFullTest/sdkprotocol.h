//版本更新记录 
//				3.0.3 
//					(1)升级文件支持超大文件
//				3.0.4 
//					(1)AlgMapData支持超大数组传输，AlgMapData 中map[] 改为 *map
//					(2)内部支持数据录制与回放


#ifndef __NAVI_PACK_PROTOCOL_H__
#define __NAVI_PACK_PROTOCOL_H__

#define LIDAR_RESOLUTION 360 
#define MAX_TARGET_LIST_SIZE 32
#define MAX_MAP_SIZE	(2000*2000)

#define FILE_PATH_MAX_LEN 64
/// 获得当前SDK的版本号
/// @return 返回NaviPack对象的ID
/// @note 属性值由3个部分组成: 主版本号(Bits 24 ~31), 子版本号(Bits 16 ~ 23), 编译号(Bits 0 ~ 15)
#define MAIN_VER		3
#define CHILD_VER		0
#define COMPILE_VER		4
#define NAVIPACK_VISION		(MAIN_VER<<24 | CHILD_VER << 16 | COMPILE_VER)




typedef float f32;
typedef double f64;

typedef signed long long s64;
typedef signed int  s32;
typedef signed short s16;
typedef signed char  s8;

typedef signed long long const sc64;
typedef signed int  const sc32;  /* Read Only */
typedef signed short const sc16;  /* Read Only */
typedef signed char  const sc8;   /* Read Only */

typedef volatile signed long long vs64;
typedef volatile signed int  vs32;
typedef volatile signed short vs16;
typedef volatile signed char  vs8;

typedef volatile signed long long  const vsc64;
typedef volatile signed int  const vsc32;  /* Read Only */
typedef volatile signed short const vsc16;  /* Read Only */
typedef volatile signed char  const vsc8;   /* Read Only */

typedef unsigned long long u64;
typedef unsigned int  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef unsigned long long const uc64;
typedef unsigned int  const uc32;  /* Read Only */
typedef unsigned short const uc16;  /* Read Only */
typedef unsigned char  const uc8;   /* Read Only */

typedef volatile unsigned long long  vu64;
typedef volatile unsigned int  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile unsigned long long const vuc64;  /* Read Only */
typedef volatile unsigned int  const vuc32;  /* Read Only */
typedef volatile unsigned short const vuc16;  /* Read Only */
typedef volatile unsigned char  const vuc8;   /* Read Only */

#pragma pack(push, 1) 

#define MAX_MAP_LIST 8
#define  ULTRASOUND_NUM      8
typedef struct {
	int mapListNum;
	int rev;
	int mapListId[MAX_MAP_LIST];
}MapListInfo;

typedef struct {
	u16 partNum;
	u16 temp;
	u32 partLen;
	u32 fileLen;
	char fileName[64];
	char md5[32];
}FileInfo;

typedef struct SdkProtocolHeader
{
	u8 deviceAddr;	//  cmd
	u8 functionCode;	// sub cmd
	u16 startAddr;		//sub sub cmd
	u32 len;
}SdkProtocolHeader;




typedef struct CompareMapData_S
{
	unsigned char result;
	unsigned char exists;
	unsigned char align[2];
	long long timeForSN;
	char sn[32];
	float distanceoofUpDownLeftRight[4];
	CompareMapData_S()
	{
		timeForSN = 0;
		sn[0] = 0;
	}
}CompareMapData_S;

typedef struct NetCompareMapData_S
{
	SdkProtocolHeader hdr;
	unsigned char result;
	unsigned char exists;
	unsigned char align[2];
	long long timeForSN;
	char sn[32];
	NetCompareMapData_S()
	{
		timeForSN = 0;
		sn[0] = 0;
	}
}NetCompareMapData_S;

typedef struct {
	char ssid[64];
	char pwd[64];
}WifiParam;

typedef struct {
	s32 x;
	s32 y;
}IntPoint;

typedef struct
{
	u16 ultrasound[8];
	u8 dropSensor;
	u16 irSensor;
	u8 collisionSensor;
	s16 angularPos;
	s32 leftEncoderPos;                  //当前左边里程计的积分位置
	s32 rightEncoderPos;                  //当前右边里程计的积分位置
	s32 lineVelocity;
	s32 angularVelocity;
	u8 chargeStatus;
	u8 batteryStatus;
	u8 pickupStatus;
	u16 errorState;
}ChassisStatusRegister;

typedef struct
{
	SdkProtocolHeader header;
	ChassisStatusRegister status;
	u8 error;
}ChassisDataInfo;

typedef struct
{
	s32 lineVelocity;
	s32 angularVelocity;
}ChassisControlRegister;


typedef struct
{
	//车的形状参数
	u8 chassisShapeParamNum;
	s16 chassisShapeParamX[8];
	s16 chassisShapeParamY[8];
	u16 minTurningRadius; //载体线速度为0原地旋转一周载体覆盖区域外接圆半径
	//超声波传感器
	u8 ultrasoundSensorNum;
	s16 ultrasoundSensorX[8];
	s16 ultrasoundSensorY[8];
	u16 ultrasoundSensorOrientationAngle[8]; //朝向角度
	u16 ultrasoundSensorMinMeasureDistance;
	u16 ultrasoundSensorMaxMeasureDistance;
	u16 ultrasoundSensorFOV; //视场角
	//防跌落传感器
	u8 dropSensorNum;
	s16 dropSensorX[8];
	s16 dropSensorY[8];
	//碰撞传感器
	u8 collisionSensorNum;
	s16 collisionSensorX[8];
	s16 collisionSensorY[8];
	//激光雷达参数
	s16 lidarSensorX;
	s16 lidarSensorY;
	u16 lidarSensorOrientationAngle; //朝向角度
}ChassisParamRegister;

typedef struct
{
	s32 vectorTargetDistance;
	s32 vectorTargetAngle;
	s32 relativeTargetPosX;
	s32 relativeTargetPosY;
	s32 setLineVelocity;
	s32 setAngularVelocity;
	u8 backuint8_tge;
	u8 backuint8_tgeThreshold;
	s32 setBackuint8_tgePosX;
	s32 setBackuint8_tgePosY;
	u8 startMapping;
	u8 stopMapping;
	u8 setDefaultMap;
	u8 emergencyStop;
}AlgControlRegister;

typedef struct
{
	u8 workMode;
	s32 lineVelocity;
	s32 angularVelocity;
	s32 posX;
	s32 posY;
	s32 posSita;
	u16 errorState;
}AlgStatusRegister;

typedef struct
{
	s32 nTargetPosNum;
	s32 phi;
	s32 PathPosX[MAX_TARGET_LIST_SIZE];
	s32 PathPosY[MAX_TARGET_LIST_SIZE];
}AlgTargetPos;


// typedef struct
// {
// 	u16 data[LIDAR_RESOLUTION];
// 	u8 confidence[LIDAR_RESOLUTION];
// }AlgRealLidarData;

typedef struct
{
	u8 sensorType;
	u32 num;						//num of valid point saved in the PosX and PosY with index(0~num)
	s32 posX;					// global pose of Carrier 单位毫米
	s32 posY;
	s32 posSita;
	s32 localPosX[LIDAR_RESOLUTION];
	s32 localPosY[LIDAR_RESOLUTION];
}AlgSensorData;

typedef struct AlgMapData
{
	s32   width;
	s32   height;
	float resolution;
	float x_min;
	float y_min;
	u8* map;	//80*80米

#ifdef __cplusplus
	AlgMapData()
	{
		map = new u8[MAX_MAP_SIZE];
	}
	~AlgMapData()
	{
		delete[] map;
	}
#endif
}AlgMapData;

typedef struct  
{
	s32 LidarPosX;
}CarrierParam;

typedef struct 
{
	s16 maxLineVelocity;
	s16 minLineVelocity;
	s16 maxAngularVelocity;
	s16 minAngularVelocity;
	u16 minPassageWidth;
	u8 enableUltrasound;
	u8 enableUsingTotalMapForNavigation;
}NaviPackParam;

#pragma pack(pop)

enum mapType
{
	TOTOAL_MAP = 0,
	LIDAR_MAP,
	ULTRASON_MAP,
	COLLISION_MAP,
	GROUND_MAP
};

enum ConnectType {
	TCP_CONNECT = 0,
	SERIAL_CONNECT = 1,
};

enum SensorType
{
	ST_LIDAR2D=0x00,
	ST_ULTRASOUND=0x01,
	ST_ENCODER,
	ST_SWITCH,
	ST_CARRIAR,
//	ST_MPU6500,
	ST_LAST
};

enum NaviPackMode {
	MODE_NAVIGATION = 0X00,		//naviPack套件模式为导航模式  默认模式
	MODE_SENSOR = 0X01,			//naviPack套件模式为传感器模式 只回传激光雷达数据
};

// Device Address
#define LIDAR_ADDRESS 0X10
#define ALG_ADDRESS 0x11
#define MCU_ADDRESS 0x12
#define MF_COMPARE_MAP_DATA  0x13       //MF开头的消息表示生产制造相关
#define SAVE_SN  0x14       //保存SN到设备
#define DEVICE_MSG 0x20		//表示状态的更新
#define ERROR_MSG 0x21		//表示错误的处理
#define SELF_MSG 0X22		//表示自定义消息


// Function Code
// ALG
#define ALG_STATUS_REG         0x01
#define ALG_CONTROL_REG_READ   0x02
#define ALG_CONTROL_REG        0x03
#define ALG_SAVE_MAP		   0x04
#define ALG_DATA_READ		   0x05
#define ALG_BUILD_MAP_MANUAL   0x06
#define ALG_BUILD_MAP_AUTO     0x07
#define ALG_NAVIGATION         0x08
#define ALG_LOCATION_PF         0x09
#define ALG_RAWLOG_REVIEW	 0x0A


//ALG_DATA_READ ADDRESS
#define ALG_DATA_ADDR_LIDAR_MAP      0x01 // correlation struct AlgLidarMapData
#define ALG_DATA_ADDR_TARGET_POINTS  0x02
#define ALG_DATA_ADDR_ULTROSON_MAP   0x03
#define ALG_DATA_ADDR_SWITCH_MAP     0x04
#define ALG_DATA_ADDR_MAP      0x05
#define ALG_DATA_ADDR_REAL_LIDAR_DATA 0x06
#define ALG_DATA_ADDR_PLANED_PATH    0x07
#define ALG_DATA_ADDR_STATUS_R    0x08
#define ALG_DATA_ADDR_REAL_ULTROSON_DATA 0x09
#define ALG_DATA_ADDR_REAL_COLLISION_DATA 0x0A
#define ALG_DATA_ADDR_LIDAR_RAW_DATA 0X10	//激光雷达原始数据

//ALG_RAWLOG_REVIEW ADDRESS
#define ALG_RAW_ADDR_BUILDMAP	0x01
#define ALG_RAW_ADDR_DEBUG		0x02

typedef enum {
	ERROR_CODE = 0X00,				//有错误 错误码
	UPDATE_MAP = 0x01,				//地图有更新
	UPDATE_SENSOR_DATA = 0x02,		//传感器数据有更新
	INIT_LOCATION_SUCCESS = 0X03,		//初始定位成功
	UPDATE_ALG_ATATUS_REG = 0X04,	//状态寄存器有更新
	NAVIGATION_STATUS = 0X05,	//目标点设置有更新
	GET_NAVIPACK_VERSION = 0X06,//获取版本号
	SET_NAVAPACK_UPLOAD_MAP = 0X07,//让navipack程序上传当前地图
	SET_SAVE_CURRENT_MAP = 0X08,	//让NaviPack保存当前正在使用的地图
	SET_LOAD_MAP_FROME_LIST = 0X09,	//从地图列表中加载地图
	UPDATE_MAP_LIST = 0X10,			//更新地图列表
	SWITCH_NAVIPACK_MODE = 0X11,	//更改NaviPack模式

	IMU_CALIBRATE_SUCCESS = 0X13,   //IMU标定成功


	//以下是需要回调的
	SET_CHARGE_POSITION = 0XA0,		//设置充电桩位置
	SET_UPDATE_FILE = 0XA1,		//设置更新包
	SET_WIFI_PARAM = 0XA2,		//设置wifi
	CHANGE_LIDAR_TO_SENSOR_MODE = 0XA3,//设置雷达到传感器模式
	CHANGE_LIDAR_TO_PACK_MODE = 0XA4,	//设置雷达为NaviPack模式
	LIDAR_CTRL_SELF_STREAM = 0XA5,		//自定义数据
	SEND_FILE_TYPE_MAP_PACKAGE = 0xA6,	//发送地图文件
	

}DEVICE_CMD_SUB;

typedef enum {
	REACH_POINT = 0X00,				//到达运动点
	TERMINAL = 0x01,				//终止
	PATH_UPGRADE = 0X02,			//路径有更新
}TatgetStatus;

typedef enum DEVICE_CMD_ERROR_CODE
{
	LIDAR_NOT_FOUND = 100,
	MAP_OVER_RANGE = 101,
}DEVICE_CMD_ERROR_CODE;
//

typedef enum {
	PACK_LIDAR_DATA,
	PACK_SET_PIXOFFSET,      /*!< 校准像素偏移量  */
	PACK_FLASH_CONFIG,       /*!< 烧录配置到flash  */
	PACK_GET_GYROSCOPE_DATA, /*!< 获取陀螺仪数据 */
	PACK_SET_SPEED,          /*!< 设置Lidar速度 */
	PACK_FIRMWARE_UPDATE,    /*!< 更新Firmware */
	PACK_ANGLE_OFFSET,       /*!< 调整角度偏移量*/
	PACK_CONTROL_LASER,      /*!< 控制激光*/
	PACK_DEBUG_MODE,         /*!< 调试模式*/
	PACK_START_ROTATE,       /*!< 开始旋转 */
	PACK_COM_TEST,           /*!< 通讯测试模式*/
	PACK_FORCE_LASER_OPEN,   /*!< 强制激光输出*/
	PACK_OPEN_LASER_DEFAULT,
	PACK_CONFIDENCE_FILTER,
	PACK_SET_LASER_MODULATE_PERIOD,
	PACK_ACK = 0xfd,
	PACK_SELF_TEST = 0xfe, /*!< 自检模式 */
	PACK_NULL = 0xff       /*!< 复位值，表明当前没有数据包 */
} PackageIDTypeDef;



typedef struct
{
	PackageIDTypeDef DataID;
	u8               *DataInBuff;
	u32              DataInLen;
	u8               *DataOutBuff;
	u32              *DataOutLen;
	u16              ErrorCode;
} PackageDataStruct;

#define ALG_CR_ADDR_SET_TARGET			0		//设置目标点组
#define ALG_CR_ADDR_SPEED_CONTROL		16
#define ALG_CR_ADDR_BACK_CHARGE			24
#define ALG_CR_ADDR_MAP_BUILDER			34
#define ALG_CR_ADDR_IDLE				37
#define ALG_CR_ADDR_IMU_CELIBRATE		50


// MCU
#define MCU_CONTROL_REG_READ   0x01
#define MCU_CONTROL_REG        0x02
#define MCU_STATUS_REG         0x03
#define MCU_PARAM_REG          0x04
#define MCU_USER_REG_READ      0x05
#define MCU_USER_REG_WRITE     0x06

//雷达数据包头尾、控制字
#define P_HEADER     0xAA
#define P_TAIL       0x55
#define P_CTRL       0xA5
#define P_FAIL       0
#define P_SUCCESS    1

typedef enum
{
	PACK_FAIL,
	PACK_OK
} ResultTypeDef;



typedef void(*DeviceMsgCallBack)(s32 id, s32 funcCode, s32 code, void* param);
typedef void(*ErrorMsgCallBack)(s32 id, s32 errorLevel, s32 errorCode, char* msg);
typedef void(*MapPackageCallBack)(s32 id, FileInfo* fileInfo, s32 checkedOk, const u8* buf, u32 len);
#endif

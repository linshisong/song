//�汾���¼�¼ 
//				3.0.3 
//					(1)�����ļ�֧�ֳ����ļ�
//				3.0.4 
//					(1)AlgMapData֧�ֳ������鴫�䣬AlgMapData ��map[] ��Ϊ *map
//					(2)�ڲ�֧������¼����ط�


#ifndef __NAVI_PACK_PROTOCOL_H__
#define __NAVI_PACK_PROTOCOL_H__

#define LIDAR_RESOLUTION 360 
#define MAX_TARGET_LIST_SIZE 32
#define MAX_MAP_SIZE	(2000*2000)

#define FILE_PATH_MAX_LEN 64
/// ��õ�ǰSDK�İ汾��
/// @return ����NaviPack�����ID
/// @note ����ֵ��3���������: ���汾��(Bits 24 ~31), �Ӱ汾��(Bits 16 ~ 23), �����(Bits 0 ~ 15)
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
	s32 leftEncoderPos;                  //��ǰ�����̼ƵĻ���λ��
	s32 rightEncoderPos;                  //��ǰ�ұ���̼ƵĻ���λ��
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
	//������״����
	u8 chassisShapeParamNum;
	s16 chassisShapeParamX[8];
	s16 chassisShapeParamY[8];
	u16 minTurningRadius; //�������ٶ�Ϊ0ԭ����תһ�����帲���������Բ�뾶
	//������������
	u8 ultrasoundSensorNum;
	s16 ultrasoundSensorX[8];
	s16 ultrasoundSensorY[8];
	u16 ultrasoundSensorOrientationAngle[8]; //����Ƕ�
	u16 ultrasoundSensorMinMeasureDistance;
	u16 ultrasoundSensorMaxMeasureDistance;
	u16 ultrasoundSensorFOV; //�ӳ���
	//�����䴫����
	u8 dropSensorNum;
	s16 dropSensorX[8];
	s16 dropSensorY[8];
	//��ײ������
	u8 collisionSensorNum;
	s16 collisionSensorX[8];
	s16 collisionSensorY[8];
	//�����״����
	s16 lidarSensorX;
	s16 lidarSensorY;
	u16 lidarSensorOrientationAngle; //����Ƕ�
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
	s32 posX;					// global pose of Carrier ��λ����
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
	u8* map;	//80*80��

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
	MODE_NAVIGATION = 0X00,		//naviPack�׼�ģʽΪ����ģʽ  Ĭ��ģʽ
	MODE_SENSOR = 0X01,			//naviPack�׼�ģʽΪ������ģʽ ֻ�ش������״�����
};

// Device Address
#define LIDAR_ADDRESS 0X10
#define ALG_ADDRESS 0x11
#define MCU_ADDRESS 0x12
#define MF_COMPARE_MAP_DATA  0x13       //MF��ͷ����Ϣ��ʾ�����������
#define SAVE_SN  0x14       //����SN���豸
#define DEVICE_MSG 0x20		//��ʾ״̬�ĸ���
#define ERROR_MSG 0x21		//��ʾ����Ĵ���
#define SELF_MSG 0X22		//��ʾ�Զ�����Ϣ


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
#define ALG_DATA_ADDR_LIDAR_RAW_DATA 0X10	//�����״�ԭʼ����

//ALG_RAWLOG_REVIEW ADDRESS
#define ALG_RAW_ADDR_BUILDMAP	0x01
#define ALG_RAW_ADDR_DEBUG		0x02

typedef enum {
	ERROR_CODE = 0X00,				//�д��� ������
	UPDATE_MAP = 0x01,				//��ͼ�и���
	UPDATE_SENSOR_DATA = 0x02,		//�����������и���
	INIT_LOCATION_SUCCESS = 0X03,		//��ʼ��λ�ɹ�
	UPDATE_ALG_ATATUS_REG = 0X04,	//״̬�Ĵ����и���
	NAVIGATION_STATUS = 0X05,	//Ŀ��������и���
	GET_NAVIPACK_VERSION = 0X06,//��ȡ�汾��
	SET_NAVAPACK_UPLOAD_MAP = 0X07,//��navipack�����ϴ���ǰ��ͼ
	SET_SAVE_CURRENT_MAP = 0X08,	//��NaviPack���浱ǰ����ʹ�õĵ�ͼ
	SET_LOAD_MAP_FROME_LIST = 0X09,	//�ӵ�ͼ�б��м��ص�ͼ
	UPDATE_MAP_LIST = 0X10,			//���µ�ͼ�б�
	SWITCH_NAVIPACK_MODE = 0X11,	//����NaviPackģʽ

	IMU_CALIBRATE_SUCCESS = 0X13,   //IMU�궨�ɹ�


	//��������Ҫ�ص���
	SET_CHARGE_POSITION = 0XA0,		//���ó��׮λ��
	SET_UPDATE_FILE = 0XA1,		//���ø��°�
	SET_WIFI_PARAM = 0XA2,		//����wifi
	CHANGE_LIDAR_TO_SENSOR_MODE = 0XA3,//�����״ﵽ������ģʽ
	CHANGE_LIDAR_TO_PACK_MODE = 0XA4,	//�����״�ΪNaviPackģʽ
	LIDAR_CTRL_SELF_STREAM = 0XA5,		//�Զ�������
	SEND_FILE_TYPE_MAP_PACKAGE = 0xA6,	//���͵�ͼ�ļ�
	

}DEVICE_CMD_SUB;

typedef enum {
	REACH_POINT = 0X00,				//�����˶���
	TERMINAL = 0x01,				//��ֹ
	PATH_UPGRADE = 0X02,			//·���и���
}TatgetStatus;

typedef enum DEVICE_CMD_ERROR_CODE
{
	LIDAR_NOT_FOUND = 100,
	MAP_OVER_RANGE = 101,
}DEVICE_CMD_ERROR_CODE;
//

typedef enum {
	PACK_LIDAR_DATA,
	PACK_SET_PIXOFFSET,      /*!< У׼����ƫ����  */
	PACK_FLASH_CONFIG,       /*!< ��¼���õ�flash  */
	PACK_GET_GYROSCOPE_DATA, /*!< ��ȡ���������� */
	PACK_SET_SPEED,          /*!< ����Lidar�ٶ� */
	PACK_FIRMWARE_UPDATE,    /*!< ����Firmware */
	PACK_ANGLE_OFFSET,       /*!< �����Ƕ�ƫ����*/
	PACK_CONTROL_LASER,      /*!< ���Ƽ���*/
	PACK_DEBUG_MODE,         /*!< ����ģʽ*/
	PACK_START_ROTATE,       /*!< ��ʼ��ת */
	PACK_COM_TEST,           /*!< ͨѶ����ģʽ*/
	PACK_FORCE_LASER_OPEN,   /*!< ǿ�Ƽ������*/
	PACK_OPEN_LASER_DEFAULT,
	PACK_CONFIDENCE_FILTER,
	PACK_SET_LASER_MODULATE_PERIOD,
	PACK_ACK = 0xfd,
	PACK_SELF_TEST = 0xfe, /*!< �Լ�ģʽ */
	PACK_NULL = 0xff       /*!< ��λֵ��������ǰû�����ݰ� */
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

#define ALG_CR_ADDR_SET_TARGET			0		//����Ŀ�����
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

//�״����ݰ�ͷβ��������
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

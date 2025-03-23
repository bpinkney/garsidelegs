/*
 * garsidelegs_hw_sensors.c
 *
 */

#include <garsidelegs_hw_sensors.h>
#include "main.h"

// I2C dataready and buffers
static uint8_t I2C2_TX_Buffer_data_ready = 0;
static uint8_t I2C2_RX_Buffer_data_ready = 0;
static uint8_t I2C2_TX_Buffer[32];
static uint8_t I2C2_RX_Buffer[32];

// Overloaded I2C handlers for interrupt
void HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef * hi2c)
{
  // TX Done .. Do Something!
	I2C2_TX_Buffer_data_ready = 1;
	printf("** I2C ICM20X_B0_WHOAMI TX complete ** \n\r");
}

void HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef * hi2c)
{
  // RX Done .. Do Something!
	I2C2_RX_Buffer_data_ready = 1;
	printf("** I2C ICM20X_B0_WHOAMI RX complete ** \n\r");
}

// this works but overcomplicates things for 2 devices,
// TODO: replace polling version with interrupts for multiple devices if needed when running AI solve
void garsidelegs_hw_sensors_imu_process_interrupt_sample(void)
{
	// check dataready for ICM20649 callbacks
	if(I2C2_TX_Buffer_data_ready)
	{
		printf("** I2C2_TX_Buffer_data_ready returned 1 ** \n\r");
		I2C2_TX_Buffer_data_ready = 0;
	}

	if(I2C2_RX_Buffer_data_ready)
	{
		printf("** I2C2_TX_Buffer_data_ready returned 1 and whoami = %#04x ** \n\r", I2C2_RX_Buffer[0]);
		I2C2_RX_Buffer_data_ready = 0;
	}

  // check if the I2C devices exist (interrupt approach TODO)
  if(HAL_I2C_IsDeviceReady(&hi2c2, ICM20649_I2CADDR_DEFAULT, 3, 1000))
  {
  	printf("** ICM20X_B0_WHOAMI check start ** \n\r");
    memset(&I2C2_TX_Buffer, 0, sizeof(I2C2_TX_Buffer));
    I2C2_TX_Buffer[0] = ICM20X_B0_WHOAMI;

    do
    {
			//HAL_I2C_Master_Transmit_IT (I2C_HandleTypeDef * hi2c, uint16_t DevAddress, uint8_t * pData, uint16_t Size);
			if(HAL_I2C_Master_Transmit_IT(&hi2c2, ICM20649_I2CADDR_DEFAULT<<1, I2C2_TX_Buffer, 1)!= HAL_OK) //TX in Interrupt mode
			{
				printf("** HAL_I2C_Master_Transmit_IT failure! ** \n\r");
			}

	     while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
			{
				// not actually needed for async
	    	 //printf("** ICM20X_B0_WHOAMI TX churn ** \n\r");
			}
    }
    while(HAL_I2C_GetError(&hi2c2) == HAL_I2C_ERROR_AF);

    HAL_Delay(1000);

    printf("** ICM20X_B0_WHOAMI RX start ** \n\r");
    /*##-4- Put I2C peripheral in reception process ###########################*/
    do
    {
      if(HAL_I2C_Master_Receive_IT(&hi2c2, ICM20649_I2CADDR_DEFAULT<<1, I2C2_RX_Buffer, 1) != HAL_OK)//RX in Interrupt mode
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*##-5- Wait for the end of the transfer #################################*/
      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it’s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while(HAL_I2C_GetError(&hi2c2) == HAL_I2C_ERROR_AF);

    HAL_Delay(1000);

    //HAL_I2C_Master_Receive_IT (I2C_HandleTypeDef * hi2c, uint16_t DevAddress, uint8_t * pData, uint16_t Size);
    //HAL_I2C_Master_Receive_IT(&hi2c2, ICM20649_I2CADDR_DEFAULT, I2C2_RX_Buffer, 1); //RX in Interrupt mode

  }
  else
  {
  	printf("** ICM20X_B0_WHOAMI check failed due to bad HAL_I2C_IsDeviceReady call! ** \n\r");
  }
}

// I2C write abstraction for busypoll
/*HAL_StatusTypeDef HAL_I2C_Mem_Read(
I2C_HandleTypeDef *hi2c,

uint16_t DevAddress,

uint16_t MemAddress,

uint16_t MemAddSize,

uint8_t *pData,

uint16_t Size,

uint32_t Timeout);*/

/*hi2c
I2C_HandleTypeDef *
Pointer to a I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
DevAddress
uint16_t
Target device address: The device 7 bits address value in datasheet must be shifted to the left before calling the interface
MemAddress
uint16_t
Internal memory address
MemAddSize
uint16_t
Size of internal memory address
pData
uint8_t *
Pointer to data buffer
Size
uint16_t
Amount of data to be sent
Timeout
uint32_t
Timeout duration*/

/*HAL_StatusTypeDef HAL_I2C_Mem_Write(
I2C_HandleTypeDef *hi2c,

uint16_t DevAddress,

uint16_t MemAddress,

uint16_t MemAddSize,

uint8_t *pData,

uint16_t Size,

uint32_t Timeout);*/

/*hi2c
I2C_HandleTypeDef *
Pointer to a I2C_HandleTypeDef structure that contains the configuration information for the specified I2C.
DevAddress
uint16_t
Target device address: The device 7 bits address value in datasheet must be shifted to the left before calling the interface
MemAddress
uint16_t
Internal memory address
MemAddSize
uint16_t
Size of internal memory address
pData
uint8_t *
Pointer to data buffer
Size
uint16_t
Amount of data to be sent
Timeout
uint32_t
Timeout duration*/

void HAL_I2C_write_poll(const int8_t address, const uint8_t reg, const uint8_t data)
{
  // Blocking mode
	// Note: the buffer must be on the heap (no unsized pointers allowed) (I2C2_TX_Buffer)
  if(HAL_I2C_IsDeviceReady(&hi2c2, address, 3, 1000))
	{
		//printf("** I2C write start TX ** \n\r");
		memset(&I2C2_TX_Buffer, 0, sizeof(I2C2_TX_Buffer));

		// set register and data to set
		I2C2_TX_Buffer[0] = reg;
		I2C2_TX_Buffer[1] = data;

		// for bad reasons, we need to shift the address by one for reads and writes
		if(HAL_I2C_Master_Transmit(&hi2c2, address<<1, I2C2_TX_Buffer, 2, 50) != HAL_OK)
		{
	    /* Error_Handler() function is called when Timeout error occurs.
	       When Acknowledge failure occurs (Slave don't acknowledge its address)
	       Master restarts communication */
			printf("** I2C write failed due to bad TX call! ** \n\r");
/*	    if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
	    {
	      Error_Handler();
	      printf("** I2C read failed due to bad TX call! ** \n\r");
	    }*/
	    return;
		}
	}
	else
	{
		printf("** I2C write failed due to bad HAL_I2C_IsDeviceReady call! ** \n\r");
	}
}

// I2C read abstraction for busypoll
void HAL_I2C_read_poll(const int8_t address, const int8_t reg, const uint8_t num_bytes, const uint8_t special)
{
  // Blocking mode
	// Note: the buffer must be on the heap (no unsized pointers allowed) (I2C2_RX_Buffer)
  if(HAL_I2C_IsDeviceReady(&hi2c2, address, 3, 1000))
	{
  	// set register to read from
		memset(&I2C2_TX_Buffer, 0, sizeof(I2C2_TX_Buffer));
		// if num_bytes is greater than 1, assume we just want to read offset to the init reg
		// TODO: We could add an option where the I2C2_RX_Buffer is pre-filled with registers and this overwrite is bypassed if needed
		if(special)
		{
			// special is just all repeats
			for(int8_t byte_offset = 0; byte_offset < num_bytes; byte_offset++)
			{
				I2C2_TX_Buffer[byte_offset] = reg + byte_offset;
			}

		}
		else
		{
			for(int8_t byte_offset = 0; byte_offset < num_bytes; byte_offset++)
			{
				I2C2_TX_Buffer[byte_offset] = reg + byte_offset;
				//printf("** I2C read REG = %x ** \n\r", I2C2_RX_Buffer[byte_offset]);
			}
		}
		//printf("** I2C read start TX ** \n\r");
		// for bad reasons, we need to shift the address by one for reads and writes
		if(HAL_I2C_Master_Transmit(&hi2c2, address<<1, I2C2_TX_Buffer, num_bytes, 1000) != HAL_OK)
		{
	    /* Error_Handler() function is called when Timeout error occurs.
	       When Acknowledge failure occurs (Slave don't acknowledge its address)
	       Master restarts communication */
			printf("** I2C read failed due to bad TX call! ** \n\r");
     /*	    if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
	    {
	      Error_Handler();
	      printf("** I2C read failed due to bad TX call! ** \n\r");
	    }*/
	    return;
		}

		//printf("** I2C read start RX ** \n\r");
	  if(HAL_I2C_Master_Receive(&hi2c2, address<<1, I2C2_RX_Buffer, num_bytes, 1000) != HAL_OK)
	  {
	    /* Error_Handler() function is called when Timeout error occurs.
	       When Acknowledge failure occurs (Slave don't acknowledge it's address)
	       Master restarts communication */
	  	printf("** I2C read failed due to bad RX call! ** \n\r");
/*	    if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
	    {
	      Error_Handler();
	      printf("** I2C read failed due to bad RX call! ** \n\r");
	    }*/
	    return;
	  }

	  if(special)
	  {
			for(uint8_t byte_offset = 0; byte_offset < num_bytes; byte_offset++)
			{
				printf("%d, ", I2C2_RX_Buffer[byte_offset]);
			}
			printf("\n\r");
	  }
	}
	else
	{
		printf("** I2C read failed due to bad HAL_I2C_IsDeviceReady call! ** \n\r");
	}
}

/*Adafruit_BusIO_RegisterBits()
Adafruit_BusIO_RegisterBits::Adafruit_BusIO_RegisterBits	(	Adafruit_BusIO_Register * 	reg,
uint8_t 	bits,
uint8_t 	shift
)
Create a slice of the register that we can address without touching other bits.

Parameters
reg	The Adafruit_BusIO_Register which defines the bus/register
bits	The number of bits wide we are slicing
shift	The number of bits that our bit-slice is shifted from LSB*/

#define ICM20649_DEVICE_ID 0xE1 // Correct content of WHO_AM_I register for IMU
#define ICM20649_BANK(bank) ((bank_number & 0b11) << 4)
uint8_t garsidelegs_hw_sensors_imu_init(void)
{
	// Note: bank number persists until changed
	uint8_t bank_number = 0;
	//uint8_t num_bytes = 0;

	// check whoami for IMU
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_REG_BANK_SEL, ICM20649_BANK(bank_number));
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_WHOAMI, 1, 0);
	// we expect 0xE1 back for WHOAMI
	printf("ICM20649 WHOAMI 0x%02X == 0x%02X\n\r", ICM20649_DEVICE_ID, I2C2_RX_Buffer[0]);

	// take the part out of (default) power saving mode by clearing bit 6
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_PWR_MGMT_1, 0x00);
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_PWR_MGMT_1, 1, 0);
	printf("ICM20649 Clear Sleep Mode 0x%02X == 0x%02X \n\r", 0x00, I2C2_RX_Buffer[0]);

	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_PWR_MGMT_2, 0x00);
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_PWR_MGMT_2, 1, 0);
	printf("ICM20649 Clear Accel Gyro Disable 0x%02X == 0x%02X \n\r", 0x00, I2C2_RX_Buffer[0]);

	//// Set gyro rate divisor (ODR) and LPFILT behaviour
	// For now, we'll pick a smoother filter for fun (change later if delay is too large)
	// set register bank to 2
	bank_number = 2;
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_REG_BANK_SEL, ICM20649_BANK(bank_number));

	// zero for max ODR, for 1100/11 -> 100Hz, go for 10 for now
	const uint8_t ODR_factor_gyro = 10;
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_GYRO_SMPLRT_DIV, ODR_factor_gyro);
	// verify accel ODR was set correctly
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_GYRO_SMPLRT_DIV, 1, 0);
	printf("ICM20649 Gyro ODR 0x%02X == 0x%02X \n\r", ODR_factor_gyro, I2C2_RX_Buffer[0]);


	// bits:
	// 5:3 GYRO_DLPFCFG[2:0] - Gyro low pass filter configuration as shown in Table 15
	// -> go for [4] for 3dB BW = 23.9Hz, and NBW = 35.9Hz for now
	// 2:1 GYRO_FS_SEL[1:0]
	// Gyro Full Scale Select (start with 1000):
	// 00 = ±500 dps
	// 01= ±1000 dps
	// 10 = ±2000 dps
	// 11 = ±4000 dps
	// 0 GYRO_FCHOICE - Must be 1 for non-zero GYRO_DLPFCFG
	uint8_t gyro_config = 0;
	//gyro_config |= 0x4 << 3; 		// GYRO_DLPFCFG
	gyro_config |= ICM20649_GYRO_RANGE_1000_DPS << 1;   // GYRO_FS_SEL
	//gyro_config |= 0x1;         // GYRO_FCHOICE
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_GYRO_CONFIG_1, gyro_config);

	// verify gyro config was set correctly
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_GYRO_CONFIG_1, 1, 0);
	printf("ICM20649 Gyro Config 0x%02X == 0x%02X \n\r", gyro_config, I2C2_RX_Buffer[0]);

	//// Set accel rate divisor (ODR) and LPFILT behaviour
	// For now, we'll pick a smoother filter for fun (change later if delay is too large)
	// zero for max ODR, for 1100/11 -> 100Hz, go for 10 for now
	const uint8_t ODR_factor_accel = 10;
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_ACCEL_SMPLRT_DIV_2, ODR_factor_accel);

	// verify accel ODR was set correctly
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_ACCEL_SMPLRT_DIV_2, 1, 0);
	printf("ICM20649 Accel ODR 0x%02X == 0x%02X \n\r", ODR_factor_accel, I2C2_RX_Buffer[0]);

	// bits:
	// 5:3 ACCEL_DLPFCFG[2:0] - Accel low pass filter configuration as shown in Table 17
	// -> go for [4] for 3dB BW = 23.9Hz, and NBW = 34.4Hz for now
	// 2:1 ACCEL_FS_SEL[1:0]
	// Accelerometer Full Scale Select (use +-30g, we need max range for bouncy steps):
	// 00: ±4g
	// 01: ±8g
	// 10: ±16g
	// 11: ±30g
	// 0 ACCEL_FCHOICE  - Must be 1 for non-zero ACCEL_DLPFCFG
	uint8_t accel_config = 0;
	//accel_config |= 0x4 << 3; 	 // GYRO_DLPFCFG
	accel_config |= ICM20649_ACCEL_RANGE_30_G << 1;   // GYRO_FS_SEL
	//accel_config |= 0x1;         // GYRO_FCHOICE
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_ACCEL_CONFIG_1, accel_config);

	// verify accel config was set correctly
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B2_ACCEL_CONFIG_1, 1, 0);
	printf("ICM20649 Accel Config 0x%02X == 0x%02X \n\r", accel_config, I2C2_RX_Buffer[0]);

	// Revert bank number
	bank_number = 0;
	HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_REG_BANK_SEL, ICM20649_BANK(bank_number));
	printf("ICM20649 Done Init \n\r\n\r");
	return 0;
}

void garsidelegs_hw_sensors_imu_process_poll_sample(void)
{
	//printf("** IMU read start ** \n\r");

	// Note: bank number persists until changed
	uint8_t bank_number = 0;
	uint8_t num_bytes = 0;

	// Revert bank number
	//bank_number = 0;
	//HAL_I2C_write_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_REG_BANK_SEL, ICM20649_BANK(bank_number));

	//HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_GYRO_XOUT_H, 1);
	//int16_t rawGyroXind = I2C2_RX_Buffer[0] << 8;
	//HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_GYRO_XOUT_L, 1);
/*	rawGyroXind |= I2C2_RX_Buffer[0];

	// Read Accel 6 bytes, gyro 6 bytes, temp, and 9 bytes of mag
	num_bytes = 6;
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_ACCEL_XOUT_H, num_bytes);

	int16_t rawAccX = I2C2_RX_Buffer[0] << 8 | I2C2_RX_Buffer[1];
	int16_t rawAccY = I2C2_RX_Buffer[2] << 8 | I2C2_RX_Buffer[3];
	int16_t rawAccZ = I2C2_RX_Buffer[4] << 8 | I2C2_RX_Buffer[5];

	num_bytes = 6;
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_GYRO_XOUT_H, num_bytes);

	int16_t rawGyroX = I2C2_RX_Buffer[0] << 8 | I2C2_RX_Buffer[1];
	int16_t rawGyroY = I2C2_RX_Buffer[2] << 8 | I2C2_RX_Buffer[3];
	int16_t rawGyroZ = I2C2_RX_Buffer[4] << 8 | I2C2_RX_Buffer[5];

	num_bytes = 2;
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_TEMP_OUT_H, num_bytes);

	int16_t temperature = I2C2_RX_Buffer[0] << 8 | I2C2_RX_Buffer[1];

	// TODO: This is garbage data, fix it

	printf("ICM20649 Raw Sample:\n\r   gyro = [%0.2f (%d -> %d), %0.2f, %0.2f]\n\r  accel = [%d, %d, %d]\n\r   temp = [%d]\n\r",
			(float)rawGyroX / 32.8, rawGyroX, rawGyroXind,
			(float)rawGyroY / 32.8,
			(float)rawGyroZ / 32.8,
			rawAccX,
			rawAccY,
			rawAccZ,
			temperature);*/

	num_bytes = 2;
	HAL_I2C_read_poll(ICM20649_I2CADDR_DEFAULT, ICM20X_B0_GYRO_XOUT_H, num_bytes, 1);


}




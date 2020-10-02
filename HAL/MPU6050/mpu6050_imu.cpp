#include "mpu6050_imu.h"

#ifdef MPU6050_MODULE

/*
*   PURPOSE: 
*   This is *another* mpu6050 library, mainly because all of the more simple libraries are gpl 
*   liscensed, and I wanted something that was more C oriented rather that cpp. 
*
*   It's important to visit the invernsense datasheet register map here: 
*   https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf(if link doesnt work search for mpu6050 register datasheet)
*   It's got everything you need to understand all of the registers outline below(but I will provide a brief description)
*/

/*!
*   @brief Sample Rate Divider register. 
*   @note Sets the divider that determines the sample rate of the device. 
*   @note Equation is (default_sample_rate)/(sample_rate + 1).
*   @note Default sample rate of gyroscope is 8khz, and a 1khz accelerometer rate
*   @note Just pass in the divider as the next byte to write to the i2c slave after this register. 
*/
#define SMPLRT_DIV       0x19

/*!
*   @brief Configures the digital low pass filter(DLPF) and the Frame synchronization pin sampling. 
*   @brief For both the accelerometer and the gyroscope.
*   @note To configure the these features, it's essentially a bitmask. 
*   @note To see the official bitmask, visit page 13 of the primary PDF. 
*/
#define CONFIG           0x1A

/*!
*   @brief Configures the Gyroscope reading rate and self tests. 
*   @note Check the enumerated gyroscope values to see the available ranges in the header file
*   @note For more information, check page 14 of the primary PDF. 
*/
#define GYRO_CONFIG      0x1B

/*!
*   @brief Configures the accelorometer reading rates and self tests. 
*   @note Check the enumerated accelerometer values to see the available ranges in the header file
*   @note For more information, check page 15 of the primary pdf. 
*/
#define ACCEL_CONFIG     0x1C

/*!
*   @brief Configures the behavior of the interrupt pins and sets up the i2c bypassing
*   @note For more information, visit page 26 of the primary pdf. 
*/
#define INT_PIN_CFG      0x37

/*!
*   @brief Sets up the interrupt generation via interrupt sources. 
*   @note For more information, visit page 27 of the primay pdf. 
*/
#define INT_ENABLE       0x38

/*!
*   @brief Shows the interrupt status of each interrupt generation source. 
*   @note After reading the bitmask, the associated bits are cleared. 
*   @note For more information, take a look at page 28 of the primary pdf. 
*/
#define INT_STATUS       0x3A

/*!
*   @brief These registers all allow us to fetch the most recent values of information from the mpu6050. 
*   @note For implementation purposes, just call the accel_xout_h register, and since that's the first register
*   @note if you read out all 6 bytes, you'll get the all the accelerometer data. 
*   @note For more information, take a look at page 29 of the primary pdf. 
*/
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40

/*!
*   @brief These registers all allow us to fetch the most recent values of information from the mpu6050. 
*   @note For implementation purposes, just call the accel_xout_h register, and since that's the first register
*   @note if you read out all 6 bytes, you'll get the all the gyroscopeo data. 
*   @note For more information, take a look at page 31 of the primary pdf. 
*/
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48

/*!
*   @brief Allows us to configure the power mode and clock source of the imu. Provides a bit for resetting entire device
*   @brief And a bit for disabling tempurature sensor. 
*   @note For more information on the power management register, visit page 40 of the primary pdf. 
*
*/
#define PWR_MGMT_1       0x6B 

/*!
*   @brief Register that helps identify what type of device we are talking to on the i2c bus 
*   @note Should return 0x68 if it's the correct device.
*   @note Page 45 of the primary pdf may have more information, but it's currently unknown 
*/
#define WHO_AM_I 0x75 

/*!
*   @brief Which i2c device are we reading from 
*/
static uint8_t device_address; 

/*!
*   @brief The set accelerometer range
*   @note Only relevant after you call the init_mpu6050 function
*/
static mpu_accelerometer_range_t accelerometer_range; 


/*!
*   @brief The set accelerometer range
*   @note Only relevant after you call the init_mpu6050 function
*/
static mpu_gyro_range_t gyroscope_range; 

/*              FUNCTION DECLARATIONS BEGIN                 */
mpu_init_status_t init_mpu6050(uint8_t i2c_address, mpu_accelerometer_range_t a_range, mpu_gyro_range_t g_range); 
static inline void i2c_setup_gyroscope(mpu_gyro_range_t g_range);
static inline void i2c_setup_accelerometer(mpu_accelerometer_range_t a_range); 
imu_data_raw get_latest_mpu6050_data(bool blocking); 
imu_data_raw get_latest_mpu6050_data_sampled(uint16_t samples);
static inline void get_mpu6050_gyro_data(imu_data_raw *dat); 
static inline void get_mpu6050_accelerometer_data(imu_data_raw *dat); 
static void i2c_read_bytes(uint8_t sub_addr, uint8_t count, uint8_t *dest); 
static uint8_t i2c_read_byte(uint8_t sub_addr); 
static void i2c_write_byte(uint8_t sub_addr, uint8_t data);
static inline void convert_accel_raw_g_helper(accel_data_g *dat, imu_data_raw *raw_dat, float divider); 
accel_data_g translate_accel_raw_g(imu_data_raw raw_dat); 
accel_data_ms2 translate_accel_g_ms2(accel_data_g dat_g);
static inline void convert_gyro_raw_d_s_helper(gyro_data_d_s *dat, imu_data_raw *raw_dat, float deg_sec);
gyro_data_d_s translate_gyro_raw_d_s(imu_data_raw raw_dat);
/*               FUNCTION DECLARATIONS END                  */

/*!
*   @brief Easy setup of the imu module. 
*   @note Default i2c address is 0x68, specify otherwise
*   @param uint8_t i2c_address
*   @return mpu_init_status_t status of setting up the imu
*/
mpu_init_status_t init_mpu6050(uint8_t i2c_address, mpu_accelerometer_range_t a_range, mpu_gyro_range_t g_range){
    // Start up the i2c device. 
    Wire.begin(); 
    // Tryna overclock. 
    Wire.setClock(600000);

    // Save these for use during conversions
    accelerometer_range = a_range; 
    gyroscope_range = g_range; 

    device_address = i2c_address; 
    
    // Making sure the device we are trying to read
    // Has the right return address. 
    if(i2c_read_byte(WHO_AM_I) != 0x68)
        return MPU6050_NOT_FOUND; 

    // Get's stable time source from the PLL with x-axis gyro reference. 
    // Check Page 40 of the primary pdf for more details.      
    i2c_write_byte(PWR_MGMT_1, 0b00000001); 

    // Configures the accelerometer and the gyroscope. 
    // Disables Fsync
    // Set's sample rate to 1khz for entire imu. 
    i2c_write_byte(CONFIG, 0b00000011); 

    // Sets sample rate dividor to 200hz(1000/ (1 + 4) = 200hz)
    i2c_write_byte(SMPLRT_DIV, 0x04); 

    i2c_setup_gyroscope(g_range); 
    i2c_setup_accelerometer(a_range); 

    // Configure interrupts and bypass enable. 
    // Set's interrupt pins to active high, push-pull and clear on read of INT_STATUS
    // Enables' I2C_BYPASS_EN so more chips can join i2c bus. 
    i2c_write_byte(INT_PIN_CFG, 0b00000010); 
    i2c_write_byte(INT_ENABLE, 0b00000001);                  // Enable data read (bit 0) interrupt

    return MPU6050_INIT_SUCCESS; 
}

/*!
*   @brief Helper function that abstracts i2c setup of gyroscope
*   @param mpu_gyro_range_t g_range
*/
static inline void i2c_setup_gyroscope(mpu_gyro_range_t g_range){
    // Set's gyro range 
    // Set FS_SEL and AFS_SEL bitmasks. 
    register uint8_t buffer = i2c_read_byte(GYRO_CONFIG); 
    buffer &= 0b11100111; 
    buffer |= (((uint8_t)g_range) << 3); 
    i2c_write_byte(GYRO_CONFIG, buffer | ((uint8_t)gyroscope_range << 3)); // Set full range for gyroscope
}

/*!
*   @brief Helper function that abstracts i2c setup of accelereometer
*   @param mpu_accelerometer_range_t a_range
*/
static inline void i2c_setup_accelerometer(mpu_accelerometer_range_t a_range){
    register uint8_t buffer = i2c_read_byte(ACCEL_CONFIG); 
    buffer &= 0b11100111;
    buffer |= (((uint8_t)a_range) << 3); 
    i2c_write_byte(ACCEL_CONFIG, buffer | ((uint8_t)a_range << 3)); // Set full range for gyroscope
}

/*!
*   @brief Getting the latest data from the imu
*   @param bool Whether or not we will wait for data to be available or not
*/
imu_data_raw get_latest_mpu6050_data(bool blocking){
    imu_data_raw dat;

    if(blocking == false){
        dat.success = false; 
        if(!(i2c_read_byte(INT_STATUS) &0x01))
            return dat; 
        
        dat.success = true; 
        get_mpu6050_accelerometer_data(&dat); 
        get_mpu6050_gyro_data(&dat);
    }
    else{
        // Sit and wait for new data to come in 
        while(!(i2c_read_byte(INT_STATUS) & 0x01))
            _os_yield();    // Since this module is built into Will-OS, we call the os yield

        // We were able to get data successfully. 
        dat.success = true; 
        get_mpu6050_accelerometer_data(&dat); 
        get_mpu6050_gyro_data(&dat); 
    }
    return dat; 
}

/*!
*   @brief Function that lets us get a group of sampled data from the imu to prevent outliers causing issues. 
*   @param uint16_t (number of samples )
*/
imu_data_raw get_latest_mpu6050_data_sampled(uint16_t samples){
    imu_data_raw dat;
    
    // Buffer of data that we will use to help with filtering. 
    int32_t a_x = 0, a_y = 0, a_z = 0 , g_x = 0, g_y = 0, g_z = 0; 

    for(int n = 0; n < samples; n++){
        // Sit and wait for new data to come in 
        while(!(i2c_read_byte(INT_STATUS) & 0x01))
            _os_yield();    // Since this module is built into Will-OS, we call the os yield

        // We were able to get data successfully. 
        dat.success = true; 
        get_mpu6050_accelerometer_data(&dat); 
        get_mpu6050_gyro_data(&dat);
        
        // Adding all the data together.  
        a_x += dat.a_x; 
        a_y += dat.a_y; 
        a_z += dat.a_z;
        g_x += dat.g_x; 
        g_y += dat.g_y; 
        g_z += dat.g_z; 
    }

    // Averaging out the accelerometer and gyroscope data. 
    a_x /= samples; 
    a_y /= samples; 
    a_z /= samples; 
    g_x /= samples; 
    g_y /= samples; 
    g_z /= samples; 

    // Fill data back into struct. 
    dat.a_x = a_x; 
    dat.a_y = a_y; 
    dat.a_z = a_z; 
    dat.g_x = g_x; 
    dat.g_y = g_y; 
    dat.g_z = g_z; 

    return dat; 
}

/*!
*   @brief Fetches the latest imu gyroscope data. 
*   @param imy_data_raw* pointer to data to fill in. 
*/
static inline void get_mpu6050_gyro_data(imu_data_raw *dat){
    uint8_t raw_dat[6];
    i2c_read_bytes(ACCEL_XOUT_H, 6, raw_dat); 

    // Concatenate 2 bytes into single uint16_t variable
    dat->g_x = (int16_t)((raw_dat[0] << 8) | raw_dat[1]); 
    dat->g_y = (int16_t)((raw_dat[2] << 8) | raw_dat[3]); 
    dat->g_z = (int16_t)((raw_dat[4] << 8) | raw_dat[5]); 
}

/*!
*   @brief Fetches the latest imu accelerometer data. 
*   @param imy_data_raw* pointer to data to fill in. 
*/
static inline void get_mpu6050_accelerometer_data(imu_data_raw *dat){
    uint8_t raw_dat[6];
    i2c_read_bytes(ACCEL_XOUT_H, 6, raw_dat); 

    // Concatenate 2 bytes into single uint16_t variable
    dat->a_x = (int16_t)((raw_dat[0] << 8) | raw_dat[1]); 
    dat->a_y = (int16_t)((raw_dat[2] << 8) | raw_dat[3]); 
    dat->a_z = (int16_t)((raw_dat[4] << 8) | raw_dat[5]); 
}

/*!
*   @brief Helper function that allows us to fetch a certain amount of bytes on the i2c bus. 
*   @param uint8_t sub_addr, or register that we are writing from 
*   @param uint8_t count, how many bytes are we reading
*   @param uint8_t *dest, pointer to destination arr. 
*/
static void i2c_read_bytes(uint8_t sub_addr, uint8_t count, uint8_t *dest){
    Wire.beginTransmission(device_address); 
    Wire.write(sub_addr); 
    Wire.endTransmission(false); 

    uint8_t n = 0; 
    Wire.requestFrom(device_address, count); 
    while(Wire.available()){
        dest[n++] = Wire.read(); 
    } 
}

/*!
*   @brief Helper function that lets us get a single byte.
*   @param uint8_t sub_addr, or register we are reading from 
*   @return uint8_t return_byte
*/
static uint8_t i2c_read_byte(uint8_t sub_addr){
    uint8_t dat; 
    i2c_read_bytes(sub_addr, 1, &dat); 
    return dat; 
}

/*!
*   @brief Helper function that let's us write bytes to specific registers 
*   @param uint8_t sub_addr, or register we are writing to 
*   @param uint8_t data that we want to right
*/
static void i2c_write_byte(uint8_t sub_addr, uint8_t data){
    Wire.beginTransmission(device_address); 
    Wire.write(sub_addr); 
    Wire.write(data); 
    Wire.endTransmission(); 
}

/*!
*   @brief Helper function that preforms the fpu operations
*   @param accel_data_g *dat
*   @param imu_data_raw *raw_dat
*   @param float divider
*/
static inline void convert_accel_raw_g_helper(accel_data_g *dat, imu_data_raw *raw_dat, float divider){
    dat->a_x = float(raw_dat->a_x)/divider; 
    dat->a_y = float(raw_dat->a_y)/divider; 
    dat->a_z = float(raw_dat->a_z)/divider; 
}

/*!
*   @brief Translates the raw data coming out of the sensor into g data. 
*   @param imu_data_raw raw_dat
*   @return accel_data_g
*/
accel_data_g translate_accel_raw_g(imu_data_raw raw_dat){
    accel_data_g dat;
    switch(accelerometer_range){
    case(ACCELEROMETER_2G):
        convert_accel_raw_g_helper(&dat, &raw_dat, 16384.0);
    break; 
    case(ACCELEROMETER_4G):
        convert_accel_raw_g_helper(&dat, &raw_dat, 8182.0);
    break; 
    
    case(ACCELEROMETER_8G):
        convert_accel_raw_g_helper(&dat, &raw_dat, 4096.0);
    break; 
    
    case(ACCELEROMETER_16G):
        convert_accel_raw_g_helper(&dat, &raw_dat, 2048.0);
    break; 
    default: 
    break; 
    }
    return dat; 
}

/*!
*   @brief Translates g data into m/s^2 data. 
*   @param accel_data_g dat_g
*   @param accel_data_ms2 
*/
accel_data_ms2 translate_accel_g_ms2(accel_data_g dat_g){
    accel_data_ms2 dat; 
    dat.a_x = dat_g.a_x/0.101971; 
    dat.a_y = dat_g.a_y/0.101971; 
    dat.a_z = dat_g.a_z/0.101971; 
    return dat; 
}

/*!
*   @brief Helper function that does the fpu math converting gyro data into raw data. 
*   @param gyro_data_d_s *dat
*   @param imu_data_raw *raw_dat
*   @param float deg_sec
*/
static inline void convert_gyro_raw_d_s_helper(gyro_data_d_s *dat, imu_data_raw *raw_dat, float deg_sec){
    dat->g_x = (float(raw_dat->g_x) / 32768) * deg_sec;
    dat->g_y = (float(raw_dat->g_y) / 32768) * deg_sec;
    dat->g_z = (float(raw_dat->g_z) / 32768) * deg_sec;
}

/*!
*   @brief Takes in raw imu data and spits out gyro data in degrees per second
*   @param imu_data_raw
*   @note gyro_data_d_s
*/
gyro_data_d_s translate_gyro_raw_d_s(imu_data_raw raw_dat){
    gyro_data_d_s dat; 

    switch(gyroscope_range){
    case(GYRO_250_DEGREE_SECCOND):
    convert_gyro_raw_d_s_helper(&dat, &raw_dat, 250);
    break; 

    case(GYRO_500_DEGREE_SECCOND):
    convert_gyro_raw_d_s_helper(&dat, &raw_dat, 500);
    break; 
    
    case(GYRO_1000_DEGREE_SECCOND):
    convert_gyro_raw_d_s_helper(&dat, &raw_dat, 1000);
    break; 
    
    case(GYRO_2000_DEGREE_SECCOND):
    convert_gyro_raw_d_s_helper(&dat, &raw_dat, 2000);
    break; 
    
    default:
    break; 
    }
    return dat; 
}

#endif 
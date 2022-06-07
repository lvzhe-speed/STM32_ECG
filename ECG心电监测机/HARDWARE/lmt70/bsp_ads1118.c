#include "bsp_ads1118.h"
#include "bsp_spi.h"

static void ads1118_cs_out(uint8_t dat)
{
	if(dat)
	{
		ADS118_CS=1;
	}	
	else
	{
		ADS118_CS=0;
	}
}

static uint8_t ads1118_gpio_init(void)
{
	//spi CPOL = low, CPHA = 2
	//init cs pin
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);   //使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //管脚SPI复用 (<STM32F4为>PA5:SPI1_SCK,PA6:SPI1_MISO,PA7:SPI1_MOSI)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         //复用 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	GPIO_Init(GPIOF, &GPIO_InitStructure);        
	
	ads1118_cs_out(1);
	
	return 0;
}

uint8_t ads1118_spi_write_read_byte(uint8_t byte)
{
	return SPI2_ReadWriteByte(byte);
}

static uint16_t ads1118_write_reg_read_dat(uint16_t reg_val)
{
	uint16_t read_dat = 0;
	
	ads1118_cs_out(0);
	
	read_dat |= ads1118_spi_write_read_byte(reg_val >> 8);
	read_dat = (read_dat << 8) | ads1118_spi_write_read_byte(reg_val);
	
	ads1118_cs_out(1);
	
	return read_dat;
}

static void wait_conv_finish(void)
{
	int i = 10000;
	
	while(i--);
}

int ads1118_init(void)
{
	SPI2_Init();
	ads1118_gpio_init();

	return 0;
}

int16_t ads1118_get_voltage(uint8_t adc_channel)
{
	int16_t adc_val = 0;
	
	ads1118_write_reg_read_dat(0xc3eb | ((adc_channel & 0x03) << 12));
	wait_conv_finish();
	
	adc_val |= ads1118_write_reg_read_dat(0xc3eb | ((adc_channel & 0x03) << 12));
	
	return adc_val;
}

float ads1118_get_temperature(void)
{
	int16_t temperature_x4 = 0;
	
	ads1118_write_reg_read_dat(0x8592);
	wait_conv_finish();
	temperature_x4 |= ads1118_write_reg_read_dat(0x8592);
	
	return (temperature_x4 / 4) * 0.03125;
}





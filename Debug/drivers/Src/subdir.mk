################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/Src/queue.c \
../drivers/Src/stm32407xx_gpio_driver.c \
../drivers/Src/stm32f407xx_I2C_driver.c \
../drivers/Src/stm32f407xx_SPI_driver.c \
../drivers/Src/stm32f4xx_USART_driver.c \
../drivers/Src/timebase.c 

OBJS += \
./drivers/Src/queue.o \
./drivers/Src/stm32407xx_gpio_driver.o \
./drivers/Src/stm32f407xx_I2C_driver.o \
./drivers/Src/stm32f407xx_SPI_driver.o \
./drivers/Src/stm32f4xx_USART_driver.o \
./drivers/Src/timebase.o 

C_DEPS += \
./drivers/Src/queue.d \
./drivers/Src/stm32407xx_gpio_driver.d \
./drivers/Src/stm32f407xx_I2C_driver.d \
./drivers/Src/stm32f407xx_SPI_driver.d \
./drivers/Src/stm32f4xx_USART_driver.d \
./drivers/Src/timebase.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/Src/%.o: ../drivers/Src/%.c drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I../Inc -I"E:/Course_armcortexmx/cortex/Secure_Bootloadere/drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-drivers-2f-Src

clean-drivers-2f-Src:
	-$(RM) ./drivers/Src/queue.d ./drivers/Src/queue.o ./drivers/Src/stm32407xx_gpio_driver.d ./drivers/Src/stm32407xx_gpio_driver.o ./drivers/Src/stm32f407xx_I2C_driver.d ./drivers/Src/stm32f407xx_I2C_driver.o ./drivers/Src/stm32f407xx_SPI_driver.d ./drivers/Src/stm32f407xx_SPI_driver.o ./drivers/Src/stm32f4xx_USART_driver.d ./drivers/Src/stm32f4xx_USART_driver.o ./drivers/Src/timebase.d ./drivers/Src/timebase.o

.PHONY: clean-drivers-2f-Src


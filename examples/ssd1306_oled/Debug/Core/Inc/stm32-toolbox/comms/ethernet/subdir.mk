################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Inc/stm32-toolbox/comms/ethernet/Print.cpp \
../Core/Inc/stm32-toolbox/comms/ethernet/Stream.cpp \
../Core/Inc/stm32-toolbox/comms/ethernet/WString.cpp 

OBJS += \
./Core/Inc/stm32-toolbox/comms/ethernet/Print.o \
./Core/Inc/stm32-toolbox/comms/ethernet/Stream.o \
./Core/Inc/stm32-toolbox/comms/ethernet/WString.o 

CPP_DEPS += \
./Core/Inc/stm32-toolbox/comms/ethernet/Print.d \
./Core/Inc/stm32-toolbox/comms/ethernet/Stream.d \
./Core/Inc/stm32-toolbox/comms/ethernet/WString.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/stm32-toolbox/comms/ethernet/%.o Core/Inc/stm32-toolbox/comms/ethernet/%.su: ../Core/Inc/stm32-toolbox/comms/ethernet/%.cpp Core/Inc/stm32-toolbox/comms/ethernet/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-stm32-2d-toolbox-2f-comms-2f-ethernet

clean-Core-2f-Inc-2f-stm32-2d-toolbox-2f-comms-2f-ethernet:
	-$(RM) ./Core/Inc/stm32-toolbox/comms/ethernet/Print.d ./Core/Inc/stm32-toolbox/comms/ethernet/Print.o ./Core/Inc/stm32-toolbox/comms/ethernet/Print.su ./Core/Inc/stm32-toolbox/comms/ethernet/Stream.d ./Core/Inc/stm32-toolbox/comms/ethernet/Stream.o ./Core/Inc/stm32-toolbox/comms/ethernet/Stream.su ./Core/Inc/stm32-toolbox/comms/ethernet/WString.d ./Core/Inc/stm32-toolbox/comms/ethernet/WString.o ./Core/Inc/stm32-toolbox/comms/ethernet/WString.su

.PHONY: clean-Core-2f-Inc-2f-stm32-2d-toolbox-2f-comms-2f-ethernet


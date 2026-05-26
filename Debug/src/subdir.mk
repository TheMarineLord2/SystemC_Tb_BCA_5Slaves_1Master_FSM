################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/EclipsePr.cpp \
../src/TrafficLight.cpp \
../src/Trafficlightcontroller.cpp \
../src/library.cpp 

CPP_DEPS += \
./src/EclipsePr.d \
./src/TrafficLight.d \
./src/Trafficlightcontroller.d \
./src/library.d 

OBJS += \
./src/EclipsePr.o \
./src/TrafficLight.o \
./src/Trafficlightcontroller.o \
./src/library.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/soph/systemc-3.0.2/include -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/EclipsePr.d ./src/EclipsePr.o ./src/TrafficLight.d ./src/TrafficLight.o ./src/Trafficlightcontroller.d ./src/Trafficlightcontroller.o ./src/library.d ./src/library.o

.PHONY: clean-src


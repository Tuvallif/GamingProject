################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Broker.cpp \
../src/BrokerMng.cpp \
../src/Dispatcher.cpp \
../src/Server.cpp \
../src/TCPMSNServer.cpp \
../src/User.cpp \
../src/UsersDataBase.cpp 

OBJS += \
./src/Broker.o \
./src/BrokerMng.o \
./src/Dispatcher.o \
./src/Server.o \
./src/TCPMSNServer.o \
./src/User.o \
./src/UsersDataBase.o 

CPP_DEPS += \
./src/Broker.d \
./src/BrokerMng.d \
./src/Dispatcher.d \
./src/Server.d \
./src/TCPMSNServer.d \
./src/User.d \
./src/UsersDataBase.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/dmanzur/workspace/SocketUtils" -I"/home/dmanzur/workspace/TimeCalculateGame" -I"/home/dmanzur/workspace/GamingServer" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



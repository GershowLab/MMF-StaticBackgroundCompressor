#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Windows
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib ../WindowsBinaries/image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../WindowsBinaries/multistacktester.exe

../WindowsBinaries/multistacktester.exe: ../WindowsBinaries/image_stack_compressor.lib

../WindowsBinaries/multistacktester.exe: ${OBJECTFILES}
	${MKDIR} -p ../WindowsBinaries
	${LINK.cc} -o ../WindowsBinaries/multistacktester ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I.. -I../Necessary\ Libraries\ and\ Includes/CV/headers -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Windows

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../WindowsBinaries/multistacktester.exe

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Windows clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

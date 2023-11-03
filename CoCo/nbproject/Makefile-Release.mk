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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/AllItemsBag.o \
	${OBJECTDIR}/src/BitFlip.o \
	${OBJECTDIR}/src/BoundaryItemsBag.o \
	${OBJECTDIR}/src/CLKSolver.o \
	${OBJECTDIR}/src/ItemsBag.o \
	${OBJECTDIR}/src/PGCH.o \
	${OBJECTDIR}/src/ProBitFlip.o \
	${OBJECTDIR}/src/ProTwoOPT.o \
	${OBJECTDIR}/src/TTPInstance.o \
	${OBJECTDIR}/src/TTPSolution.o \
	${OBJECTDIR}/src/TTPSolver.o \
	${OBJECTDIR}/src/TwoOPT.o


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
LDLIBSOPTIONS=lib/concorde.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/coco

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/coco: lib/concorde.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/coco: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/coco ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/AllItemsBag.o: src/AllItemsBag.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AllItemsBag.o src/AllItemsBag.cpp

${OBJECTDIR}/src/BitFlip.o: src/BitFlip.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BitFlip.o src/BitFlip.cpp

${OBJECTDIR}/src/BoundaryItemsBag.o: src/BoundaryItemsBag.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BoundaryItemsBag.o src/BoundaryItemsBag.cpp

${OBJECTDIR}/src/CLKSolver.o: src/CLKSolver.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CLKSolver.o src/CLKSolver.cpp

${OBJECTDIR}/src/ItemsBag.o: src/ItemsBag.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ItemsBag.o src/ItemsBag.cpp

${OBJECTDIR}/src/PGCH.o: src/PGCH.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PGCH.o src/PGCH.cpp

${OBJECTDIR}/src/ProBitFlip.o: src/ProBitFlip.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProBitFlip.o src/ProBitFlip.cpp

${OBJECTDIR}/src/ProTwoOPT.o: src/ProTwoOPT.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProTwoOPT.o src/ProTwoOPT.cpp

${OBJECTDIR}/src/TTPInstance.o: src/TTPInstance.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TTPInstance.o src/TTPInstance.cpp

${OBJECTDIR}/src/TTPSolution.o: src/TTPSolution.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TTPSolution.o src/TTPSolution.cpp

${OBJECTDIR}/src/TTPSolver.o: src/TTPSolver.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TTPSolver.o src/TTPSolver.cpp

${OBJECTDIR}/src/TwoOPT.o: src/TwoOPT.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TwoOPT.o src/TwoOPT.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc

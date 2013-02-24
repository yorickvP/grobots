# Monolithic Makefile for Grobots headless mode
# Updated by Hector, 2012-12-8
# General Automake options
AUTOMAKE_OPTIONS = foreign subdir-objects

# Set the parameters to compile executable
bin_PROGRAMS = grobots
grobots_SOURCES = GBHeadless.cpp support/GBColor.cpp \
support/GBErrors.cpp support/GBDeletionReporter.cpp \
support/GBFinePoint.cpp support/GBGraphics.cpp \
support/GBMilliseconds.cpp \
support/GBModel.cpp support/GBNumber.cpp support/GBRandomState.cpp \
support/GBStringUtilities.cpp \
simulation/GBDecorations.cpp simulation/GBFood.cpp \
simulation/GBHardwareState.cpp simulation/GBMessages.cpp \
simulation/GBObject.cpp simulation/GBObjectWorld.cpp \
simulation/GBRobot.cpp simulation/GBSensorShot.cpp \
simulation/GBShot.cpp simulation/GBWorld.cpp \
sides/GBHardwareSpec.cpp sides/GBRobotType.cpp sides/GBScores.cpp \
sides/GBSide.cpp sides/GBSideReader.cpp \
brains/GBBrain.cpp brains/GBBrainSpec.cpp brains/GBStackBrain.cpp \
brains/GBStackBrainOpcodes.cpp brains/GBStackBrainPrimitives.cpp \
brains/GBStackBrainSpec.cpp

# Include all the basic GB header libraries and set other flags
CXXFLAGS = -I$(srcdir)/support -I$(srcdir)/simulation \
-I$(srcdir)/sides -I$(srcdir)/brains -I$(srcdir)/views -DHEADLESS -O

# Data files
dist_doc_DATA = Readme

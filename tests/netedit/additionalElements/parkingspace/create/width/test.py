#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeElement("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# select space
netedit.changeElement("space")

# set invalid width (dummy)
netedit.selectAdditionalChild(8, 0)
netedit.changeDefaultValue(3, "dummyWidth")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set invalid width (empty)
netedit.changeDefaultValue(3, "")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set invalid width (negative)
netedit.changeDefaultValue(3, "-3")

# try to create area
netedit.leftClick(referencePosition, 280, 100)

# set valid width
netedit.changeDefaultValue(3, "2.5")

# create area
netedit.leftClick(referencePosition, 280, 100)

# Check undo redo
netedit.undo(referencePosition, 2)
netedit.redo(referencePosition, 2)

# save additionals
netedit.saveAdditionals(referencePosition)

# save network
netedit.saveNetwork(referencePosition)

# quit netedit
netedit.quit(neteditProcess)

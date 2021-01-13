/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEMoveFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// The Widget for move elements
/****************************************************************************/
#include <config.h>

#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMoveFrame::ChangeZInSelection) ChangeZInSelectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNEMoveFrame::ChangeZInSelection::onCmdChangeZMode),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_APPLY,          GNEMoveFrame::ChangeZInSelection::onCmdApplyZ),
};

// Object implementation
FXIMPLEMENT(GNEMoveFrame::ChangeZInSelection,   FXGroupBox, ChangeZInSelectionMap,  ARRAYNUMBER(ChangeZInSelectionMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMoveFrame::ChangeZInSelection - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::ChangeZInSelection::ChangeZInSelection(GNEMoveFrame* moveFrameParent) :
    FXGroupBox(moveFrameParent->myContentFrame, "Change Z in selection", GUIDesignGroupBoxFrame),
    myMoveFrameParent(moveFrameParent) {
    // create horizontal frame
    FXHorizontalFrame* myZValueFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create elements for Z value
    new FXLabel(myZValueFrame, "Z value", 0, GUIDesignLabelAttribute);
    myZValueTextField = new FXTextField(myZValueFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myZValueTextField->setText("0");
    // Create all options buttons
    myAbsoluteValue = new FXRadioButton(this, "Absolute value\t\tSet Z value as absolute",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    myRelativeValue = new FXRadioButton(this, "Relative value\t\tSet Z value as relative",
        this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create apply button
    new FXButton(this,
        "Apply Z value\t\tApply Z value to all selected junctions",
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_APPLY, GUIDesignButton);
    // set absolute value as default
    myAbsoluteValue->setCheck(true);
    // set info label
    myInfoLabel = new FXLabel(this, "", nullptr, GUIDesignLabelFrameInformation);
}


GNEMoveFrame::ChangeZInSelection::~ChangeZInSelection() {}


void 
GNEMoveFrame::ChangeZInSelection::showChangeZInSelection() {
    // update info label
    updateInfoLabel();
    // show modul
    show();
}


void
GNEMoveFrame::ChangeZInSelection::hideChangeZInSelection() {
    // hide modul
    hide();
}


long 
GNEMoveFrame::ChangeZInSelection::onCmdChangeZValue(FXObject*, FXSelector, void*) {
    // nothing to do
    return 1;
}


long 
GNEMoveFrame::ChangeZInSelection::onCmdChangeZMode(FXObject* obj, FXSelector, void*) {
    if (obj == myAbsoluteValue) {
        myAbsoluteValue->setCheck(true);
        myRelativeValue->setCheck(false);
    } else {
        myAbsoluteValue->setCheck(false);
        myRelativeValue->setCheck(true);
    }
    return 1;
}


long
GNEMoveFrame::ChangeZInSelection::onCmdApplyZ(FXObject*, FXSelector, void*) {
    // get undo-list
    auto undoList = myMoveFrameParent->getViewNet()->getUndoList();
    // get value
    const double zValue = GNEAttributeCarrier::parse<double>(myZValueTextField->getText().text());
    // get junctions
    const auto junctions = myMoveFrameParent->getViewNet()->getNet()->retrieveJunctions(true);
    // get selected edges
    const auto edges = myMoveFrameParent->getViewNet()->getNet()->retrieveEdges(true);
    // begin undo-redo 
    myMoveFrameParent->getViewNet()->getUndoList()->p_begin("change Z values in selection");
    // iterate over junctions
    for (const auto& junction : junctions) {
        if (junction->getNBNode()->hasCustomShape()) {
            // get junction position
            PositionVector junctionShape = junction->getNBNode()->getShape();
            // modify z Value depending of absolute/relative
            for (auto &shapePos : junctionShape) {
                if (myAbsoluteValue->getCheck() == TRUE) {
                    shapePos.setz(zValue);
                } else {
                    shapePos.add(Position(0, 0, zValue));
                }
            }
            // set new position again
            junction->setAttribute(SUMO_ATTR_SHAPE, toString(junctionShape), undoList);
        }
        // get junction position
        Position junctionPos = junction->getNBNode()->getPosition();
        // modify z Value depending of absolute/relative
        if (myAbsoluteValue->getCheck() == TRUE) {
            junctionPos.setz(zValue);
        } else {
            junctionPos.add(Position(0, 0, zValue));
        }
        // set new position again
        junction->setAttribute(SUMO_ATTR_POSITION, toString(junctionPos), undoList);
    }
    // iterate over edges
    for (const auto& edge : edges) {
        // get edge geometry
        PositionVector edgeShape = edge->getNBEdge()->getInnerGeometry();
        // get first and last position
        Position shapeStart = edge->getNBEdge()->getGeometry().front();
        Position shapeEnd = edge->getNBEdge()->getGeometry().back();
        // modify z Value depending of absolute/relative
        for (auto& shapePos : edgeShape) {
            if (myAbsoluteValue->getCheck() == TRUE) {
                shapePos.setz(zValue);
            } else {
                shapePos.add(Position(0, 0, zValue));
            }
        }
        // modify begin an end positions
        if (myAbsoluteValue->getCheck() == TRUE) {
            shapeStart.setz(zValue);
            shapeEnd.setz(zValue);
        } else {
            shapeStart.add(Position(0, 0, zValue));
            shapeEnd.add(Position(0, 0, zValue));
        }
        // set new shape again
        if (edgeShape.size() > 0) {
            edge->setAttribute(SUMO_ATTR_SHAPE, toString(edgeShape), undoList);
        }
        // set new start and end positions
        if ((edge->getAttribute(GNE_ATTR_SHAPE_START).size() > 0) && 
            (shapeStart.distanceSquaredTo2D(edge->getParentJunctions().front()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_START, toString(shapeStart), undoList);
        }
        if ((edge->getAttribute(GNE_ATTR_SHAPE_END).size() > 0) && 
            (shapeEnd.distanceSquaredTo2D(edge->getParentJunctions().back()->getNBNode()->getPosition()) < 2)) {
            edge->setAttribute(GNE_ATTR_SHAPE_END, toString(shapeEnd), undoList);
        }
    }
    // end undo-redo
    myMoveFrameParent->getViewNet()->getUndoList()->p_end();
    // update info label
    updateInfoLabel();
    return 1;
}


void 
GNEMoveFrame::ChangeZInSelection::updateInfoLabel() {
    // get junctions
    const auto junctions = myMoveFrameParent->getViewNet()->getNet()->retrieveJunctions(true);
    // get selected edges
    const auto edges = myMoveFrameParent->getViewNet()->getNet()->retrieveEdges(true);
    if (junctions.size() > 0) {
        // declare minimum, maximun and average
        double junctionMinimum = junctions.front()->getNBNode()->getPosition().z();
        double junctionMaximun = junctions.front()->getNBNode()->getPosition().z();
        double junctionAverage = 0;
        // iterate over junctions
        for (const auto& junction : junctions) {
            // get z
            const double z = junction->getNBNode()->getPosition().z();
            // check min
            if (z < junctionMinimum) {
                junctionMinimum = z;
            }
            // check max
            if (z > junctionMaximun) {
                junctionMaximun = z;
            }
            // update average
            junctionAverage += z;
        }
        // update average
        junctionAverage = 100 * junctionAverage / (double)junctions.size();
        junctionAverage = floor(junctionAverage);
        junctionAverage *= 0.01;
        // set label string
        const std::string labelStr = 
            "- Junction minimum Z: " + toString(junctionMinimum) + "\n" +
            "- Junction maximum Z: " + toString(junctionMaximun) + "\n" +
            "- Junction average Z: " + toString(junctionAverage);
        // update info label
        myInfoLabel->setText(labelStr.c_str());
    }
}

// ---------------------------------------------------------------------------
// GNEMoveFrame - methods
// ---------------------------------------------------------------------------

GNEMoveFrame::GNEMoveFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Move") {
    // create change z selection
    myChangeZInSelection = new ChangeZInSelection(this);
}


GNEMoveFrame::~GNEMoveFrame() {}


void
GNEMoveFrame::processClick(const Position& /*clickedPosition*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderCursor*/,
                           const GNEViewNetHelper::ObjectsUnderCursor& /*objectsUnderGrippedCursor*/) {
    // currently unused
}


void
GNEMoveFrame::show() {
    // check if there are junctions selected
    if ((myViewNet->getNet()->retrieveJunctions(true).size() > 0) || 
        (myViewNet->getNet()->retrieveEdges(true).size() > 0)) {
        myChangeZInSelection->showChangeZInSelection();
    } else {
        myChangeZInSelection->hideChangeZInSelection();
    }
    // show
    GNEFrame::show();
    // recalc and update
    update();
}


void
GNEMoveFrame::hide() {
    // hide frame
    GNEFrame::hide();
}

/****************************************************************************/

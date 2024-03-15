// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora, Radu Serban
// =============================================================================

#include "chrono/core/ChGlobal.h"
#include "chrono/physics/ChLink.h"
#include "chrono/physics/ChSystem.h"

namespace chrono {

// Register into the object factory, to enable run-time dynamic creation and persistence
// CH_FACTORY_REGISTER(ChLink)   // NO! abstract class!

ChLink::ChLink(const ChLink& other) : ChLinkBase(other) {
    m_body1 = nullptr;
    m_body2 = nullptr;

    react_force = other.react_force;
    react_torque = other.react_torque;
}

// -----------------------------------------------------------------------------

ChFramed ChLink::GetFrame1Abs() const {
    return GetFrame1Rel() >> *m_body1;
}

ChFramed ChLink::GetFrame2Abs() const {
    return GetFrame2Rel() >> *m_body2;
}

// -----------------------------------------------------------------------------

// The default ChLink implementation assumes that react_force and react_torque represent the reaction wrench on the 2nd
// body, expressed in the link frame 2. A derived class may interpret react_force and react_torque differently, in which
// case it must override GetReaction1() and GetReaction2().

ChWrenchd ChLink::GetReaction1() const {
    auto w1_abs = GetFrame2Abs().TransformWrenchLocalToParent({-react_force, -react_torque});
    return GetFrame1Abs().TransformWrenchParentToLocal(w1_abs);
}

ChWrenchd ChLink::GetReaction2() const {
    return {react_force, react_torque};
}

// -----------------------------------------------------------------------------

ChVector3d ChLink::GetReactForce1() const {
    return GetFrame1Rel().TransformDirectionParentToLocal(
        GetBody2()->TransformDirectionLocalToParent(GetReactForceBody2()));
}

ChVector3d ChLink::GetReactTorque1() const {
    auto posF2_from_F1_inF1 =
        GetFrame1Rel().TransformPointParentToLocal(GetBody2()->TransformPointLocalToParent(GetFrame2Rel().GetPos()));
    auto torque1_dueto_force2_inF1 = Vcross(posF2_from_F1_inF1, GetReactForce1());
    auto torque1_dueto_torque2_inF1 = GetFrame1Rel().TransformDirectionParentToLocal(
        GetBody2()->TransformDirectionLocalToParent(GetFrame2Rel().TransformDirectionLocalToParent(GetReactTorque2())));
    return torque1_dueto_torque2_inF1 + torque1_dueto_force2_inF1;
}

ChVector3d ChLink::GetReactForce2() const {
    return react_force;
}

ChVector3d ChLink::GetReactTorque2() const {
    return react_torque;
}

// -----------------------------------------------------------------------------

ChVector3d ChLink::GetReactForceBody1() const {
    return GetBody1()->TransformDirectionParentToLocal(
        GetBody2()->TransformDirectionLocalToParent(GetReactForceBody2()));
}

ChVector3d ChLink::GetReactTorqueBody1() const {
    auto posF2_from_B1_inB1 =
        GetBody1()->TransformPointParentToLocal(GetBody2()->TransformPointLocalToParent(GetFrame2Rel().GetPos()));
    auto torque1_dueto_force2_inB1 = Vcross(posF2_from_B1_inB1, GetReactForceBody1());
    auto torque1_dueto_torque2_inB1 = GetBody1()->TransformDirectionParentToLocal(
        GetBody2()->TransformDirectionLocalToParent(GetFrame2Rel().TransformDirectionLocalToParent(GetReactTorque2())));
    return torque1_dueto_torque2_inB1 + torque1_dueto_force2_inB1;
}

ChVector3d ChLink::GetReactForceBody2() const {
    return GetFrame2Rel().TransformDirectionLocalToParent(GetReactForce2());
}

ChVector3d ChLink::GetReactTorqueBody2() const {
    auto posF2_from_B2_inB2 = GetFrame2Rel().GetPos();
    auto torque2_dueto_force2_inB2 = Vcross(posF2_from_B2_inB2, GetReactForceBody2());
    auto torque2_dueto_torque2_inB2 =
        GetBody2()->TransformDirectionLocalToParent(GetFrame2Rel().TransformDirectionLocalToParent(GetReactTorque2()));
    return torque2_dueto_torque2_inB2 + torque2_dueto_force2_inB2;
}

/// Get reaction torque, expressed on body 2 frame.

void ChLink::UpdateTime(double time) {
    ChTime = time;
}

void ChLink::Update(double time, bool update_assets) {
    UpdateTime(time);

    // Update assets
    ChPhysicsItem::Update(ChTime, update_assets);
}

void ChLink::Update(bool update_assets) {
    Update(ChTime, update_assets);  // use the same time
}

void ChLink::ArchiveOut(ChArchiveOut& archive_out) {
    // version number
    archive_out.VersionWrite<ChLink>();

    // serialize parent class
    ChLinkBase::ArchiveOut(archive_out);

    // serialize all member data:
    archive_out << CHNVP(m_body1);
    archive_out << CHNVP(m_body2);
}

void ChLink::ArchiveIn(ChArchiveIn& archive_in) {
    // version number
    /*int version =*/archive_in.VersionRead<ChLink>();

    // deserialize parent class
    ChLinkBase::ArchiveIn(archive_in);

    // deserialize all member data:
    archive_in >> CHNVP(m_body1);
    archive_in >> CHNVP(m_body2);
}

}  // end namespace chrono

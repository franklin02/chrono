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
// Authors: Radu Serban
// =============================================================================
//
// Double-pin track shoe with central guide constructed with data from file
// (JSON format).
//
// =============================================================================

#ifndef TRACK_SHOE_DOUBLE_PIN_H
#define TRACK_SHOE_DOUBLE_PIN_H

#include "chrono_vehicle/ChApiVehicle.h"
#include "chrono_vehicle/tracked_vehicle/track_shoe/ChTrackShoeDoublePin.h"

#include "chrono_thirdparty/rapidjson/document.h"

namespace chrono {
namespace vehicle {

/// @addtogroup vehicle_tracked_shoe
/// @{

/// Double-pin track shoe with central guide constructed with data from file (JSON format).
class CH_VEHICLE_API TrackShoeDoublePin : public ChTrackShoeDoublePin {
  public:
    TrackShoeDoublePin(const std::string& filename);
    TrackShoeDoublePin(const rapidjson::Document& d);
    ~TrackShoeDoublePin() {}

    virtual GuidePinType GetType() const final override { return GuidePinType::CENTRAL_PIN; }

    /// Return the height of the track shoe.
    virtual double GetHeight() const override { return m_shoe_height; }

    /// Return the mass of the shoe body.
    virtual double GetShoeMass() const override { return m_shoe_mass; }
    /// Return the moments of inertia of the shoe body.
    virtual const ChVector<>& GetShoeInertia() const override { return m_shoe_inertia; }
    /// Return shoe length (distance between pins).
    virtual double GetShoeLength() const override { return m_shoe_length; }
    /// Return shoe width (separation between connectors).
    virtual double GetShoeWidth() const override { return m_shoe_width; }

    /// Return the mass of a connector body.
    virtual double GetConnectorMass() const override { return m_connector_mass; }
    /// Return the moments of inertia of a connector body.
    virtual const ChVector<>& GetConnectorInertia() const override { return m_connector_inertia; }
    /// Return the length of a connector body.
    virtual double GetConnectorLength() const override { return m_connector_length; }
    /// Return the radius of a connector body.
    virtual double GetConnectorRadius() const override { return m_connector_radius; }
    /// Return the width of a connector body (visualization only).
    virtual double GetConnectorWidth() const override { return m_connector_width; }

  private:
    virtual void Create(const rapidjson::Document& d) override;

    /// Create the contact materials for the shoe collision shapes.
    void CreateShoeContactMaterials(ChContactMethod contact_method) override;

    /// Create the contact material for the connectors.
    void CreateConnectorContactMaterial(ChContactMethod contact_method) override;

    /// Add visualization assets for the idler subsystem.
    virtual void AddVisualizationAssets(VisualizationType vis) override;

    double m_shoe_mass;
    ChVector<> m_shoe_inertia;
    double m_shoe_length;
    double m_shoe_width;
    double m_shoe_height;

    double m_connector_mass;
    ChVector<> m_connector_inertia;
    double m_connector_radius;
    double m_connector_length;
    double m_connector_width;

    bool m_has_mesh;
    std::string m_meshName;
    std::string m_meshFile;

    struct MatInfo {
        float mu;
        float cr;
        float Y;
        float nu;
        float kn;
        float gn;
        float kt;
        float gt;
    };
    std::vector<MatInfo> m_shoe_mat_info;
    MatInfo m_cyl_mat_info;
};

/// @} vehicle_tracked_shoe

}  // end namespace vehicle
}  // end namespace chrono

#endif

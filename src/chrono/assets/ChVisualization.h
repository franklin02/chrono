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

#ifndef CHVISUALIZATION_H
#define CHVISUALIZATION_H

#include "chrono/assets/ChAsset.h"
#include "chrono/assets/ChColor.h"
#include "chrono/core/ChMath.h"

#include "chrono/assets/ChVisualMaterial.h"

namespace chrono {

/// Base class for a visualization asset for rendering (run-time or post processing).
/// Encapsulates basic information about the asset position, materials, and visibility.
class ChApi ChVisualization : public ChAsset {
  public:
    virtual ~ChVisualization() {}

    /// Set this visualization asset as visible.
    void SetVisible(bool mv) { visible = mv; }

    /// Return true if the asset is set as visible.
    bool IsVisible() const { return visible; }

    /// Set the diffuse color of the default material (default: white).
    void SetColor(const ChColor& col);

    /// Return the diffuse color of the default material.
    ChColor GetColor() const;

    /// Set the fading level, a value in [0,1] (default: 0).
    /// If fading = 0, the surface is completely opaque.
    /// If fading = 1, the surface is completely transparent.
    void SetFading(const float mc) { fading = mc; }

    /// Get the fading level.
    float GetFading() const { return fading; }

    /// Set this visualization asset as static (default: false).
    /// Set to true to indicate that the asset never changes and therefore does not require updates
    /// (e.g. for a non-deformable triangular mesh).
    /// A particular visualization system may take advantage of this setting to accelerate rendering.
    void SetStatic(bool val) { is_static = val; }

    /// Return true if the visualization asset is marked as static.
    bool IsStatic() const { return is_static; }

    /// Add a visualization material and return its index in the list of materials.
    int AddMaterial(std::shared_ptr<ChVisualMaterial> material);

    /// Get the list of visualization materials.
    std::vector<std::shared_ptr<ChVisualMaterial>>& GetMaterials() { return material_list; }

    /// Get the specified material in the list.
    std::shared_ptr<ChVisualMaterial> GetMaterial(int i) { return material_list[i]; }

    /// Get the number of visualizatin materials.
    int GetNumMaterials() const { return (int)material_list.size(); }

    ChVector<> Pos;    ///< asset position
    ChMatrix33<> Rot;  ///< asset orientation

  protected:
    ChVisualization();

    /// Method to allow serialization of transient data to archives.
    virtual void ArchiveOUT(ChArchiveOut& marchive) override;

    /// Method to allow de-serialization of transient data from archives.
    virtual void ArchiveIN(ChArchiveIn& marchive) override;

    bool visible;
    bool is_static;
    float fading;

    std::shared_ptr<ChVisualMaterial> default_mat;                 ///< default material
    std::vector<std::shared_ptr<ChVisualMaterial>> material_list;  ///< list of visualization materials

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

CH_CLASS_VERSION(ChVisualization, 0)

}  // end namespace chrono

#endif

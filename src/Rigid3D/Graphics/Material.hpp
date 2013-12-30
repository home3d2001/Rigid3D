/**
 * @brief MaterialProperties
 *
 * @author Dustin Biser
 */

#ifndef RIGID3D_MATERIAL_HPP_
#define RIGID3D_MATERIAL_HPP_

#include <Rigid3D/Common/Settings.hpp>

namespace Rigid3D {

    /**
     * Material lighting properties for emission, ambient, diffuse, and specular lighting.
     */
    struct Material {
        vec3 emission; // Emission light intensity from material for each RGB component.
        vec3 Ka; // Coefficients of ambient reflectivity for each RGB component.
        vec3 Kd; // Coefficients of diffuse reflectivity for each RGB component.
        float Ks; // Coefficient of specular reflectivity, uniform across each RGB component.
        float shininessFactor; // Specular shininess factor.
    };

}


#endif /* RIGID3D_MATERIAL_HPP_ */

#include <Rigid3D/Common/GlmOutStream.hpp> // For printing glm types to output streams for failed tests.

#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#include <vector>
using std::vector;

#include <boost/math/special_functions/next.hpp>
using boost::math::float_distance;

#include <cmath>

#include <limits>

namespace TestUtils {
    namespace predicates {

    // Floating point numbers must be less than this distance apart to be considered equal.
    const int ulpTolerance = 5;

    const float epsilon = std::numeric_limits<float>::epsilon();

    inline bool array_eq(const float * expected, const float * actual, size_t length) {
        for(size_t i = 0; i < length; i++) {
            if (!(std::abs((int)float_distance(expected[i], actual[i])) >= ulpTolerance)) {
                return false;
            }
        }

        return true;
    }
    //-----------------------------------------------------------------------------------
    inline bool float_eq_ulp(float a, float b) {
        return (std::abs((int)float_distance(a, b)) < ulpTolerance);
    }

    //-----------------------------------------------------------------------------------
    inline bool float_neq_ulp(float a, float b) {
        return !float_eq_ulp(a, b);
    }

    //-----------------------------------------------------------------------------------
    /**
     * Relative epsilon float comparison.
     */
    inline bool float_eq(float a, float b) {
        return (std::fabs(a - b) <= epsilon * (std::fabs(a) + std::fabs(b) + 1.0f));
    }

    //-----------------------------------------------------------------------------------
    /**
     * Relative epsilon float comparison.
     */
    inline bool float_neq(float a, float b) {
        return !float_eq(a, b);
    }

    //-----------------------------------------------------------------------------------
    inline bool vec3_eq(const vec3 & expected, const vec3 & actual) {
        return ( float_eq(expected.x, actual.x) &&
                 float_eq(expected.y, actual.y) &&
                 float_eq(expected.z, actual.z) );
    }

    //-----------------------------------------------------------------------------------
    inline bool vec3_neq(const vec3 & expected, const vec3 & actual) {
        return !vec3_eq(expected, actual);
    }

    //-----------------------------------------------------------------------------------
    inline bool vec4_eq(const vec4 & expected, const vec4 & actual) {
        return ( float_eq(expected.x, actual.x) &&
                 float_eq(expected.y, actual.y) &&
                 float_eq(expected.z, actual.z) &&
                 float_eq(expected.w, actual.w) );
    }

    //-----------------------------------------------------------------------------------
    inline bool vec4_neq(const vec4 & expected, const vec4 & actual) {
        return !vec4_eq(expected, actual);
    }

    //-----------------------------------------------------------------------------------
    inline bool mat3_eq(const mat3 & expected, const mat3 & actual) {
        return (vec3_eq(expected[0], actual[0]) &&  // compare column one.
                vec3_eq(expected[1], actual[1]) &&  // compare column two.
                vec3_eq(expected[2], actual[2]) );  // compare column three.

    }

    //-----------------------------------------------------------------------------------
    inline bool mat3_neq(const mat3 & expected, const mat3 & actual) {
        return (vec3_neq(expected[0], actual[0]) ||  // compare column one.
                vec3_neq(expected[1], actual[1]) ||  // compare column two.
                vec3_neq(expected[2], actual[2]) );  // compare column three.

    }

    //-----------------------------------------------------------------------------------
    inline bool mat4_eq(const mat4 & expected, const mat4 & actual) {
        return (vec4_eq(expected[0], actual[0]) &&  // compare column one.
                vec4_eq(expected[1], actual[1]) &&  // compare column two.
                vec4_eq(expected[2], actual[2]) &&  // compare column three.
                vec4_eq(expected[3], actual[3]) );  // compare column four.

    }

    //-----------------------------------------------------------------------------------
    inline bool mat4_neq(const mat4 & expected, const mat4 & actual) {
        return (vec4_neq(expected[0], actual[0]) ||  // compare column one.
                vec4_neq(expected[1], actual[1]) ||  // compare column two.
                vec4_neq(expected[2], actual[2]) ||  // compare column three.
                vec4_neq(expected[3], actual[3]) );  // compare column four.

    }

    //-----------------------------------------------------------------------------------
    inline bool vectors_eq(const vector<vec3> & expected, const vector<vec3> & actual) {
        if (expected.size() != actual.size()) {
            return false;
        }

        for(size_t i = 0; i < expected.size(); i++) {
            if (!vec3_eq(expected.at(i), actual.at(i))) {
                return false;
            }
        }

        return true;
    }

}} // end namespace TestUtils::predicates

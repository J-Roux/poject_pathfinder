using UnityEngine;
using System.Collections;

namespace Pathfinder.Simulation.Helper {

    /// <summary>
    /// Defines angle-unit extension methods.
    /// </summary>
    public static class UnitExtensions {
        public static float Radians(this float deg) {
            return deg * Mathf.PI / 180f;
        }
        
        public static float Degrees(this float deg) {
            return deg;
        }
    }

}
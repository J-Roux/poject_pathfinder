using UnityEngine;
using System.Collections;

using Pathfinder.Simulation.Helper;

namespace Pathfinder.Simulation {

    public class DroneController : MonoBehaviour {
        
        /// <summary>
        /// Sets the rotations.
        /// </summary>
        /// <param name="rotations">Vector3 of rotations in degrees (roll, pitch, yaw).</param>
        public void SetRotations(Vector3 rotations) {
            transform.rotation = Quaternion.identity;

            AddRoll(rotations.x.Degrees());
            AddPitch(rotations.y.Degrees());
            AddYaw(rotations.z.Degrees());
        }
        
        /// <summary>
        /// Rolls the craft.
        /// </summary>
        /// <param name="roll">Roll rotation in degrees.</param>
        public void AddRoll(float roll) {
            transform.Rotate(0f, 0f, -roll.Degrees());
        }
        
        /// <summary>
        /// Pitches the craft.
        /// </summary>
        /// <param name="pitch">Pitch rotation in radians.</param>
        public void AddPitch(float pitch) {
            transform.Rotate(-pitch.Degrees(), 0f, 0f);
        }
        
        /// <summary>
        /// Adds yaw.
        /// </summary>
        /// <param name="yaw">Yaw rotation in radians.</param>
        public void AddYaw(float yaw) {
            transform.Rotate(0f, yaw.Degrees(), 0f);
        }
    }

}

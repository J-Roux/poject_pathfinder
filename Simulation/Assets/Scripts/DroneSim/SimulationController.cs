using UnityEngine;

namespace Pathfinder.Simulation {

    public class SimulationController : MonoBehaviour {

        public DroneController CurrentDrone;
        public Vector3 ControlsSensitivity = 20f * Vector3.one;

        private void HandleKeyboardControls() {
            if (Input.GetKey("d"))             {
                CurrentDrone.AddYaw(ControlsSensitivity.z * Time.deltaTime);
            } else if (Input.GetKey("a"))             {
                CurrentDrone.AddYaw(-ControlsSensitivity.z * Time.deltaTime);
            }
            
            if (Input.GetKey("w")) {
                CurrentDrone.AddPitch(ControlsSensitivity.y * Time.deltaTime);
            } else if (Input.GetKey("s")) {
                CurrentDrone.AddPitch(-ControlsSensitivity.y * Time.deltaTime);
            }
            
            if (Input.GetKey("e")) {
                CurrentDrone.AddRoll(ControlsSensitivity.x * Time.deltaTime);
            } else if (Input.GetKey("q")) {
                CurrentDrone.AddRoll(-ControlsSensitivity.x * Time.deltaTime);
            }
        }

        private void HandleMouseControls() {
            if (Input.GetMouseButton(1)) {
                Camera.main.transform.RotateAround(CurrentDrone.transform.position,
                                                      Vector3.up,
                                                      Input.GetAxis("Mouse X"));
            }
        }

        private void Update() {
            HandleKeyboardControls();
            HandleMouseControls();
        }
    }

}
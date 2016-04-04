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

			if (Input.GetKey("i")) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.forward);
			} else if (Input.GetKey("k")) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.forward * -1f);
			}

			if (Input.GetKey(KeyCode.Space)) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.up);
			} else if (Input.GetKey(KeyCode.LeftShift)) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.up * -1f);
			}

			if (Input.GetKey("l")) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.right);
			} else if (Input.GetKey("j")) {
				CurrentDrone.SetAcceleration(CurrentDrone.transform.right * -1f);
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
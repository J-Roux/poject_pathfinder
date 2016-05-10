using UnityEngine;
using System.Collections;

using Pathfinder.Simulation.Helper;

namespace Pathfinder.Simulation {

    [RequireComponent(typeof(DroneController))]
    public class TcpDroneUpdater : MonoBehaviour {
        public string Hostname;
        public short Port;

        private DroneController drone;
        private TcpClient client;

        private void Awake() {
            drone = GetComponent<DroneController>();
        }


    }

}
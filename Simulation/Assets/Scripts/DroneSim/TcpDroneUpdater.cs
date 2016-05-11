using UnityEngine;
using System.Collections;

using Pathfinder.Simulation.Helper;
using System.Net.Sockets;

namespace Pathfinder.Simulation {

    public struct TcpUpdatePacket {
        public Vector3 RollPitchYaw;
        public Vector3 Acceleration;
        public float Heading;
    }

    [RequireComponent(typeof(DroneController))]
    public class TcpDroneUpdater : MonoBehaviour {
        public string Hostname;
        public ushort Port;

        public const int PACKET_LENGTH = 1024; 

        private DroneController drone;
        private TcpClient client;
        private NetworkStream dataStream;

        private byte[] dataBuffer = new byte[PACKET_LENGTH];
        private int alreadyRead;

        private void Awake() {
            drone = GetComponent<DroneController>();
            client = new TcpClient(Hostname, Port);
            dataStream = client.GetStream();
        }

        private void ProcessPacket() {
            var updatePacket = new TcpUpdatePacket();
            // Perform some shaman magic here and fill out the packet class.

            drone.SetAcceleration(updatePacket.Acceleration);
            drone.SetRotations(updatePacket.RollPitchYaw);
            // Do nothing with heading.
        }

        private void Update() {
            if (!dataStream.CanRead)
                return;

            if (dataStream.DataAvailable)
            {
                alreadyRead += dataStream.Read(dataBuffer, alreadyRead, PACKET_LENGTH - alreadyRead);
            }

            if (alreadyRead == PACKET_LENGTH)
            {
                ProcessPacket();
                alreadyRead = 0;
            }
        }
    }

}
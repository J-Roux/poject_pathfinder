using UnityEngine;
using System.Collections;
using Pathfinder.Simulation.Helper;
using System.Net.Sockets;
using System;
namespace Pathfinder.Simulation
{

    public struct TcpUpdatePacket
    {
        public Vector3 RollPitchYaw;
        public Vector3 Acceleration;
        public float Heading;
    }

    [RequireComponent(typeof(DroneController))]
    public class TcpDroneUpdater : MonoBehaviour
    {
        public string Hostname;
        public ushort Port;

        public const int PACKET_LENGTH = 128;

        private DroneController drone;
        private TcpClient client;
        private NetworkStream dataStream;
        private byte[] dataBuffer = new byte[PACKET_LENGTH];
        private int alreadyRead;
        private String data;
        private bool first;
        private TcpUpdatePacket updatePacket;
        private const int step = 2;
        private void Awake()
        {
            drone = GetComponent<DroneController>();
            client = new TcpClient(Hostname, Port);
            dataStream = client.GetStream();
            updatePacket = new TcpUpdatePacket();
            first = true;
        }

        private void ProcessPacket()
        {
          
            data = System.Text.Encoding.Default.GetString(dataBuffer);
            var result = data.Split('#');
            if (result.Length > 3)
            {
                result = result[1].Split('\n');
                if (result.Length == 4)
                {
                    var tempVector = new Vector3();
                    var rollPitchYaw = result[0].Replace("ypr: ", "").Split(' ');
                    if (rollPitchYaw.Length == 3)
                    {
                     
                        float.TryParse(rollPitchYaw[0], out tempVector.z);
                        float.TryParse(rollPitchYaw[1], out tempVector.y);
                        float.TryParse(rollPitchYaw[2], out tempVector.x);
                        if (first)
                        {
                            updatePacket.RollPitchYaw = tempVector;
                        }
                        if (Math.Abs(tempVector.x - updatePacket.RollPitchYaw.x) < 90)
                            updatePacket.RollPitchYaw.x = tempVector.x;
                        if (Math.Abs(tempVector.y - updatePacket.RollPitchYaw.y) < 90)
                            updatePacket.RollPitchYaw.y = tempVector.y;
                        if (Math.Abs(tempVector.z - updatePacket.RollPitchYaw.z) < 90)
                            updatePacket.RollPitchYaw.z = tempVector.z;
                    }
                    Debug.Log(result[1]);
                    var accelerometr = result[1].Replace("aword: ", "").Split(' ');
                    if (accelerometr.Length == 3)
                    {
                        float.TryParse(accelerometr[0], out tempVector.x);
                        float.TryParse(accelerometr[1], out tempVector.y);
                        float.TryParse(accelerometr[2], out tempVector.z);

                        updatePacket.Acceleration.x = (float)Math.Truncate(tempVector.x / step) * step;
                        updatePacket.Acceleration.y = (float)Math.Truncate(tempVector.x / step) * step;
                        updatePacket.Acceleration.z = (float)Math.Truncate(tempVector.x / step) * step;
                    }

                    result[2] = result[2].Replace("Heating: ", "");
                    float.TryParse(result[2], out updatePacket.Heading);

                }
                first = false;
            }
            drone.SetAcceleration(updatePacket.Acceleration);
            drone.SetRotations(updatePacket.RollPitchYaw);
            // Do nothing with heading.
        }

        private void Update()
        {
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
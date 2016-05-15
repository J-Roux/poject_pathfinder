using UnityEngine;
using System.Collections;
using Pathfinder.Simulation.Helper;
using System.Net.Sockets;
using System;
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

        public const int PACKET_LENGTH = 128; 

        private DroneController drone;
        private TcpClient client;
        private NetworkStream dataStream;
        private byte[] dataBuffer = new byte[PACKET_LENGTH];
        private int alreadyRead;
        private String data;
		private bool first;
        private TcpUpdatePacket updatePacket;
        private void Awake() {
            drone = GetComponent<DroneController>();
            client = new TcpClient(Hostname, Port);
            dataStream = client.GetStream();
            updatePacket = new TcpUpdatePacket();
			first = true;
        }

        private void ProcessPacket() {
            // Perform some shaman magic here and fill out the packet class.
			//Debug.Log ("data");
            data = System.Text.Encoding.Default.GetString (dataBuffer);
			var result = data.Split('#');
			if (result.Length > 2) {
				result = result[1].Split('\n');
				if (result.Length == 4) {
					var rollPitchYaw = result [0].Replace ("ypr: ", "").Split (' ');
					if (rollPitchYaw.Length == 3) {
						var tempVector = new Vector3 ();
						float.TryParse (rollPitchYaw [0], out tempVector.z);
						float.TryParse (rollPitchYaw [1], out tempVector.y);
						float.TryParse (rollPitchYaw [2], out tempVector.x);
						if(first){
							updatePacket.RollPitchYaw = tempVector;
						}
						if(Vector3.Distance(tempVector, updatePacket.RollPitchYaw) < 20)
							updatePacket.RollPitchYaw = tempVector;

					}
					Debug.Log(result[1]);
					var accelerometr = result[1].Replace("aword: ", "").Split(' ');
                	if(accelerometr.Length == 3)
                	{
                   		 float.TryParse(accelerometr[0], out updatePacket.Acceleration.z);
                   		 float.TryParse(accelerometr[1], out updatePacket.Acceleration.y);
                   		 float.TryParse(accelerometr[2], out updatePacket.Acceleration.x);
                	}

					result [2] = result [2].Replace ("Heating: ", "");
					float.TryParse (result [2], out updatePacket.Heading);

				}
				first = false;
			}
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

				//Debug.Log("Read");
                ProcessPacket();
                alreadyRead = 0;
            }
        }
    }

}
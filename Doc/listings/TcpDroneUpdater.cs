[RequireComponent(typeof(DroneController))]
public class TcpDroneUpdater : MonoBehaviour {
    public string Hostname;
    public ushort Port;
    
    public readonly int PACKET_LENGTH = 7 * sizeof(float); 
    public readonly int UPDATE_EVERY = 50; 

    private DroneController drone;
    private TcpClient client;
    private NetworkStream dataStream;
    
    private byte[] dataBuffer = new byte[PACKET_LENGTH];
    private int alreadyRead;
    private int packetNo = 0;

    private void Awake() {
	drone = GetComponent<DroneController>();
	client = new TcpClient(Hostname, Port);
	dataStream = client.GetStream();
    }

    private void ProcessPacket() {
	var updatePacket = new RPYAccPacket();

	// Details ommitted...
	
	drone.SetAcceleration(updatePacket.AccX, updatePacket.AccY, updatePacket.AccZ);
	drone.SetRotations(updatePacket.Roll, updatePacket.Pitch, updatePacket.Yaw);
	if (packetNo % UPDATE_EVERY == 0)
	    drone.AdjustHeading(updatePacket.Heading);
    }

    private void Update() {
	if (!dataStream.CanRead)
	    return;
	
	if (dataStream.DataAvailable)
	{
	    alreadyRead += dataStream.Read(dataBuffer, alreadyRead,
					   PACKET_LENGTH - alreadyRead);
	}
	
	if (alreadyRead == PACKET_LENGTH)
	{
	    ProcessPacket();
	    alreadyRead = 0;
	    packetNo++;
	}
    }
}

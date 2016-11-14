/* server.cpp */
#include <stdio.h>
#include <enet/enet.h>
#include "../../CommonInterfaces/CommonGUIHelperInterface.h"
#ifdef NO_SHARED_MEMORY
	#include "PhysicsServerCommandProcessor.h"
	typedef PhysicsServerCommandProcessor MyCommandProcessor;
#else
	#include "SharedMemoryCommandProcessor.h"
	typedef SharedMemoryCommandProcessor MyCommandProcessor ;
#endif //NO_SHARED_MEMORY
#include "SharedMemoryCommands.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "PhysicsServerCommandProcessor.h"
	
	
bool gVerboseNetworkMessagesServer = false;

void MySerializeInt(unsigned int sz, unsigned char* output)
{
	unsigned int tmp = sz;
	output[0] = tmp & 255;
	tmp = tmp >> 8;
	output[1] = tmp & 255;
	tmp = tmp >> 8;
	output[2] = tmp & 255;
	tmp = tmp >> 8;
	output[3] = tmp & 255;
}



int main(int argc, char *argv[])
{

	DummyGUIHelper guiHelper;
	PhysicsCommandProcessorInterface* sm = new MyCommandProcessor;
	sm->setGuiHelper(&guiHelper);
	
//	PhysicsDirect* sm = new PhysicsDirect(sdk);

	//PhysicsClientSharedMemory* sm = new PhysicsClientSharedMemory();

	bool isPhysicsClientConnected = sm->connect();

	if (isPhysicsClientConnected)
	{
		
		ENetAddress address;
		ENetHost *server;
		ENetEvent event;
		int serviceResult;

		puts("Starting server");

		if (enet_initialize() != 0)
		{
			puts("Error initialising enet");
			exit(EXIT_FAILURE);
		}


		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		address.host = ENET_HOST_ANY;
		/* Bind the server to port 1234. */
		address.port = 1234;

		server = enet_host_create(&address,
			32,   /* number of clients */
			2,    /* number of channels */
			0,    /* Any incoming bandwith */
			0);   /* Any outgoing bandwith */

		if (server == NULL)
		{
			puts("Could not create server host");
			exit(EXIT_FAILURE);
		}


		while (true)
		{
			serviceResult = 1;

			/* Keep doing host_service until no events are left */
			while (serviceResult > 0)
			{
				/* Wait up to 1000 milliseconds for an event. */
				serviceResult = enet_host_service(server, &event, 0);
				if (serviceResult > 0)
				{

					switch (event.type)
					{
					case ENET_EVENT_TYPE_CONNECT:
						printf("A new client connected from %x:%u.\n",
							event.peer->address.host,
							event.peer->address.port);

						/* Store any relevant client information here. */
						event.peer->data = (void*)"Client information";

						break;

					case ENET_EVENT_TYPE_RECEIVE:
						if (gVerboseNetworkMessagesServer)
						{
							printf("A packet of length %u containing '%s' was "
								"received from %s on channel %u.\n",
								event.packet->dataLength,
								event.packet->data,
								event.peer->data,
								event.channelID);
						}
						if (event.packet->dataLength == sizeof(SharedMemoryCommand))
						{
							SharedMemoryCommand* cmdPtr = (SharedMemoryCommand*)event.packet->data;
							SharedMemoryStatus serverStatus;
							b3AlignedObjectArray<char> buffer;
							buffer.resize(SHARED_MEMORY_MAX_STREAM_CHUNK_SIZE);

							bool hasStatus = sm->processCommand(*cmdPtr,serverStatus, &buffer[0], buffer.size());

							int timeout = 1024 * 1024 * 1024;
							while ((!hasStatus) && (timeout-- > 0))
							{
								hasStatus = sm->receiveStatus(serverStatus, &buffer[0], buffer.size());

							}
							if (gVerboseNetworkMessagesServer)
							{
								printf("buffer.size = %d\n", buffer.size());
								printf("serverStatus.m_numDataStreamBytes = %d\n", serverStatus.m_numDataStreamBytes);
							}
							if (hasStatus)
							{
								//create packetData with [int packetSizeInBytes, status, streamBytes)
								unsigned char* statBytes = (unsigned char*)&serverStatus;
								b3AlignedObjectArray<unsigned char> packetData;
								packetData.resize(4 + sizeof(SharedMemoryStatus) + serverStatus.m_numDataStreamBytes);
								int sz = packetData.size();
								int curPos = 0;

								MySerializeInt(sz, &packetData[curPos]);
								curPos += 4;
								for (int i = 0; i < sizeof(SharedMemoryStatus); i++)
								{
									packetData[i + curPos] = statBytes[i];
								}
								curPos += sizeof(SharedMemoryStatus);

								for (int i = 0; i < serverStatus.m_numDataStreamBytes; i++)
								{
									packetData[i + curPos] = buffer[i];
								}

								ENetPacket *packet = enet_packet_create(&packetData[0], packetData.size() , ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(event.peer, 0, packet);
								//enet_host_broadcast(server, 0, packet);
							}
						}
						else
						{
							printf("received packet with unknown contents\n");
						}


						/* Tell all clients about this message */
						//enet_host_broadcast(server, 0, event.packet);

						break;

					case ENET_EVENT_TYPE_DISCONNECT:
						printf("%s disconnected.\n", event.peer->data);

						/* Reset the peer's client information. */

						event.peer->data = NULL;

						break;
					}
				}
				else if (serviceResult > 0)
				{
					puts("Error with servicing the server");
					exit(EXIT_FAILURE);
				}
			}

		}

		enet_host_destroy(server);
		enet_deinitialize();
	}
	delete sm;

	return 0;

}

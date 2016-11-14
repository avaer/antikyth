
#include "SharedMemoryInProcessPhysicsC_API.h"

#include "PhysicsClientSharedMemory.h"
#include"../ExampleBrowser/InProcessExampleBrowser.h"


class InProcessPhysicsClientSharedMemoryMainThread : public PhysicsClientSharedMemory
{
    btInProcessExampleBrowserMainThreadInternalData* m_data;
    
public:
    
    InProcessPhysicsClientSharedMemoryMainThread(int argc, char* argv[])
    {
        int newargc = argc+2;
        char** newargv = (char**)malloc(sizeof(void*)*newargc);
        for (int i=0;i<argc;i++)
            newargv[i] = argv[i];
        
        char* t0 = (char*)"--logtostderr";
        char* t1 = (char*)"--start_demo_name=Physics Server";
        newargv[argc] = t0;
        newargv[argc+1] = t1;
        m_data = btCreateInProcessExampleBrowserMainThread(newargc,newargv);
        SharedMemoryInterface* shMem = btGetSharedMemoryInterfaceMainThread(m_data);
        
        setSharedMemoryInterface(shMem);
    }
    
    virtual ~InProcessPhysicsClientSharedMemoryMainThread()
    {
        setSharedMemoryInterface(0);
        btShutDownExampleBrowserMainThread(m_data);
    }
    
    // return non-null if there is a status, nullptr otherwise
    virtual const struct SharedMemoryStatus* processServerStatus()
    {
        if (btIsExampleBrowserMainThreadTerminated(m_data))
        {
            PhysicsClientSharedMemory::disconnectSharedMemory();
        }
        
        btUpdateInProcessExampleBrowserMainThread(m_data);
        return PhysicsClientSharedMemory::processServerStatus();
        
        
    }
    
    virtual bool submitClientCommand(const struct SharedMemoryCommand& command)
    {
//        btUpdateInProcessExampleBrowserMainThread(m_data);
        return PhysicsClientSharedMemory::submitClientCommand(command);
    }
    
};

b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnectMainThread(int argc, char* argv[])
{
    InProcessPhysicsClientSharedMemoryMainThread* cl = new InProcessPhysicsClientSharedMemoryMainThread(argc, argv);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY);
    cl->connect();
    return (b3PhysicsClientHandle ) cl;
}

class InProcessPhysicsClientSharedMemory : public PhysicsClientSharedMemory
{
	btInProcessExampleBrowserInternalData* m_data;
public:

	InProcessPhysicsClientSharedMemory(int argc, char* argv[])
	{
		int newargc = argc+2;
		char** newargv = (char**)malloc(sizeof(void*)*newargc);
		for (int i=0;i<argc;i++)
		newargv[i] = argv[i];

		char* t0 = (char*)"--logtostderr";
		char* t1 = (char*)"--start_demo_name=Physics Server";
		newargv[argc] = t0;
		newargv[argc+1] = t1;
		m_data = btCreateInProcessExampleBrowser(newargc,newargv);
		SharedMemoryInterface* shMem = btGetSharedMemoryInterface(m_data);

		setSharedMemoryInterface(shMem);
	}

	virtual ~InProcessPhysicsClientSharedMemory()
	{
		setSharedMemoryInterface(0);
		btShutDownExampleBrowser(m_data);
	}

};

b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnect(int argc, char* argv[])
{
	
	InProcessPhysicsClientSharedMemory* cl = new InProcessPhysicsClientSharedMemory(argc, argv);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY);
    cl->connect();
	return (b3PhysicsClientHandle ) cl;
}


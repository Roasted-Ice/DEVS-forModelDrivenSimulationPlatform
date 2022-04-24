#include <DEVSpp/ModelBase/Server.h>
#include <float.h>
#include <assert.h>

using namespace MB_Basic;

/*virtual*/ void Server::init()
{
	m_phase = IDLE;
	if(m_pJob)
		delete m_pJob;
	m_pJob=NULL;
}

/*virtual*/ Time Server::tau() const 
{ 
	if(m_phase == IDLE)
		return DBL_MAX;
	else if(m_phase == BUSY) {
		assert(m_pJob);
		return m_pJob->GetProccessTime();
	}
	else
		return DBL_MAX;
}

/*virtual*/ bool Server::delta_x(const PortValue& x) 
{
	if(x.port == in){ //receiving a client
		if(m_phase == IDLE){
			m_pJob = dynamic_cast<Job*>(x.value);
			assert(m_pJob);
			m_phase = BUSY;
			return true;
		}else if( m_phase == BUSY)
		{
			m_phase = COLLIDED;
			return true;
		} // already collided,
		return false;
	} 
}
/*virtual*/ void Server::delta_y(deque<PortValue>& ys) 
{
	ys.push_back(PortValue(out, m_pJob)); 
	m_pJob = NULL; // don't delete that. sink will delete that.
	m_phase = IDLE;
}

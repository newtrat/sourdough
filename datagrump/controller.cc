#include <iostream>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;



/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), packetsUntilIncrease(0), curWinSize(10),
    lastSendTimestamp(0), packetsUntilDecrease(0),
    slowStartThreshold(5)
{
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << curWinSize << endl;
  }

  return curWinSize;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */
  /*if (lastSendTimestamp != 0 && (send_timestamp - lastSendTimestamp) > timeout_ms()) {
    curWinSize = max(curWinSize / 2, 1u);
    packetsUntilIncrease = curWinSize;
    cout << "Decreasing curWinSize to " << curWinSize << endl;
  }

  lastSendTimestamp = send_timestamp;
  */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */
  /* AIMD */
  if ((timestamp_ack_received - send_timestamp_acked) > 100 && packetsUntilDecrease == 0) {
    curWinSize = max(curWinSize  / 2, 1u);
    slowStartThreshold = curWinSize;
    packetsUntilDecrease = max(curWinSize, 1u);
    packetsUntilIncrease = curWinSize;
    curWinSize = 1;
    
    cout << "Decreasing curWinSize to " << curWinSize << endl;
    
  } else {
    // Exponential slow start
    if (curWinSize < slowStartThreshold) {
      curWinSize++;
      packetsUntilIncrease = curWinSize;
    } else {
      if (packetsUntilIncrease == 0) {
        curWinSize++;
        packetsUntilIncrease = curWinSize;
        //cout << "Increasing curWinSize to " << curWinSize << endl;
      }

      packetsUntilIncrease = (packetsUntilIncrease == 0) ? 0 : packetsUntilIncrease - 1;
    }

    packetsUntilDecrease = (packetsUntilDecrease == 0) ? 0 : packetsUntilDecrease - 1;
  }

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}

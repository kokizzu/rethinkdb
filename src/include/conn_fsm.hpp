
#ifndef __CONN_FSM_HPP__
#define __CONN_FSM_HPP__

#include "containers/intrusive_list.hpp"
#include "arch/resource.hpp"
#include "request_handler/request_handler.hpp"
#include "event.hpp"
#include "btree/fsm.hpp"
#include "corefwd.hpp"

// The actual state structure
template<class config_t>
struct conn_fsm : public event_state_t,
                    public config_t::iocalls_t,
                    public intrusive_list_node_t<conn_fsm<config_t> >
{
public:
    typedef typename config_t::alloc_t alloc_t;
    typedef typename config_t::iocalls_t iocalls_t;
    typedef typename config_t::iobuf_t iobuf_t;
    typedef typename config_t::btree_fsm_t btree_fsm_t;
    typedef typename config_t::req_handler_t req_handler_t;
    
public:
    // Possible transition results
    enum result_t {
        fsm_invalid,
        fsm_shutdown_server,
        fsm_quit_connection,
        fsm_transition_ok,
    };

    enum state_t {
        // Socket is connected, is in a clean state (no outstanding ops) and ready to go
        fsm_socket_connected,
        // Socket has received an incomplete packet and waiting for the rest of the command
        fsm_socket_recv_incomplete,
        // We sent a msg over the socket but were only able to send a partial packet
        fsm_socket_send_incomplete,
        // Waiting for IO initiated by the BTree to complete
        fsm_btree_incomplete
    };
    
public:
    conn_fsm(resource_t _source, alloc_t* _alloc, req_handler_t *_req_handler,
               event_queue_t *_event_queue);
    ~conn_fsm();
    
    result_t do_transition(event_t *event);

public:
    state_t state;

    // A buffer with IO communication (possibly incomplete). The nbuf
    // variable indicates how many bytes are stored in the buffer. The
    // snbuf variable indicates how many bytes out of the buffer have
    // been sent (in case of a send workflow).
    char *buf;
    unsigned int nbuf, snbuf;
    alloc_t *alloc;
    req_handler_t *req_handler;
    event_queue_t *event_queue;
    btree_fsm_t *btree_fsm;
    
private:
    result_t do_socket_ready(event_t *event);
    result_t do_socket_send_incomplete(event_t *event);
    result_t do_fsm_btree_incomplete(event_t *event);
    
    void send_msg_to_client();
    void send_err_to_client();
    void init_state();
    void return_to_socket_connected();
};

// Include the implementation
#include "conn_fsm_impl.hpp"

#endif // __CONN_FSM_HPP__


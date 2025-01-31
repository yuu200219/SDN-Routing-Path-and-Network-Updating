#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <climits>
#include <functional>
#include <iomanip>
#include <stack>
#include <string>
#include <cstring>
#include <deque>
#include <algorithm>

using namespace std;

#define SET(func_name,type,var_name,_var_name) void func_name(type _var_name) { var_name = _var_name ;} 
#define GET(func_name,type,var_name) type func_name() const { return var_name ;}

unsigned int r_arr[1000][1000];//dest and nodes
static deque<unsigned int> n_update[1000]; //suppose there is 100 dests
class header;
class payload;
class packet;
class node;
class event;
class link; // new

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const unsigned int ONE_HOP_DELAY = 10;
const unsigned int BROCAST_ID = UINT_MAX;
//static vector <pair<unsigned int, unsigned int>>n_update[10000];


// BROCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of unsigned int

class header {
    public:
        virtual ~header() {}

        SET(setSrcID, unsigned int , srcID, _srcID);
        SET(setDstID, unsigned int , dstID, _dstID);
        SET(setPreID, unsigned int , preID, _preID);
        SET(setNexID, unsigned int , nexID, _nexID);
        GET(getSrcID, unsigned int , srcID);
        GET(getDstID, unsigned int , dstID);
        GET(getPreID, unsigned int , preID);
        GET(getNexID, unsigned int , nexID);
        
        virtual string type() = 0;
        
        // factory concept: generate a header
        class header_generator {
                // lock the copy constructor
                header_generator(header_generator &){}
                // store all possible types of header
                static map<string,header_generator*> prototypes;
            protected:
                // allow derived class to use it
                header_generator() {}
                // after you create a new header type, please register the factory of this header type by this function
                void register_header_type(header_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your header
                virtual header* generate() = 0 ;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0 ;
        	    // this function is used to generate any type of header derived
        	    static header * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such header type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered header types: " << endl;
            	    for (map<string,header::header_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~header_generator(){};
        };
        
    protected:
        header():srcID(BROCAST_ID),dstID(BROCAST_ID),preID(BROCAST_ID),nexID(BROCAST_ID){} // this constructor cannot be directly called by users

    private:
        unsigned int srcID;
        unsigned int dstID;
        unsigned int preID;
        unsigned int nexID;
        header(header&){} // this constructor cannot be directly called by users
};
map<string,header::header_generator*> header::header_generator::prototypes;

class SDN_data_header : public header{
        SDN_data_header(SDN_data_header&){} // cannot be called by users
        
    protected:
        SDN_data_header(){} // this constructor cannot be directly called by users

    public:
        ~SDN_data_header(){}
        string type() { return "SDN_data_header"; }

        class SDN_data_header_generator;
        friend class SDN_data_header_generator;
        // SDN_data_header_generator is derived from header_generator to generate a header
        class SDN_data_header_generator : public header_generator{
                static SDN_data_header_generator sample;
                // this constructor is only for sample to register this header type
                SDN_data_header_generator() { /*cout << "SDN_data_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "SDN_data_header generated" << endl;
                    return new SDN_data_header; 
                }
            public:
                virtual string type() { return "SDN_data_header";}
                ~SDN_data_header_generator(){}
        
        };
};
SDN_data_header::SDN_data_header_generator SDN_data_header::SDN_data_header_generator::sample;

class SDN_ctrl_header : public header{
        SDN_ctrl_header(SDN_ctrl_header&){} // cannot be called by users
        
    protected:
        SDN_ctrl_header(){} // this constructor cannot be directly called by users

    public:
        ~SDN_ctrl_header(){}
        string type() { return "SDN_ctrl_header"; }

        class SDN_ctrl_header_generator;
        friend class SDN_ctrl_header_generator;
        // SDN_ctrl_header_generator is derived from header_generator to generate a header
        class SDN_ctrl_header_generator : public header_generator{
                static SDN_ctrl_header_generator sample;
                // this constructor is only for sample to register this header type
                SDN_ctrl_header_generator() { /*cout << "SDN_ctrl_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "SDN_ctrl_header generated" << endl;
                    return new SDN_ctrl_header; 
                }
            public:
                virtual string type() { return "SDN_ctrl_header";}
                ~SDN_ctrl_header_generator(){}
        
        };
};
SDN_ctrl_header::SDN_ctrl_header_generator SDN_ctrl_header::SDN_ctrl_header_generator::sample;


class payload {
        payload(payload&){} // this constructor cannot be directly called by users
        
        string msg;
        
    protected:
        payload(){}
    public:
        virtual ~payload(){}
        virtual string type() = 0;
        
        SET(setMsg,string,msg,_msg);
        GET(getMsg,string,msg);
        
        class payload_generator {
                // lock the copy constructor
                payload_generator(payload_generator &){}
                // store all possible types of header
                static map<string,payload_generator*> prototypes;
            protected:
                // allow derived class to use it
                payload_generator() {}
                // after you create a new payload type, please register the factory of this payload type by this function
                void register_payload_type(payload_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual payload* generate() = 0;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of header derived
        	    static payload * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such payload type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered payload types: " << endl;
            	    for (map<string,payload::payload_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~payload_generator(){};
        };
};
map<string,payload::payload_generator*> payload::payload_generator::prototypes;


class SDN_data_payload : public payload {
        SDN_data_payload(SDN_data_payload&){}

    protected:
        SDN_data_payload(){} // this constructor cannot be directly called by users
    public:
        ~SDN_data_payload(){}
        
        string type() { return "SDN_data_payload"; }
        
        class SDN_data_payload_generator;
        friend class SDN_data_payload_generator;
        // SDN_data_payload is derived from payload_generator to generate a payload
        class SDN_data_payload_generator : public payload_generator{
                static SDN_data_payload_generator sample;
                // this constructor is only for sample to register this payload type
                SDN_data_payload_generator() { /*cout << "SDN_data_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){ 
                    // cout << "SDN_data_payload generated" << endl;
                    return new SDN_data_payload; 
                }
            public:
                virtual string type() { return "SDN_data_payload";}
                ~SDN_data_payload_generator(){}
        };
};
SDN_data_payload::SDN_data_payload_generator SDN_data_payload::SDN_data_payload_generator::sample;

class SDN_ctrl_payload : public payload {
        SDN_ctrl_payload(SDN_ctrl_payload&){}
        
        unsigned int matID; // match: target
        unsigned int actID; // action: the next hop

    protected:
        SDN_ctrl_payload(){} // this constructor cannot be directly called by users
    public:
        ~SDN_ctrl_payload(){}

        string type() { return "SDN_ctrl_payload"; }
        
        SET(setMatID,unsigned int,matID,_matID);
        GET(getMatID,unsigned int,matID);
        SET(setActID,unsigned int,actID,_actID);
        GET(getActID,unsigned int,actID);
        
        
        class SDN_ctrl_payload_generator;
        friend class SDN_ctrl_payload_generator;
        // SDN_data_payload is derived from payload_generator to generate a payload
        class SDN_ctrl_payload_generator : public payload_generator{
                static SDN_ctrl_payload_generator sample;
                // this constructor is only for sample to register this payload type
                SDN_ctrl_payload_generator() { /*cout << "SDN_ctrl_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){ 
                    // cout << "SDN_ctrl_payload generated" << endl;
                    return new SDN_ctrl_payload; 
                }
            public:
                virtual string type() { return "SDN_ctrl_payload";}
                ~SDN_ctrl_payload_generator(){}
        };
};
SDN_ctrl_payload::SDN_ctrl_payload_generator SDN_ctrl_payload::SDN_ctrl_payload_generator::sample;


class packet{
        // a packet usually contains a header and a payload
        header *hdr;
        payload *pld;
        unsigned int p_id;
        static unsigned int last_packet_id ; 
        
        packet(packet &) {}
        static int live_packet_num ;
    protected:
        // these constructors cannot be directly called by users
        packet(): hdr(nullptr), pld(nullptr) { p_id=last_packet_id++; live_packet_num ++; }
        packet(string _hdr, string _pld, bool rep = false, unsigned int rep_id = 0) {
            if (! rep ) // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            else
                p_id = rep_id;
            hdr = header::header_generator::generate(_hdr); 
            pld = payload::payload_generator::generate(_pld); 
            live_packet_num ++;
        }
    public:
        virtual ~packet(){ 
            // cout << "packet destructor begin" << endl;
            if (hdr != nullptr) 
                delete hdr; 
            if (pld != nullptr) 
                delete pld; 
            live_packet_num --;
            // cout << "packet destructor end" << endl;
        }
        
        SET(setHeader,header*,hdr,_hdr);
        GET(getHeader,header*,hdr);
        SET(setPayload,payload*,pld,_pld);
        GET(getPayload,payload*,pld);
        GET(getPacketID,unsigned int,p_id);
        
        static void discard ( packet* &p ) {
            // cout << "checking" << endl;
            if (p != nullptr) {
                // cout << "discarding" << endl;
                // cout << p->type() << endl;
                delete p;
                // cout << "discarded" << endl;
            }
            p = nullptr;
            // cout << "checked" << endl;
        }
        virtual string type () = 0;
        //bool ack = false;
        
        static int getLivePacketNum () { return live_packet_num; }
        
        class packet_generator;
        friend class packet_generator;
        class packet_generator {
                // lock the copy constructor
                packet_generator(packet_generator &){}
                // store all possible types of packet
                static map<string,packet_generator*> prototypes;
            protected:
                // allow derived class to use it
                packet_generator() {}
                // after you create a new packet type, please register the factory of this payload type by this function
                void register_packet_type(packet_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual packet* generate ( packet *p = nullptr) = 0;
            public:
                // you have to implement your own type() to return your packet type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of packet derived
        	    static packet * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            			return prototypes[type]->generate(); // generate it!!
            		}
                    //cout << type << endl;
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static packet * replicate (packet *p) {
            	    if(prototypes.find(p->type()) != prototypes.end()){ // if this type derived exists 
            			return prototypes[p->type()]->generate(p); // generate it!!
            		}
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered packet types: " << endl;
            	    for (map<string,packet::packet_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~packet_generator(){};
        };
};
map<string,packet::packet_generator*> packet::packet_generator::prototypes;
unsigned int packet::last_packet_id = 0 ;
int packet::live_packet_num = 0;


// this packet is used to tell the destination the msg
class SDN_data_packet: public packet {
        SDN_data_packet(SDN_data_packet &) {}
        
    protected:
        SDN_data_packet(){} // this constructor cannot be directly called by users
        SDN_data_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<SDN_data_header*>(this->getHeader())) = *(dynamic_cast<SDN_data_header*> (p->getHeader()));
            *(dynamic_cast<SDN_data_payload*>(this->getPayload())) = *(dynamic_cast<SDN_data_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<SDN_data_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<SDN_data_header*>(p))->isVisited;
        } // for duplicate
        SDN_data_packet(string _h, string _p): packet(_h,_p) {}
        
    public:
        virtual ~SDN_data_packet(){}
        string type() { return "SDN_data_packet"; }
        
        class SDN_data_packet_generator;
        friend class SDN_data_packet_generator;
        // SDN_data_packet is derived from packet_generator to generate a pub packet
        class SDN_data_packet_generator : public packet_generator{
                static SDN_data_packet_generator sample;
                // this constructor is only for sample to register this packet type
                SDN_data_packet_generator() { /*cout << "SDN_data_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "SDN_data_packet generated" << endl;
                    if ( nullptr == p )
                        return new SDN_data_packet("SDN_data_header","SDN_data_payload"); 
                    else
                        return new SDN_data_packet(p); // duplicate
                }
            public:
                virtual string type() { return "SDN_data_packet";}
                ~SDN_data_packet_generator(){}
        };
};
SDN_data_packet::SDN_data_packet_generator SDN_data_packet::SDN_data_packet_generator::sample;

class SDN_ctrl_packet: public packet {
        SDN_ctrl_packet(SDN_ctrl_packet &) {}
        
    protected:
        SDN_ctrl_packet(){} // this constructor cannot be directly called by users
        SDN_ctrl_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<SDN_ctrl_header*>(this->getHeader())) = *(dynamic_cast<SDN_ctrl_header*> (p->getHeader()));
            *(dynamic_cast<SDN_ctrl_payload*>(this->getPayload())) = *(dynamic_cast<SDN_ctrl_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<SDN_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<SDN_ctrl_header*>(p))->isVisited;
        } // for duplicate
        SDN_ctrl_packet(string _h, string _p): packet(_h,_p) {}
        
    public:
        virtual ~SDN_ctrl_packet(){}
        string type() { return "SDN_ctrl_packet"; }
        
        class SDN_ctrl_packet_generator;
        friend class SDN_ctrl_packet_generator;
        // SDN_ctrl_packet is derived from packet_generator to generate a pub packet
        class SDN_ctrl_packet_generator : public packet_generator{
                static SDN_ctrl_packet_generator sample;
                // this constructor is only for sample to register this packet type
                SDN_ctrl_packet_generator() { /*cout << "SDN_ctrl_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "SDN_ctrl_packet generated" << endl;
                    if ( nullptr == p )
                        return new SDN_ctrl_packet("SDN_ctrl_header","SDN_ctrl_payload"); 
                    else
                        return new SDN_ctrl_packet(p); // duplicate
                }
            public:
                virtual string type() { return "SDN_ctrl_packet";}//this type need to be exist
                ~SDN_ctrl_packet_generator(){}
        };
};
SDN_ctrl_packet::SDN_ctrl_packet_generator SDN_ctrl_packet::SDN_ctrl_packet_generator::sample;

class node {
        // all nodes created in the program
        static map<unsigned int, node*> id_node_table;
        
        unsigned int id;
        map<unsigned int,bool> phy_neighbors;
        
    protected:
        node(node&){} // this constructor should not be used
        node(){} // this constructor should not be used
        node(unsigned int _id): id(_id) { id_node_table[_id] = this; }
    public:
        virtual ~node() { // erase the node
            id_node_table.erase (id) ;
        }
        virtual string type() = 0; // please define it in your derived node class

        unsigned int flag;
        void initial_flag() {
            flag = 0;
        }
        vector<pair<unsigned int, unsigned int> > oldW, newW; //oldW newW and n_id //e
        map<unsigned int, unsigned int> table; //dest next node
        map<unsigned int, unsigned int> Pre_num;
        map<unsigned int, vector<unsigned int> > PreNode;//dest pre node
        
        
        
        
        void add_phy_neighbor (unsigned int _id, string link_type = "simple_link"); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id); // we only delete a directed link from id to _id
        
        // you can use the function to get the node's neigbhors at this time
        // but in the project 3, you are not allowed to use this function 
        const map<unsigned int,bool> & getPhyNeighbors () { 
            return phy_neighbors;
        }
        
        
        void recv (packet *p) {
            packet *tp = p;
            recv_handler(tp); 
            packet::discard(p); 
        } // the packet will be directly deleted after the handler
        void send (packet *p);
        
        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(packet *p) = 0;
        void send_handler(packet *P);
        
        static node * id_to_node (unsigned int _id) { return ((id_node_table.find(_id)!=id_node_table.end()) ? id_node_table[_id]: nullptr) ; }
        GET(getNodeID,unsigned int,id);
        
        static void del_node (unsigned int _id) {
            if (id_node_table.find(_id) != id_node_table.end())
                id_node_table.erase(_id);
        }
        static unsigned int getNodeNum () { return id_node_table.size(); }

        class node_generator {
                // lock the copy constructor
                node_generator(node_generator &){}
                // store all possible types of node
                static map<string,node_generator*> prototypes;
            protected:
                // allow derived class to use it
                node_generator() {}
                // after you create a new node type, please register the factory of this node type by this function
                void register_node_type(node_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your node
                virtual node* generate(unsigned int _id) = 0;
            public:
                // you have to implement your own type() to return your node type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of node derived
        	    static node * generate (string type, unsigned int _id) {
        	        if(id_node_table.find(_id)!=id_node_table.end()){
        	            std::cerr << "duplicate node id" << std::endl; // node id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id ) {
        	            cerr << "BROCAST_ID cannot be used" << endl;
        	            return nullptr;
        	        }
            		else if(prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            		    node * created_node = prototypes[type]->generate(_id);
            			return created_node; // generate it!!
            		}
            		std::cerr << "no such node type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered node types: " << endl;
            	    for (map<string,node::node_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~node_generator(){};
        };
};
map<string,node::node_generator*> node::node_generator::prototypes;
map<unsigned int,node*> node::id_node_table;

class SDN_controller: public node {
    //received info to restore the info
    protected:
        SDN_controller() {} // it should not be used
        SDN_controller(SDN_controller&) {} // it should not be used
        SDN_controller(unsigned int _id): node(_id) {} // this constructor cannot be directly called by users
        //map<int, int> reg; //to store the input packet, dest register

    public:
        ~SDN_controller(){}
        string type() { return "SDN_controller"; }

        virtual void recv_handler (packet *p);

        class SDN_controller_generator;
        friend class SDN_controller_generator;
        // SDN_switch is derived from node_generator to generate a node
        class SDN_controller_generator : public node_generator{
                static SDN_controller_generator sample;
                // this constructor is only for sample to register this node type
                SDN_controller_generator() { /*cout << "SDN_switch registered" << endl;*/ register_node_type(&sample); }
            protected:
                virtual node * generate(unsigned int _id){ /*cout << "SDN_switch generated" << endl;*/ return new SDN_controller(_id); }
            public:
                virtual string type() { return "SDN_controller";}
                ~SDN_controller_generator(){}
        };

};
SDN_controller::SDN_controller_generator SDN_controller::SDN_controller_generator::sample;

class SDN_switch: public node {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors 
       map<unsigned int, unsigned int> r_table;//destid, nextid 
       
    protected:
        SDN_switch() {} // it should not be used
        SDN_switch(SDN_switch&) {} // it should not be used
        SDN_switch(unsigned int _id): node(_id) {} // this constructor cannot be directly called by users
    
    public:
        ~SDN_switch(){}
        string type() { return "SDN_switch"; }
        
        // please define recv_handler function to deal with the incoming packet
        virtual void recv_handler (packet *p);
        
        // void add_one_hop_neighbor (unsigned int n_id) { one_hop_neighbors[n_id] = true; }
        // unsigned int get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }
        
        class SDN_switch_generator;
        friend class SDN_switch_generator;
        // SDN_switch is derived from node_generator to generate a node
        class SDN_switch_generator : public node_generator{
                static SDN_switch_generator sample;
                // this constructor is only for sample to register this node type
                SDN_switch_generator() { /*cout << "SDN_switch registered" << endl;*/ register_node_type(&sample); }
            protected:
                virtual node * generate(unsigned int _id){ /*cout << "SDN_switch generated" << endl;*/ return new SDN_switch(_id); }
            public:
                virtual string type() { return "SDN_switch";}
                ~SDN_switch_generator(){}
        };
};
SDN_switch::SDN_switch_generator SDN_switch::SDN_switch_generator::sample;

class mycomp {
    bool reverse;
    
    public:
        mycomp(const bool& revparam = false) { reverse=revparam ; }
        bool operator() (const event* lhs, const event* rhs) const;
};

class event {
        event(event*&){} // this constructor cannot be directly called by users
        static priority_queue < event*, vector < event* >, mycomp > events;
        static unsigned int cur_time; // timer
        static unsigned int end_time;
        
        // get the next event
        static event * get_next_event() ;
        static void add_event (event *e) { events.push(e); }
        static hash<string> event_seq;
        
    protected:
        unsigned int trigger_time;
        
        event(){} // it should not be used
        event(unsigned int _trigger_time): trigger_time(_trigger_time) {}

    public:
        virtual void trigger()=0;
        virtual ~event(){}

        virtual unsigned int event_priority() const = 0;
        unsigned int get_hash_value(string string_for_hash) const {
            unsigned int priority = event_seq (string_for_hash);
            return priority;
        }
        
        static void flush_events (); // only for debug
        
        GET(getTriggerTime,unsigned int,trigger_time);
        
        static void start_simulate( unsigned int _end_time ); // the function is used to start the simulation
        
        static unsigned int getCurTime() { return cur_time ; }
        static void getCurTime(unsigned int _cur_time) { cur_time = _cur_time; } 
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }
        
        virtual void print () const = 0; // the function is used to print the event information

        class event_generator{
                // lock the copy constructor
                event_generator(event_generator &){}
                // store all possible types of event
                static map<string,event_generator*> prototypes;
            protected:
                // allow derived class to use it
                event_generator() {}
                // after you create a new event type, please register the factory of this event type by this function
                void register_event_type(event_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your event
                virtual event* generate(unsigned int _trigger_time, void * data) = 0;
            public:
                // you have to implement your own type() to return your event type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of event derived
        	    static event * generate (string type, unsigned int _trigger_time, void * data) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    event * e = prototypes[type]->generate(_trigger_time, data);
            		    add_event(e);
            		    return e; // generate it!!
            		}
            		std::cerr << "no such event type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered event types: " << endl;
            	    for (map<string,event::event_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~event_generator(){}
        };
};
map<string,event::event_generator*> event::event_generator::prototypes;
priority_queue < event*, vector< event* >, mycomp > event::events;
hash<string> event::event_seq;

unsigned int event::cur_time = 0;
unsigned int event::end_time = 0;

void event::flush_events()
{ 
    cout << "**flush begin" << endl;
    while ( ! events.empty() ) {
        cout << setw(11) << events.top()->trigger_time << ": " << setw(11) << events.top()->event_priority() << endl;
        delete events.top();
        events.pop();
    }
    cout << "**flush end" << endl;
}
event * event::get_next_event() {
    if(events.empty()) 
        return nullptr; 
    event * e = events.top();
    events.pop(); 
    // cout << events.size() << " events remains" << endl;
    return e; 
}
void event::start_simulate(unsigned int _end_time) {
    if (_end_time<0) {
        cerr << "you should give a possitive value of _end_time" << endl;
        return;
    }
    end_time = _end_time;
    event *e; 
    e = event::get_next_event ();
    while ( e != nullptr && e->trigger_time <= end_time ) {
        if ( cur_time <= e->trigger_time )
            cur_time = e->trigger_time;
        else {
            cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << endl;
            break;
        }

        // cout << "event trigger_time = " << e->trigger_time << endl;
        e->print(); // for log
        // cout << " event begin" << endl;
        e->trigger();
        // cout << " event end" << endl;
        delete e;
        e = event::get_next_event ();
    }
    // cout << "no more event" << endl;
}

bool mycomp::operator() (const event* lhs, const event* rhs) const {
    // cout << lhs->getTriggerTime() << ", " << rhs->getTriggerTime() << endl;
    // cout << lhs->type() << ", " << rhs->type() << endl;
    unsigned int lhs_pri = lhs->event_priority();
    unsigned int rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << endl;
    // cout << "rhs hash = " << rhs_pri << endl;
    
    if (reverse) 
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri < rhs_pri): ((lhs->getTriggerTime()) < (rhs->getTriggerTime()));
    else 
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri > rhs_pri): ((lhs->getTriggerTime()) > (rhs->getTriggerTime()));
}

class recv_event: public event {
    public:
        class recv_data; // forward declaration
            
    private:
        recv_event(recv_event&) {} // this constructor cannot be used
        recv_event() {} // we don't allow users to new a recv_event by themselv
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver; the packet will be given to the receiver
        packet *pkt; // the packet
        
    protected:
        // this constructor cannot be directly called by users; only by generator
        recv_event(unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            recv_data * data_ptr = (recv_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id; // the packet will be given to the receiver
            pkt = data_ptr->_pkt;
        } 
        
    public:
        virtual ~recv_event(){}
        // recv_event will trigger the recv function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class recv_event_generator;
        friend class recv_event_generator;
        // recv_event is derived from event_generator to generate a event
        class recv_event_generator : public event_generator{
                static recv_event_generator sample;
                // this constructor is only for sample to register this event type
                recv_event_generator() { /*cout << "recv_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "recv_event generated" << endl; 
                    return new recv_event(_trigger_time, data); 
                }
                
            public:
                virtual string type() { return "recv_event";}
                ~recv_event_generator(){}
        };
        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
        };
        
        void print () const;
};
recv_event::recv_event_generator recv_event::recv_event_generator::sample;

void recv_event::trigger() {
    if (pkt == nullptr) {
        cerr << "recv_event error: no pkt!" << endl; 
        return ; 
    }
    else if (node::id_to_node(receiverID) == nullptr){
        cerr << "recv_event error: no node " << receiverID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(receiverID)->recv(pkt); 
}
unsigned int recv_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the recv_event::print() function is used for log file
void recv_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "   recID"       << setw(11) << receiverID 
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID"       << setw(11) << pkt->getHeader()->getSrcID() 
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << "   "            << pkt->type()
         << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime() 
    //      << "   recID "      << setw(11) << receiverID 
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID() 
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

class send_event: public event {
    public:
        class send_data; // forward declaration
            
    private:
        send_event (send_event &){}
        send_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver 
        packet *pkt; // the packet
    
    protected:
        send_event (unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            send_data * data_ptr = (send_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id;
            pkt = data_ptr->_pkt;
        } 
        
    public:
        virtual ~send_event(){}
        // send_event will trigger the send function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class send_event_generator;
        friend class send_event_generator;
        // send_event is derived from event_generator to generate a event
        class send_event_generator : public event_generator{
                static send_event_generator sample;
                // this constructor is only for sample to register this event type
                send_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "send_event generated" << endl; 
                    return new send_event(_trigger_time, data); 
                }
            
            public:
                virtual string type() { return "send_event";}
                ~send_event_generator(){}
        };
        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
                unsigned int t;
        };
        
        void print () const;
};
send_event::send_event_generator send_event::send_event_generator::sample;

void send_event::trigger() {
    if (pkt == nullptr) {
        cerr << "send_event error: no pkt!" << endl; 
        return ; 
    }
    else if (node::id_to_node(senderID) == nullptr){
        cerr << "send_event error: no node " << senderID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(senderID)->send(pkt);
}
unsigned int send_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the send_event::print() function is used for log file
void send_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "   senID"       << setw(11) << senderID 
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID"       << setw(11) << pkt->getHeader()->getSrcID() 
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << "   "            << pkt->type()
         //<< "   msg"         << setw(11) << dynamic_cast<SDN_data_payload*>(pkt->getPayload())->getMsg()
         << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime() 
    //      << "   senID "      << setw(11) << senderID
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID() 
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID() 
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

////////////////////////////////////////////////////////////////////////////////

class SDN_data_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration
            
    private:
        SDN_data_pkt_gen_event (SDN_data_pkt_gen_event &){}
        SDN_data_pkt_gen_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst 
        // packet *pkt; // the packet
        string msg;
    
    protected:
        SDN_data_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            msg = data_ptr->msg;
        } 
        
    public:
        virtual ~SDN_data_pkt_gen_event(){}
        // SDN_data_pkt_gen_event will trigger the packet gen function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class SDN_data_pkt_gen_event_generator;
        friend class SDN_data_pkt_gen_event_generator;
        // SDN_data_pkt_gen_event_generator is derived from event_generator to generate an event
        class SDN_data_pkt_gen_event_generator : public event_generator{
                static SDN_data_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                SDN_data_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "send_event generated" << endl; 
                    return new SDN_data_pkt_gen_event(_trigger_time, data); 
                }
            
            public:
                virtual string type() { return "SDN_data_pkt_gen_event";}
                ~SDN_data_pkt_gen_event_generator(){}
        };
        // this class is used to initialize the SDN_data_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id;
                unsigned int dst_id;
                string msg;
                // packet *_pkt;
        };
        
        void print () const;
};
SDN_data_pkt_gen_event::SDN_data_pkt_gen_event_generator SDN_data_pkt_gen_event::SDN_data_pkt_gen_event_generator::sample;

void SDN_data_pkt_gen_event::trigger() {
    if (node::id_to_node(src) == nullptr){
        cerr << "SDN_data_pkt_gen_event error: no node " << src << "!" << endl;
        return ;
    }
    else if ( dst != BROCAST_ID && node::id_to_node(dst) == nullptr ) {
        cerr << "SDN_data_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }
    
    SDN_data_packet *pkt = dynamic_cast<SDN_data_packet*> ( packet::packet_generator::generate("SDN_data_packet") );
    if (pkt == nullptr) { 
        cerr << "packet type is incorrect" << endl; return; 
    }
    SDN_data_header *hdr = dynamic_cast<SDN_data_header*> ( pkt->getHeader() );
    SDN_data_payload *pld = dynamic_cast<SDN_data_payload*> ( pkt->getPayload() );
    
    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
    hdr->setNexID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

    pld->setMsg(msg);
    
    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src; // to make the packet start from the src
    e_data._pkt = pkt;
    
    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event", trigger_time, (void *)&e_data) );
    if(e == nullptr)
        cout << "event type is incorrect" << endl;
}
unsigned int SDN_data_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string (dst) ; //to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the SDN_data_pkt_gen_event::print() function is used for log file
void SDN_data_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   SDN_data_packet generating"
         << endl;
}

class SDN_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration
            
    private:
        SDN_ctrl_pkt_gen_event (SDN_ctrl_pkt_gen_event &){}
        SDN_ctrl_pkt_gen_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst 
        unsigned int mat;
        unsigned int act;
        // packet *pkt; // the packet
        string msg;
    
    protected:
        SDN_ctrl_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            mat = data_ptr->mat_id;
            act = data_ptr->act_id;
            msg = data_ptr->msg;
        } 
        
    public:
        virtual ~SDN_ctrl_pkt_gen_event(){}
        // SDN_ctrl_pkt_gen_event will trigger the packet gen function
        virtual void trigger();
        
        unsigned int event_priority() const;
        
        class SDN_ctrl_pkt_gen_event_generator;
        friend class SDN_ctrl_pkt_gen_event_generator;
        // SDN_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class SDN_ctrl_pkt_gen_event_generator : public event_generator{
                static SDN_ctrl_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                SDN_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){ 
                    // cout << "send_event generated" << endl; 
                    return new SDN_ctrl_pkt_gen_event(_trigger_time, data); 
                }
            
            public:
                virtual string type() { return "SDN_ctrl_pkt_gen_event";}
                ~SDN_ctrl_pkt_gen_event_generator(){}
        };
        // this class is used to initialize the SDN_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the controller
                unsigned int dst_id; // the node that should update its rule
                unsigned int mat_id; // the target of the rule
                unsigned int act_id; // the next hop toward the target recorded in the rule
                string msg;
                // packet *_pkt;
        };
        
        void print () const;
};
SDN_ctrl_pkt_gen_event::SDN_ctrl_pkt_gen_event_generator SDN_ctrl_pkt_gen_event::SDN_ctrl_pkt_gen_event_generator::sample;

void SDN_ctrl_pkt_gen_event::trigger() {
    if ( dst == BROCAST_ID || node::id_to_node(dst) == nullptr ) {
        cerr << "SDN_ctrl_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }
    
    SDN_ctrl_packet *pkt = dynamic_cast<SDN_ctrl_packet*> ( packet::packet_generator::generate("SDN_ctrl_packet") );
    if (pkt == nullptr) { 
        cerr << "packet type is incorrect" << endl; return; 
    }
    SDN_ctrl_header *hdr = dynamic_cast<SDN_ctrl_header*> ( pkt->getHeader() );
    SDN_ctrl_payload *pld = dynamic_cast<SDN_ctrl_payload*> ( pkt->getPayload() );
    
    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src); 
    hdr->setDstID(dst);
    hdr->setPreID(src);
    hdr->setNexID(src);

    pld->setMsg(msg);
    pld->setMatID(mat);
    pld->setActID(act);
    
    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;
    
    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
    if(e == nullptr)
        cout << "event type is incorrect" << endl;
}
unsigned int SDN_ctrl_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the SDN_ctrl_pkt_gen_event::print() function is used for log file
void SDN_ctrl_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime() 
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   conID"       << setw(11) << src 
         << "   dstID"       << setw(11) << dst
         << "   matID"       << setw(11) << mat
         << "   actID"       << setw(11) << act
         << "   SDN_ctrl_packet generating"
         << endl;
}

////////////////////////////////////////////////////////////////////////////////

class link {
        // all links created in the program
        static map< pair<unsigned int,unsigned int>, link*> id_id_link_table;
        
        unsigned int id1; // from
        unsigned int id2; // to
        
    protected:
        link(link&){} // this constructor should not be used
        link(){} // this constructor should not be used
        link(unsigned int _id1, unsigned int _id2): id1(_id1), id2(_id2) { id_id_link_table[pair<unsigned int,unsigned int>(id1,id2)] = this; }

    public:
        virtual ~link() { 
            id_id_link_table.erase (pair<unsigned int,unsigned int>(id1,id2)); // erase the link
        }
        
        static link * id_id_to_link (unsigned int _id1, unsigned int _id2) { 
            return ((id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()) ? id_id_link_table[pair<unsigned,unsigned>(_id1,_id2)]: nullptr) ; 
        }

        virtual double getLatency() = 0; // you must implement your own latency
        
        static void del_link (unsigned int _id1, unsigned int _id2) {
            pair<unsigned int, unsigned int> temp;
            if (id_id_link_table.find(temp)!=id_id_link_table.end())
                id_id_link_table.erase(temp); 
        }

        static unsigned int getLinkNum () { return id_id_link_table.size(); }

        class link_generator {
                // lock the copy constructor
                link_generator(link_generator &){}
                // store all possible types of link
                static map<string,link_generator*> prototypes;
            protected:
                // allow derived class to use it
                link_generator() {}
                // after you create a new link type, please register the factory of this link type by this function
                void register_link_type(link_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your link
                virtual link* generate(unsigned int _id1, unsigned int _id2) = 0;
            public:
                // you have to implement your own type() to return your link type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of link derived
        	    static link * generate (string type, unsigned int _id1, unsigned int _id2) {
        	        if(id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()){
        	            std::cerr << "duplicate link id" << std::endl; // link id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id1 || BROCAST_ID == _id2 ) {
        	            cerr << "BROCAST_ID cannot be used" << endl;
        	            return nullptr;
        	        }
            		else if (prototypes.find(type) != prototypes.end()){ // if this type derived exists 
            		    link * created_link = prototypes[type]->generate(_id1,_id2);
            			return created_link; // generate it!!
            		}
            		std::cerr << "no such link type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered link types: " << endl;
            	    for (map<string,link::link_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~link_generator(){};
        };
};
map<string,link::link_generator*> link::link_generator::prototypes;
map<pair<unsigned int,unsigned int>, link*> link::id_id_link_table;

void node::add_phy_neighbor (unsigned int _id, string link_type){
    if (id == _id) return; // if the two nodes are the same...
    if (id_node_table.find(_id)==id_node_table.end()) return; // if this node does not exist
    if (phy_neighbors.find(_id)!=phy_neighbors.end()) return; // if this neighbor has been added
    phy_neighbors[_id] = true;
    
    link::link_generator::generate(link_type,id,_id);
}
void node::del_phy_neighbor (unsigned int _id){
    phy_neighbors.erase(_id);
    
}


class simple_link: public link {
    protected:
        simple_link() {} // it should not be used outside the class
        simple_link(simple_link&) {} // it should not be used
        simple_link(unsigned int _id1, unsigned int _id2): link (_id1,_id2){} // this constructor cannot be directly called by users
    
    public:
        virtual ~simple_link() {}
        virtual double getLatency() { return ONE_HOP_DELAY; } // you can implement your own latency
        
        class simple_link_generator;
        friend class simple_link_generator;
        // simple_link is derived from link_generator to generate a link
        class simple_link_generator : public link_generator {
                static simple_link_generator sample;
                // this constructor is only for sample to register this link type
                simple_link_generator() { /*cout << "simple_link registered" << endl;*/ register_link_type(&sample); }
            protected:
                virtual link * generate(unsigned int _id1, unsigned int _id2) 
                { /*cout << "simple_link generated" << endl;*/ return new simple_link(_id1,_id2); }
            public:
                virtual string type() { return "simple_link"; }
                ~simple_link_generator(){}
        };
};

simple_link::simple_link_generator simple_link::simple_link_generator::sample;


// the data_packet_event function is used to add an initial event
void data_packet_event (unsigned int src, unsigned int dst, unsigned int t = 0, string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr) ) {
        cerr << "src or dst is incorrect" << endl; return ;
        return;
    }

    SDN_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;
    
    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    SDN_data_pkt_gen_event *e = dynamic_cast<SDN_data_pkt_gen_event*> ( event::event_generator::generate("SDN_data_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

// the ctrl_packet_event function is used to add an initial event
void ctrl_packet_event (unsigned int con_id, unsigned int id, unsigned int mat, unsigned int act, unsigned int t = event::getCurTime(), string msg = "default"){
// void ctrl_packet_event (unsigned int dst, unsigned int t = 0, string msg = "default"){
    if ( id == BROCAST_ID || node::id_to_node(id) == nullptr ) {
        cerr << "id is incorrect" << endl; return;
    }
    
    //msg = to_string(mat) + " " + to_string(act);
    unsigned int src = con_id;
    SDN_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = id;
    e_data.mat_id = mat;
    e_data.act_id = act;
    e_data.msg = msg;
    
    SDN_ctrl_pkt_gen_event *e = dynamic_cast<SDN_ctrl_pkt_gen_event*> ( event::event_generator::generate("SDN_ctrl_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void node::send_handler(packet *p){
    //cout << "tp->type()" << p->type() << endl;
    packet *_p = packet::packet_generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->getHeader()->getPreID();
    e_data.r_id = _p->getHeader()->getNexID();
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event*> (event::event_generator::generate("send_event",event::getCurTime(), (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

void node::send(packet *p){ // this function is called by event; not for the user
    if (p == nullptr) return;
    
    unsigned int _nexID = p->getHeader()->getNexID();
    //cout << "node::send, nextID: " << _nexID << endl; 
    for ( map<unsigned int,bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it ++) {
        unsigned int nb_id = it->first; // neighbor id
        
        if (nb_id != _nexID && BROCAST_ID != _nexID) continue; // this neighbor will not receive the packet
        
        unsigned int trigger_time = event::getCurTime() + link::id_id_to_link(id, nb_id)->getLatency() ; // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << endl;
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)
        
        packet *p2 = packet::packet_generator::replicate(p);
        e_data._pkt = p2;
        
        recv_event *e = dynamic_cast<recv_event*> (event::event_generator::generate("recv_event", trigger_time, (void*) &e_data)); // send the packet to the neighbor
        if (e == nullptr) cerr << "event type is incorrect" << endl;
    }
    packet::discard(p);
}


/*
    // (以 packet 流向的方向為定義，先到的為 parent，後到的為 child) 

    針對目前輸入的 node id 的 pre node 的 next node 做 traversal，
    如果前一個 node 已經 send ctrl packet to switch，則進入pre node，遞迴繼續 DetermineRoute，
    如果前一個 node 還沒有 send ctrl packet to switch，則把 pre node push 到 arr
    如果 arr 不為空，則把 arr 裡面的 node 都送 ctrl packet to switch，並讓 pre node state change to 1 (send to switch)。
    圖解來說，就是 dst 的 pre node 如果有人沒有更新 route (not send ctrl packet to switch yet)，先 push to array，先著重於 如果有人已經 send ctrl packet 但其 parent 尚未送，則先送 parent 的 ctrl packet
    也就是先從最外層開始更新（graph 的 leaf），root 為 dst(mat)，最外層先更新好 path 就不會有 loop 問題。
    這樣的做法確保 parents 都已經送出 ctrl packet，
    也就是，parent 的 route table 都更新完整，此時從 parent 流向 child 的 packet 確保不會有 loop 產生。
    這樣的好處是可以在 upTime 時可以有 data 流入並依照當時有的 route table 去走到 dst，同時也可以邊 updating network，並確保不會有 loop 發生。
*/
void DetermineRout(int con_id, unsigned int id, unsigned int mat, int* _r_arr) { //only in update time, w_order=2
    //w_order == 1, so there must be two different graph for all dests
    deque<int> arr;
    unsigned int _mat = mat;
    
    if(_r_arr[id] == 2) {
        for(auto it = node::id_to_node(id)->PreNode[_mat].begin(); it != node::id_to_node(id)->PreNode[_mat].end(); it++) {
            if(_r_arr[*it] != 0) {//already sent
                DetermineRout(con_id, *it, mat, _r_arr);
                continue;
            }
            else if(_r_arr[*it] == 0) {
                // _r_arr[*it] = 1;
                arr.push_back(*it);
                //_id.push_back(*it);
            }
        }
        if(arr.empty() == 0) {
            while(!arr.empty()) {
                ctrl_packet_event(con_id, arr.front(), _mat, node::id_to_node(arr.front())->table[_mat]);
                _r_arr[*it] = 1;
                arr.pop_front();
            }
        }
    }
}  

void SDN_controller::recv_handler (packet *p) {
    
    SDN_ctrl_payload *l3 = nullptr;
    l3 = dynamic_cast<SDN_ctrl_payload*> (p->getPayload());
    unsigned int mat = l3->getMatID();
    unsigned int act = l3->getActID();

    if(p == nullptr) return;
    if(p->type() == "SDN_ctrl_packet" && p->getHeader()->getDstID() == getNodeID()) { //this is ack
        // _r_arr is use to record the r_arr state, when go in DetermineRout
        // if _r_arr == 1, can be send to controller. If recurrent, it won't be send again.
        // if it really send to controller, r_arr will be 1. 
        int* _r_arr = new int[getNodeNum()-1]();
        for(int i = 0; i < getNodeNum()-1; i++)//transmit r_arr to _r_arr
            _r_arr[i] = r_arr[mat][i];
        _r_arr[mat] = 2; // we need to let _r_arr[mat] always be 2, so in determine route we can processing
        
        if(n_update[mat].empty() == 0) {
            // because this is ack, we need to pop the finished node
            n_update[mat].pop_front();

            if(n_update[mat].empty()) {
                // if all the node in the n_update is updated...
                int i;
                for(i = 0; i<getNodeNum()-1; i++) {
                    if(i == mat) continue; // node(i) is mat...
                    if(r_arr[mat][i] == 2) // node(i) is updated...
                        continue;
                    else break; // node(i) is not updated yet...
                }
                if(i == getNodeNum()-1) // all node is visited, let r_arr reset
                    for(i = 0; i<getNodeNum()-1; i++) 
                        r_arr[mat][i] = 0;
                // the round is finish, next round!
                if(r_arr[mat][mat] != 2) { // when n_update is empty, let we do the mat round!
                    DetermineRout(getNodeNum()-1, mat, node::id_to_node(mat)->table[mat], _r_arr);
                    r_arr[mat][mat] = 2; // after DetermineRout, node(mat) is updated!
                    return;
                }   
            } 
            /*
                after the first round update, r_arr[mat][mat] will become 2.
                Everytime we get the ack packet, we check whether pre node's parent's route table is update, if not, parents send ctrl packe to switch to update the route
            */
            if(r_arr[mat][mat] == 2){ 
                DetermineRout(getNodeNum()-1, p->getHeader()->getPreID(), node::id_to_node(mat)->table[mat], _r_arr);
            }
        }
        
    }
    else if(p->type() == "SDN_ctrl_packet" && (r_arr[mat][p->getHeader()->getDstID()] == 0)) { //non-ack, send to switch
        
        SDN_ctrl_packet * p2 = nullptr;
        p2 = dynamic_cast<SDN_ctrl_packet*> (p);
        SDN_ctrl_payload* l2 = nullptr;
        l2 = dynamic_cast<SDN_ctrl_payload*> (p2->getPayload());
        //cout << "msg: " << l2->getMsg() << endl;
        if(l2->getMsg() == "first round") {
            r_arr[mat][mat] = 0;
        }
        p2->getHeader()->setPreID ( getNodeID());
        p2->getHeader()->setNexID ( p->getHeader()->getDstID());
        p2->getHeader()->setDstID ( p->getHeader()->getDstID());

        r_arr[mat][p->getHeader()->getDstID()] = 1;
        n_update[mat].push_back(p2->getHeader()->getDstID());
        //cout << "push: " <<  p2->getHeader()->getDstID() << endl;
        send_handler(p2);
    }
}

bool find_key(map<unsigned int, unsigned int> &m, unsigned int dest) {
    for(auto it = m.begin(); it != m.end(); it++) {
        if(it->first == dest)
            return true;
    }
    return false;
}
// you have to write the code in recv_handler of SDN_switch
void SDN_switch::recv_handler (packet *p){

    if (p == nullptr) return ;
    
    if (p->type() == "SDN_data_packet" ) { // the switch receives a packet from the other switch
        // cout << "node " << getNodeID() << " send the packet" << endl;
        
        SDN_data_packet * p2 = nullptr;
        p2 = dynamic_cast<SDN_data_packet*> (p);

        if(!find_key(r_table, p->getHeader()->getDstID())) {
            //cout << "fail!!!" << endl;
            return;
        }
        p2->getHeader()->setPreID ( getNodeID() );
        p2->getHeader()->setNexID ( r_table[p->getHeader()->getDstID()] );
        p2->getHeader()->setDstID ( p->getHeader()->getDstID() );

        send_handler (p2);
    }
    else if (p->type() == "SDN_ctrl_packet") { // the switch receives a packet from the controller
        
        //cout << "send by: " << p->getHeader()->getSrcID() << endl;
        SDN_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_ctrl_packet*> (p); 
        SDN_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<SDN_ctrl_payload*> (p3->getPayload());
        string msg = l3->getMsg();

        unsigned mat = l3->getMatID();//dst
        unsigned act = l3->getActID();//next
        //cout << "mat: " << mat << endl;
        //cout << "act: " << act << endl;
        if(find_key(r_table, mat)) {//該dest已經存在
            r_table[mat] = act;
        }
        else {
            r_table.insert(pair<unsigned int, unsigned int>(mat, act));
        }
        
        //send back to controller, reverse the pre and next, set con_id as dst
        unsigned int pre = p->getHeader()->getPreID();
        p3->getHeader()->setDstID(p->getHeader()->getSrcID());
        p3->getHeader()->setPreID(p->getHeader()->getNexID());
        p3->getHeader()->setNexID(pre);
        r_arr[mat][p3->getHeader()->getPreID()] = 2;//send!
        
        send_handler(p3);
    }   

}

void Build_Table(unsigned int dest, unsigned int nodes, int w_order) {
    //Using dijkstra algo
    vector<pair<unsigned int, unsigned int> >::iterator iter, iter_end;
    vector<int> des;//des代表到各點距離 會隨著演算法更新 -1代表無限大
    for(unsigned int i = 0; i < nodes; i++) {
        //let the destination to the other nodes length set to -1 
        if(node::id_to_node(i) == nullptr)
            cout << "null!" << endl;
        des.push_back(-1);
        node::id_to_node(i)->flag = 0;
        
    }
    des[dest] = 0;
    unsigned int i = dest;
    while(1) {
        //cout << " nextID: " << i << endl;
        if(w_order == 0) {
            iter = node::id_to_node(i)->oldW.begin();
            iter_end = node::id_to_node(i)->oldW.end();
        }
        else {
            iter = node::id_to_node(i)->newW.begin();
            iter_end = node::id_to_node(i)->newW.end();
        }
        for(auto it = iter; it != iter_end; it++) {
            //destribute the way
            int w = it->second; //weight
            if(des[it->first] == -1) { 
                //next node not visit yet
                des[it->first] = w + des[i];
                node::id_to_node(it->first)->table[dest] = i;
                node::id_to_node(i)->Pre_num[dest]++;
                node::id_to_node(i)->PreNode[dest].push_back(it->first);
            }    
            else if((des[it->first] != -1) && (des[i] + w < des[it->first])) { 
                //visited 需要update 新的way較小
                des[it->first] = des[i] + w;
                node::id_to_node(node::id_to_node(it->first)->table[dest])->Pre_num[dest]--;
                //cout << "pre_num: " << node::id_to_node(node::id_to_node(it->first)->table[dest])->Pre_num[dest] << endl;
                auto it2 = find(node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].begin(), 
                                node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].end(),
                                it->first);
                
                node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].erase(it2);
                node::id_to_node(it->first)->table[dest] = i;
                node::id_to_node(i)->Pre_num[dest]++;
                node::id_to_node(i)->PreNode[dest].push_back(it->first);
            }
            else if(des[i] + w == des[it->first]) {//already visited
                //相等時，取前一個node id小的
                if(i < node::id_to_node(it->first)->table[dest]) { 
                    node::id_to_node(node::id_to_node(it->first)->table[dest])->Pre_num[dest]--;
                    //cout << "pre_num: " << node::id_to_node(node::id_to_node(it->first)->table[dest])->Pre_num[dest] << endl;
                    auto it2 = find(node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].begin(), 
                                    node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].end(),
                                    it->first);
                    
                    node::id_to_node(node::id_to_node(it->first)->table[dest])->PreNode[dest].erase(it2);
                    node::id_to_node(it->first)->table[dest] = i;
                    node::id_to_node(i)->Pre_num[dest]++;
                    node::id_to_node(i)->PreNode[dest].push_back(it->first);
                }
            }
        }
        //if i==dest, there is no next id, so it won't go into the it loop
        if(i == dest) {
            node::id_to_node(i)->table[dest] = dest;
            //node::id_to_node(i)->Pre_num[dest]++;
        }
        //cout << "table: id " << i << " next " << node::id_to_node(i)->table[dest] << endl;  
        int pos = -1, min = 5000; 
        //尋找已拜訪node到dest中最短path當作下一次起始點
        for(unsigned int k = 0; k < nodes; k++) {
            if(node::id_to_node(k)->flag == 1) continue; //碰到flag = 1就繼續
            if(des[k] == -1) continue; //not visited yet
            if(k == dest) continue;
            if(des[k] < min) {
                min = des[k]; pos = k;
            }
        }
        if(pos == -1) break;
        node::id_to_node(pos)->flag = 1; //flag = 1 確定dest到該node為最短path 並且將該點作為下一輪的起始
        i = pos; //如果有兩個way sum相同 會先取id小的
    }
}

class Data {
    unsigned int t, src, dst;
    public:
        void SetData(unsigned int _t, unsigned int _src, unsigned int _dst) {
            t = _t;
            src = _src;
            dst = _dst;
        }
        unsigned int getTime() {
            return t;
        }
        unsigned int getSrc() {
            return src;
        }
        unsigned int getDst() {
            return dst;
        }
};

/*
    在 first round send 當中，讓在 old graph 為 leaf 的 node 優先送出 ctrl packet 更新 route，
    可以確保絕對不會產生 loop。
*/
void first_round_send(int con_id, int nodes, int dests, vector<unsigned int> dest, vector<pair<int ,int> > temp_table[], map<int, int>* temp_pre_num, unsigned int UpdTime) {
    // first round to update the route table
    for(int i = 0; i < dest.size(); i++){
        int mat = dest[i];
        int act; 
        for(int id = 0; id < nodes; id++) {
            if(id == mat)
                continue;
            // int it = temp_table[node::id_to_node(id)->table[i]].at(i).second; // current node point to node _n, if _n's old next point to current id, loop.
            act = node::id_to_node(id)->table[mat];
            if(temp_pre_num[id][mat] == 0) { //in old graph, node[id] is leaf
                ctrl_packet_event(con_id, id, mat, act, UpdTime, "first round");
            }
        }
    }
    //cout << "finished" << endl;
}

int main()
{
    unsigned int nodes, dests, links;
    cin >> nodes >> dests >> links;
    // read the input and generate switch node
    for (unsigned int id = 0; id < nodes; id ++){
        node::node_generator::generate("SDN_switch",id);
    }
    
    unsigned int InsTime, UpdTime, SimDuration;
    cin >> InsTime >> UpdTime >> SimDuration; //start installing routing table time(using ctrl packet), update routing table time
    vector<unsigned int>dest;
    unsigned int _dest;
    for (unsigned int i = 0; i < dests; i++) {
        cin >> _dest; 
        dest.push_back(_dest);
        node::id_to_node(i)->table.insert(pair<unsigned int, unsigned int>(dest.back(), 0));
    }

    unsigned int con_id = node::getNodeNum();
    node::node_generator::generate("SDN_controller", con_id);
    for(int i = 0; i < nodes; i++) {
        node::id_to_node(i)->add_phy_neighbor(con_id);
        node::id_to_node(con_id)->add_phy_neighbor(i);
        for(int j = 0; j<dests;j++)
            node::id_to_node(i)->Pre_num[dest[j]] = 0;//initial all to false
    }
    //node::node_generator::print(); // print all registered nodes
    //be aware to the dest, whether is exist in the table
    // set switches' neighbors
    for (unsigned int i = 0; i < links; i++) {
        unsigned int n1, n2, n3, oldW, newW;
        cin >> n1;//input link_id
        cin >> n2;//input node_1 id
        cin >> n3;//input node_2 id
        cin >> oldW >> newW;
        if(node::id_to_node(n2) == nullptr || node::id_to_node(n3) == nullptr)
            continue;
        node::id_to_node(n2)->add_phy_neighbor(n3);
        node::id_to_node(n3)->add_phy_neighbor(n2);
        node::id_to_node(n2)->oldW.push_back(make_pair(n3, oldW));
        node::id_to_node(n3)->oldW.push_back(make_pair(n2, oldW));
        node::id_to_node(n2)->newW.push_back(make_pair(n3, newW));
        node::id_to_node(n3)->newW.push_back(make_pair(n2, newW));
    }

    unsigned int i = 0;
    for(auto it = dest.begin(); it != dest.end(); it++, i++) {
        if(node::id_to_node(dest[i]) == nullptr) {
            //if the dest is not in the graph, then delete it.
            dest.erase(it);
        }
    }
    unsigned int _dst, _src, _t, ctrl_t = 0, count = 0,
    dst, src, t;//t = triggerTime
    Data* input = new Data[10000]; //suppose there will be 10000 inputs
    // start input real traffic
    while(cin >> _t >> _src >> _dst) {
        input[count].SetData(_t, _src, _dst);
        count++;
    }
    unsigned int input_size = count;   
    
    count = 0;
    bool ctrl_flag[2] = {0}; // initial false
    // ctrl_flag: index用來判斷是第一次 (ctrl_flag[0]) (install time) 還是第二次 (ctrl_flag[1]) (update time)，已送出 ctrl_packet 的話會更新 ctrl_flag，值會從 0=>1。 ex: 如果是在 InsTime 跟 upTime 區間間，只會進行一次 routing update
    int w_order = 0; // weight order
    vector<pair<int ,int> > temp_table[10000]; // 用來記錄第一次的table，如果第二次更新有相同的 msg 則略過
    map<int, int> temp_pre_num[10000];
    static int* record_arr = new int[10000]();
    
    /*
        Value of r_arr explaination:
        0 -> initial state
        1 -> send ctrl packet to switch
        2 -> ack, return from switch to controller

        if all the route of this round is over, all the packet has mark with ack, then go next round.
    */

     /* 
        -------------------------
        START SIMULATION!!! IMPLEMENT IN THE while loop
        -------------------------
    */
    while(1) {
        
        /* 
            -------------------------
            DETRERMINE THE w_order ACCORDING TO THE TIME t,
            THEN DO THE CORRESPONDING WORKS => Build route table or Clear the parameters from first time installation.
            -------------------------
        */
        if(input_size != 0) { // exist real traffic in the network
            t = input[count].getTime();//trigger time
            src = input[count].getSrc();
            dst = input[count].getDst();
            if(t >= UpdTime) {
                ctrl_t = UpdTime;
                w_order = 1; // update the route table!
            }
            else if(t >= InsTime && t < UpdTime) {
                ctrl_t = InsTime;
                w_order = 0; // install route table, using dijktra
            }
            else if(t < InsTime) {
                ctrl_t = InsTime;
                w_order = -1; // not install route table yet!
            }
        }
        else { // no real traffic in the network
            if(w_order == 0) {
                t = InsTime;
                ctrl_t = t;
            }
            else if(w_order == 1) {
                t = UpdTime;
                ctrl_t = t;
            }
        }
        
        if(w_order == 1 && ctrl_flag[0] == 1 && ctrl_flag[1] == 0) { // 已經 install route table，要進行 update route table
            // 清空第一次（install time）建 route table 時所紀錄的 Pre_num 與 PreNode
            for(int i = 0; i < dests; i++) {
                for(int id = 0; id < nodes; id++) {
                    node::id_to_node(id)->Pre_num[dest[i]] = 0;
                    node::id_to_node(id)->PreNode[dest[i]].clear();
                }
            }
        }
        
        if(ctrl_flag[w_order] == 0 && w_order != (-1)) { // this table(new/old) is not build yet, then build it!
            for(unsigned int i = 0; i < dests; i++) {
                Build_Table(dest[i], nodes, w_order); // using dijktra, build the route table, pre_num and PreNode will also given in this part, w_order will determine using old weight or new weight to build the table
                for(unsigned int k = 0; k < node::getNodeNum(); k++)
                    node::id_to_node(k)->initial_flag();
            } 
        }
        
        /* 
            -------------------------
            HANDLING THE DATA PACKET
            -------------------------
        */
        if(t < ctrl_t) { // send data packet first, but there is no route table to follow, so can't send successfully
            if(input_size != 0) {
                data_packet_event(src, dst, t, "~");
            }
                
        }
        else if(t >= ctrl_t) {
            if(!ctrl_flag[w_order]) { // ctrl packet not send yet, sned it!
               if(w_order == 0) { // installation time
                   for(unsigned int id = 0; id < nodes; id++) { 
                        // in second update, id need to be handling in right order. => using DetermineRout()
                        int* mat = new int[dests];
                        int* act = new int[dests]; 
                        int _flag = 0;
                        for(unsigned int i = 0; i < dests; i++) {
                            if(id == dest[i]) { // id 等於 dst 時略過
                                mat[i] = -1;
                                act[i] = -1;
                                continue;
                            }
                            mat[i] = dest[i]; //desID
                            act[i] = node::id_to_node(id)->table[dest[i]]; //nexIDs
                        }
                        
                        // given value to temp_pre_num and temp_table
                        for(unsigned int i = 0; i < dests; i++) {
                            if(mat[i] == -1 || act[i] == -1) {//id = dst
                                temp_pre_num[id].insert(make_pair(dest[i], node::id_to_node(id)->Pre_num[dest[i]])); // record the first time pre num
                                continue;
                            }
                            
                            temp_table[id].push_back(make_pair(mat[i], act[i])); // 紀錄第一次的table(w_order = 0)
                            temp_pre_num[id].insert(make_pair(mat[i], node::id_to_node(id)->Pre_num[dest[i]])); // record the first time pre num
                            

                            ctrl_packet_event(con_id, id, mat[i], act[i], InsTime); //update at update time t
                        }
                        delete[] mat;
                        delete[] act;
                    }//for id    
                
               }    
               else if(w_order == 1) { // update time
                    first_round_send(con_id, nodes, dests, dest, temp_table, temp_pre_num, UpdTime);
               }
                
            }
            ctrl_flag[w_order] = 1;
            //else keep sending data packet
            if(input_size != 0)
                data_packet_event(src, dst, t, "~");
        }
        count++;
        if(input_size == 0 && ctrl_flag[0] == 1 && ctrl_flag[1] == 0) 
            w_order = 1;
        if(input_size == 0 && ctrl_flag[0] == 1 && ctrl_flag[1] == 1)
            break;
        if(count == input_size)
            break;
    }
    // start simulation!!
    event::start_simulate(SimDuration);
    // event::flush_events() ;
    // cout << packet::getLivePacketNum() << endl;
    return 0;
}


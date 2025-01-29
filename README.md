# SDN-Routing-Path-and-Updating-Network
This is the project from "Object-Oriented Programming" at CCU
## Input
- Number of nodes, destinations and links
- Install Time, Update Time, Simulation Duration
- Input each link id and corresponding nodes pair
- Input each real packet start time, src node, dst node
## Output
- All packets transmitted in the network
- Three types of packet: data packets, ctrl packets, update packets
  - data packets:
    ```C=
    void data_packet_event(unsigned int src, unsigned int dst, unsigned int t, string msg="default)
    ```
    - A packet will be generated for a source and sent to a destination at time t.
  - ctrl packets:
    ```C=
    void ctrl_paccket_event(unsigned int con_id, unsigned int id, unsigned int mat, unsigned int act, unsigned int t=event::getCurTime(), string msg="default)
    ```
    - A packet will be genereated for the controller and sent to the node (id) at time t (optional) to update a specific rule in the node's table
    - mat: the destination in the node's routing table
    - act: the next hop in the node's routing table
    - The controller will receive the SDN ctrl packet first (since it's src)
  - update packet:
    - For each update round
      - Send the SDN ctrl and update packet from the controller
      - Record the SDN ctrl packet that have been sent from the controller
    - After update the rule in the table of each node
      - Exchange the src and dst in the header
      - Send the packet back to the controller from the node
      - i.e., ack for the update
    - After the controller receives the ack
      - Record the SDN ctrl packets that have been acked
      - If all the SDN ctrl packets in the last round are acked, then SDN controller sends the SDN ctrl packets in the next round
## Update route algorithm
- We first install the route table using dijkstra.
- The main algorithm is in function `DetermineRout`. You should trace code from function `SDN_controller::recv_handler`.

## Note
- Providing sample inputs and outputs for reference.
